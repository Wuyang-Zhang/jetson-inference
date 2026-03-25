/*
 * Copyright (c) 2017, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
*/
// 只是“调用已经存在的模型”
// 网络结构和权重都在模型文件里，segNet 只负责把它们交给 TensorRT
// segNet 从头到尾都不是“手写网络层”的代码

/* 输入参数（路径/名字）
  ↓
判断模型类型（内置 / 文件）
  ↓
new segNet 实例
  ↓
LoadNetwork（核心：TensorRT）
  ↓
解析输出 tensor 结构
  ↓
分配 GPU/CPU共享内存
  ↓
加载 labels / colors
  ↓
初始化参数
  ↓
返回 net */


#include "segNet.h"
#include "tensorConvert.h"
#include "modelDownloader.h"

#include "cudaMappedMemory.h"
#include "cudaOverlay.h"
#include "cudaResize.h"
#include "cudaFont.h"

#include "commandLine.h"
#include "filesystem.h"
#include "imageIO.h"

/*
 * segNet.cpp 是主机侧的流程控制层：
 *   1. 解析并定位用户指定的模型
 *   2. 调用 tensorNet 加载/运行 TensorRT
 *   3. 做分割特有的后处理（argmax -> mClassMap）
 *   4. 把可视化工作分发给 segNet.cu
 *
 * 高层数据流：
 *   输入图像(GPU) -> mInputs[0].CUDA(GPU) -> TensorRT 输出(mOutputs[0].CPU/GPU)
 *                -> mClassMap(CPU/GPU 共享) -> 可视化输出(GPU)
 */


//--------------------------------------------------------------------------------------//
// constructor
segNet::segNet() : tensorNet()
{
	// 构造阶段只做成员初始化，不做真正的模型加载。
	// 真正的模型解析、TensorRT engine 创建、颜色表加载，都在后面的 Create() 里完成。

	// 最近一次输入图像的缓存先清空。
	// 这样在第一次 Process() 之前，如果误调用 Overlay()，就能正确报错。
	mLastInputImg    = NULL;
	mLastInputWidth  = 0;
	mLastInputHeight = 0;
	mLastInputFormat = IMAGE_UNKNOWN;

	// 类别颜色相关的缓冲区先置空。
	// 它们会在模型加载完成、知道类别数之后再分配。
	mColorsAlphaSet = NULL;
	mClassColors    = NULL;
	mClassMap       = NULL;
}

//--------------------------------------------------------------------------------------//
// destructor
segNet::~segNet()
{
	// mClassColors 和 mClassMap 都是用 cudaAllocMapped()/LoadClassColors()
	// 分配出来的共享映射内存，所以这里用 CUDA_FREE_HOST 释放。
	CUDA_FREE_HOST(mClassColors);
	CUDA_FREE_HOST(mClassMap);
	
	// mColorsAlphaSet 是普通 CPU 内存，用 free() 释放。
	if( mColorsAlphaSet != NULL )
	{
		free(mColorsAlphaSet);
		mColorsAlphaSet = NULL;
	}
}

//--------------------------------------------------------------------------------------//
// VisualizationFlagsFromStr
uint32_t segNet::VisualizationFlagsFromStr( const char* str_user, uint32_t default_value )
{
	// 如果用户没传字符串，就直接返回默认可视化选项。
	if( !str_user )
		return default_value;

	// 先拿到字符串长度。
	// 后面 strtok() 会改写字符串内容，所以不能直接在原始只读字符串上操作。
	const size_t str_length = strlen(str_user);

	// 空字符串也没有解析意义，直接回退到默认值。
	if( str_length == 0 )
		return default_value;

	// 分配一个临时可写缓冲区，用来拷贝用户传入的字符串。
	char* str = (char*)malloc(str_length + 1);

	// 分配失败时，稳妥地返回默认值。
	if( !str )
		return default_value;

	// 把输入字符串拷贝进临时缓冲区，后面会原地切分它。
	strcpy(str, str_user);

	// 支持用 ',' 或 '|' 分隔多个可视化选项。
	// 例如 "overlay,mask" 或 "overlay|mask"。
	const char* delimiters = ",|";
	char* token = strtok(str, delimiters);

	// 如果连第一个 token 都没有拿到，说明解析失败，回退默认值。
	if( !token )
	{
		free(str);
		return default_value;
	}

	// flags 初始为 0，后面根据识别到的关键字逐步置位。
	uint32_t flags = 0;

	while( token != NULL )
	{
		// 识别 "overlay"。
		if( strcasecmp(token, "overlay") == 0 )
			flags |= VISUALIZE_OVERLAY;
		// 识别 "mask"。
		else if( strcasecmp(token, "mask") == 0 )
			flags |= VISUALIZE_MASK;

		// 继续拿下一个 token。
		token = strtok(NULL, delimiters);
	}	

	// 临时缓冲区用完后释放。
	free(str);

	// 返回解析出的位标志组合。
	return flags;
}

//--------------------------------------------------------------------------------------//
// FilterModeFromStr
segNet::FilterMode segNet::FilterModeFromStr( const char* str, FilterMode default_value )
{
	// 未传入字符串时，直接用默认滤波模式。
	if( !str )
		return default_value;

	// "point" 表示最近邻采样。
	if( strcasecmp(str, "point") == 0 )
		return segNet::FILTER_POINT;
	// "linear" 表示双线性插值。
	else if( strcasecmp(str, "linear") == 0 )
		return segNet::FILTER_LINEAR;

	// 无法识别时，回退到默认值。
	return default_value;
}


//--------------------------------------------------------------------------------------//
// 把“用户输入的语义（模型名）”转换为“系统可执行的具体配置（路径+参数）”
// Create：一个“高层工厂函数”，把“模型名字 → 真实模型路径”，再调用底层 Create 完成真正构建
// segNet::Create(模型名字)  → 解析 JSON → 拼路径 → → segNet::Create(真实路径参数)
segNet* segNet::Create( const char* network, uint32_t maxBatchSize,
				    precisionType precision, deviceType device, bool allowGPUFallback )
{
	// 这里处理“内置模型名”这种高层入口。
	// 如果用户传的是 fcn-resnet18-voc 这类名字，就先从 models.json
	// 解析出真实模型路径、labels、colors、blob 名，再转交给底层 Create()。

	// 这个 json 结构体用来接收 DownloadModel() 解析出来的模型描述信息。
	// 这是一个 JSON 对象（类似 Python dict）：models.json
	nlohmann::json model;
	
	// 根据模型类型 + 模型名，查找并下载/定位内置模型配置。
	// 如果失败，说明模型名无效，或者模型资源不可用。
	if( !DownloadModel(SEGNET_MODEL_TYPE, network, model) )
		return NULL;
	
	// 先拼出模型所在目录，后面 prototxt/labels/colors 都会基于它拼接：networks/fcn-resnet18-voc/model.onnx
	std::string model_dir = "networks/" + model["dir"].get<std::string>() + "/";
	// 模型文件本体路径。
	std::string model_path = model_dir + model["model"].get<std::string>();
	// 从 json 中读取 prototxt/labels/colors 配置。
	std::string prototxt = JSON_STR(model["prototxt"]); // 网络结构（Caffe时代）
	std::string labels = JSON_STR(model["labels"]);
	std::string colors = JSON_STR(model["colors"]);
	// 读取输入输出 blob 名；如果 json 里没写，就使用默认名字。
	std::string input = JSON_STR_DEFAULT(model["input"], SEGNET_DEFAULT_INPUT);
	std::string output = JSON_STR_DEFAULT(model["output"], SEGNET_DEFAULT_OUTPUT);
		
	// 如果 prototxt 不为空，就补上所在目录，得到完整路径。 把相对路径 → 绝对路径
	if( prototxt.length() > 0 )
		prototxt = model_dir + prototxt;
	
	// labels 如果当前找不到，就假定它位于 model_dir 下。
	if( locateFile(labels).length() == 0 )
		labels = model_dir + labels;
	
	// colors 如果当前找不到，也假定它位于 model_dir 下。
	if( locateFile(colors).length() == 0 )
		colors = model_dir + colors;
	
	// 把已经解析好的真实路径和 blob 名，继续交给更底层的 Create()。 函数重载（overload）调用
	return Create(prototxt.c_str(), model_path.c_str(), labels.c_str(), 
			    colors.c_str(), input.c_str(), output.c_str(), 
			    maxBatchSize, precision, device, allowGPUFallback);
}

//--------------------------------------------------------------------------------------//
// Create ：把 C 风格命令行参数 → 转成 C++对象 → 统一交给核心 Create 处理
segNet* segNet::Create( int argc, char** argv )
{
	// 这是最常见的命令行入口。
	// 先把 argc/argv 包装成 commandLine，再交给另一个重载统一处理。
	// | 参数   | 作用   |
/* 		| ---- | ---- |
		| argc | 参数个数 |
		| argv | 参数数组 | */

/* 		./segnet --network=fcn-resnet18 --input=image.jpg
		argc = 3

		argv[0] = "./segnet"
		argv[1] = "--network=fcn-resnet18"
		argv[2] = "--input=image.jpg" */

	return Create(commandLine(argc, argv));
}

//--------------------------------------------------------------------------------------//
// Create：根据命令行参数，决定“用内置模型”还是“自定义模型”，再统一完成网络创建和运行配置
/* cmdLine
  ↓
读取 model / network
  ↓
判断是否内置模型
  ↓
├── 内置模型 → Create(modelName)
└── 自定义模型 → Create(完整路径参数)
  ↓
后处理（legend / profiler / alpha）
  ↓
返回 net */

segNet* segNet::Create( const commandLine& cmdLine )
{
	// net 最终指向创建好的 segNet 实例。
	// 如果中途任一步失败，就保持为 NULL。
	segNet* net = NULL;

	// 这里统一支持两种用法：
	//   1. --network=<内置模型名>
	//   2. --model=/path/to/custom.onnx
	// 这样 Python/C++ 示例都可以直接透传 argv，而不必自己重复写模型解析逻辑。

	// 先尝试读取 --model=xxx。
	const char* modelName = cmdLine.GetString("model");

	// 如果没有显式传 --model，就退回到 --network。
	// 再没有的话，默认使用内置的 fcn-resnet18-voc-320x320。
	if( !modelName )
		modelName = cmdLine.GetString("network", "fcn-resnet18-voc-320x320");

	// 如果这个名字不是内置 segmentation 模型，就按“自定义模型路径”处理。
	if( !FindModel(SEGNET_MODEL_TYPE, modelName) )
	{
		// 对于自定义模型，需要额外拿到 prototxt/labels/colors/input/output 等信息。
		const char* prototxt = cmdLine.GetString("prototxt"); // 网络结构
		const char* labels   = cmdLine.GetString("labels");  // 类别
		const char* colors   = cmdLine.GetString("colors");
		const char* input    = cmdLine.GetString("input_blob");
		const char* output   = cmdLine.GetString("output_blob");

		// 如果用户没传 blob 名，就用默认输入输出名字。
		if( !input ) 	input = SEGNET_DEFAULT_INPUT;
		if( !output )  output = SEGNET_DEFAULT_OUTPUT;

		// 批大小默认从命令行读取。
		int maxBatchSize = cmdLine.GetInt("batch_size");
		
		// 非法 batch_size 回退为默认值。很标准的：输入校验 + fallback
		if( maxBatchSize < 1 )
			maxBatchSize = DEFAULT_MAX_BATCH_SIZE;
		
		// 按“自定义模型路径”的方式创建网络。走底层 Create
		net = segNet::Create(prototxt, modelName, labels, colors, input, output, maxBatchSize);
	}
	else
	{
		// 否则说明传的是内置模型名，走内置模型路径。
		net = segNet::Create(modelName);
	}

	// 创建失败就直接返回 NULL。
	if( !net )
		return NULL;

	// 如果用户希望把类别颜色图例导出成一张图片，就在这里生成。
	const char* legend = cmdLine.GetString("legend");

	if( legend != NULL )
		net->saveClassLegend(legend);

	// 如果用户打开了 --profile，就启用 TensorRT 层级 profiler。
	if( cmdLine.GetFlag("profile") )
		net->EnableLayerProfiler();

	// 最后再应用命令行中的 alpha 设置。
	net->SetOverlayAlpha(cmdLine.GetFloat("alpha", SEGNET_DEFAULT_ALPHA));

	// 返回初始化完成的网络实例。
	return net;
}

//-----------------------**********************--------------------//
// Create：真正“落地到 TensorRT/显存/张量”的核心创建函数
// 完成从“模型文件 → TensorRT Engine → GPU内存 → 推理结构”的完整初始化
segNet* segNet::Create( const char* prototxt, const char* model, const char* labels_path, const char* colors_path, 
				    const char* input_blob, const char* output_blob, uint32_t maxBatchSize,
				    precisionType precision, deviceType device, bool allowGPUFallback )
{
	// 第一部分：输入校验 + 分流
	// 再次检查一次：如果传进来的 model 其实是内置模型名，直接回退到上面的内置模型 Create()，避免重复逻辑。避免重复代码（非常关键的工程习惯）
	if( FindModel(SEGNET_MODEL_TYPE, model) )
	{
		return Create(model, maxBatchSize, precision, device, allowGPUFallback);
	}
	// 非法输入检测：如果既不是内置模型名，又没有文件扩展名.onnx / .engine，那通常说明字符串无效。
	else if( fileExtension(model).length() == 0 )
	{
		LogError(LOG_TRT "couldn't find built-in segmentation model '%s'\n", model);
		return NULL;
	}
	
	// 第二部分：对象创建 分配 segNet 实例本身。在堆上创建对象
	segNet* net = new segNet();
	
	// new 失败就返回 NULL。new 失败会 throw（现代C++）这里是老风格防御写法
	if( !net )
		return NULL;

	// 第三部分：日志（工程必备） 这是工业代码的标配（debug神器）
	// 打印当前加载配置，方便用户确认加载的是哪个模型、哪些 blob、哪些标签文件。
	LogInfo("\n");
	LogInfo("segNet -- loading segmentation network model from:\n");
	LogInfo("       -- prototxt:   %s\n", prototxt);
	LogInfo("       -- model:      %s\n", model);
	LogInfo("       -- labels:     %s\n", labels_path);
	LogInfo("       -- colors:     %s\n", colors_path);
	LogInfo("       -- input_blob  '%s'\n", input_blob);
	LogInfo("       -- output_blob '%s'\n", output_blob);
	LogInfo("       -- batch_size  %u\n\n", maxBatchSize);
	
	//net->EnableProfiler();	
	//net->EnableDebug();
	//net->DisableFP16();		// debug;

	// 第四部分：核心 —— LoadNetwork

	// 真正进入 tensorNet 的通用加载流程。
	// 如果 model 是 engine/plan，会直接反序列化 TensorRT engine。
	// 如果 model 是 ONNX/Caffe，则会按 tensorNet 的规则构建或读取 engine cache。

	// segNet 当前只关心一个输出 blob，所以这里放进 vector 传给 LoadNetwork()。
	std::vector<std::string> output_blobs;
	output_blobs.push_back(output_blob);
	
	//-----------------------------------------------------------------
	// ④ 构造输出 blob segNet只有一个输出（分割结果）
	// 调用基类 tensorNet 的加载逻辑。
	// 成功后，mInputs/mOutputs 等底层 tensor 结构都会准备好。
	// LoadNetwork  情况1：.engine 文件→ 直接反序列化 TensorRT Engine
	// 情况2：.onnx / .caffemodel
		/* 	→ 解析模型
			→ 构建 TensorRT engine
			→ 优化（FP16 / INT8）
			→ 保存 cache 
		同时还会：
			分配 GPU tensor
			建立输入输出 binding
			创建 execution context	
		成功后：
		net->mInputs
		net->mOutputs
			全部准备好
			*/

	// ❗ 失败处理
	if( !net->LoadNetwork(prototxt, model, NULL, input_blob, output_blobs, maxBatchSize,
					  precision, device, allowGPUFallback) )
	{
		LogError(LOG_TRT "segNet -- failed to load.\n");
		return NULL;
	}

/* 		这里失败常见原因：

		ONNX不支持
		TensorRT版本不兼容
		GPU资源不足 */

	// 第五部分：解析输出 tensor（非常关键）	
	// 初始化 mClassMap。  
	// 注意这里不是按原图大小分配（注意这里不是原图大小分割网络输出通常是下采样后的特征图），而是按网络输出网格大小 [W,H] 分配。 输出 tensor: [1, 21, 320, 320]
	// 因为分割网络的输出分辨率通常比输入图像低。
	const int s_w = DIMS_W(net->mOutputs[0].dims);
	const int s_h = DIMS_H(net->mOutputs[0].dims);
	const int s_c = DIMS_C(net->mOutputs[0].dims);
		
	// 输出一下分割输出张量的尺寸，便于确认模型输出结构是否符合预期。
	LogVerbose(LOG_TRT "segNet outputs -- s_w %i  s_h %i  s_c %i\n", s_w, s_h, s_c);


	// 第六部分：内存分配（重点）
	// cudaAllocMapped() 分配的是 CPU/GPU 共享映射内存：
	//   CPU 侧 classify() 可以写
	//   GPU 侧 segNet.cu 的 kernel 可以直接读
	// GPU kernel 写分割结果 CPU读取结果
	// mClassMap 是什么？每个像素：一个 uint8_t = 类别ID
	if( !cudaAllocMapped((void**)&net->mClassMap, s_w * s_h * sizeof(uint8_t)) )
		return NULL;


	// 第七部分：加载配置	
	// ⑤ 调用核心函数
	// 加载类别标签文本。
	net->loadClassLabels(labels_path);
	// 加载类别颜色表。
	net->loadClassColors(colors_path);

	// 第八部分：默认参数
	// 给 overlay 应用默认透明度。
	net->SetOverlayAlpha(SEGNET_DEFAULT_ALPHA);
	
	// 返回已经完成基本初始化的 segNet。
	// 此时 net 已经：Engine加载完成 GPU内存准备好 输出结构解析 可直接推理
	return net;
}


// loadClassColors
bool segNet::loadClassColors( const char* filename )
{
	// 先拿当前网络的类别数量。
	const uint32_t numClasses = GetNumClasses();
	
	// mClassColors 也是共享映射内存，因为 CPU 侧会修改颜色/alpha，
	// 而 GPU 侧 overlay kernel 会直接读取它。
	if( !LoadClassColors(filename, &mClassColors, numClasses) )
		return false;
	
	// 为每个类别再额外分配一个标记位，
	// 用来记录这个类别的 alpha 是否是用户/文件显式设置的。
	mColorsAlphaSet = (bool*)malloc(numClasses * sizeof(bool));

	if( !mColorsAlphaSet )
	{
		printf(LOG_TRT "segNet -- failed to allocate class colors alpha flag array\n");
		return NULL;
	}

	//memset(mColorsAlphaSet, 0, numClasses * sizeof(bool));
	
	// 遍历所有类别。
	// 如果颜色表里这个类别的 alpha 不是 255，说明它已经显式设置过。
	for( uint32_t n=0; n < numClasses; n++ )
		mColorsAlphaSet[n] = (mClassColors[n].w != 255) ? true : false;
	
	// 颜色表和 alpha 标记都准备完成。
	return true;
}


// loadClassLabels
bool segNet::loadClassLabels( const char* filename )
{
	// 把类别名字列表加载到 mClassLabels 中。
	if( !LoadClassLabels(filename, mClassLabels, GetNumClasses()) )
		return false;

	// 如果传入了文件名，就保存解析后的实际路径。
	// 后面有需要时可以查询原始标签文件来源。
	if( filename != NULL )
		mClassPath = locateFile(filename);	
	
	// 标签加载成功。
	return true;
}


// saveClassLegend
bool segNet::saveClassLegend( const char* filename )
{
	// colorSize 表示每个类别颜色块的大小。
	const int2 colorSize = make_int2(50,25);
	// xyPadding 表示图例里文本和颜色块之间、以及条目之间的留白。
	const int2 xyPadding = make_int2(10,5);

	// 图例背景色使用白色，文字前景色使用黑色。
	const float4 bgColor = make_float4(255,255,255,255);
	const float4 fgColor = make_float4(0,0,0,255);

	// 没有输出文件名就无法保存。
	if( !filename )
		return false;

	// 获取类别总数。
	const uint32_t numClasses = GetNumClasses();

	// 没有类别就没有图例可画。
	if( numClasses == 0 )
		return false;

	// 创建绘制文字用的字体对象。
	cudaFont* font = cudaFont::Create(16);

	// 字体创建失败则无法继续生成图例。
	if( !font )
		return false;

	// 先扫描一遍所有类别文本，找出需要的最大文字尺寸。
	// 后面会据此确定整张图例图像的大小。
	int2 maxTextExtents = make_int2(0,0);

	for( uint32_t n=0; n < numClasses; n++ )
	{
		// 生成形如 " 0 road" 的图例文字。
		char str[256];
		sprintf(str, "%2d %s", n, GetClassDesc(n));

		// 计算这段文字的像素范围。
		const int4 textExtents = font->TextExtents(str);

		// 更新最大文本宽度。
		if( textExtents.z > maxTextExtents.x )
			maxTextExtents.x = textExtents.z;

		// 更新最大文本高度。
		if( textExtents.w > maxTextExtents.y )
			maxTextExtents.y = textExtents.w;
	}

	//if( colorSize.y > maxTextExtents.y )
	//	maxTextExtents.y = colorSize.y;

	// 根据“最大文字尺寸 + 颜色块大小 + padding”来计算最终图例图像尺寸。
	const int imgWidth = maxTextExtents.x + colorSize.x + xyPadding.x * 3;
	const int imgHeight = (colorSize.y + xyPadding.y) * numClasses + xyPadding.y * 2;
	
	// img 是最终要保存的 RGBA 图像，使用共享映射内存分配。
	float4* img = NULL;

	if( !cudaAllocMapped((void**)&img, imgWidth * imgHeight * 4 * sizeof(float)) )
		return false;

	// 定义一个简单的填充矩形宏，后面用来刷背景和颜色块。
	#define FILL_RECT(color, x1, y1, x2, y2) \
		for( int y=y1; y < y2; y++ )		 \
			for( int x=x1; x < x2; x++ )	 \
				img[y*imgWidth+x] = color;

	// 先把整张图都填成背景色。
	FILL_RECT(bgColor, 0, 0, imgWidth, imgHeight);

	// yPosition 表示当前条目的绘制 y 坐标。
	int yPosition = xyPadding.y * 2;

	// 逐个类别绘制图例条目。
	for( uint32_t n=0; n < numClasses; n++ )
	{
		// 生成当前条目的文字内容。
		char str[256];
		sprintf(str, "%2d %s", n, GetClassDesc(n));

		// 把类别文字绘制到图像上。
		font->OverlayText(img, imgWidth, imgHeight, str, xyPadding.x, yPosition);
		// 等待文字绘制对应的 CUDA 操作完成。
		CUDA(cudaDeviceSynchronize());

		// 读取当前类别颜色。
		float4 color = GetClassColor(n);
		// 图例颜色块总是用不透明方式显示，便于看清。
		color.w = 255;

		// 计算颜色块的位置。
		const int colorX = maxTextExtents.x + xyPadding.x * 2;
		const int colorY = yPosition - ((colorSize.y - maxTextExtents.y) / 2);

		// 填充颜色块。
		FILL_RECT(color, colorX, colorY, colorX + colorSize.x, colorY + colorSize.y);

		// y 坐标下移，准备绘制下一个类别。
		yPosition += colorSize.y + xyPadding.y;
	}

	// 把生成好的图例图保存到磁盘。
	const bool result = saveImageRGBA(filename, img, imgWidth, imgHeight);

	// 释放图例临时图像内存。
	CUDA(cudaFreeHost(img));
	// 销毁字体对象。
	delete font;
	// 返回保存结果。
	return result;
}


// SetClassColor
void segNet::SetClassColor( uint32_t classIndex, const float4& color )
{
	// 越界类别或颜色表还没初始化时，不做任何事情。
	if( classIndex >= GetNumClasses() || !mClassColors )
		return;
	
	// 更新该类别的 RGBA 颜色。
	mClassColors[classIndex] = color;
	// 如果 alpha 不是 255，说明这个 alpha 是显式设置的。
	mColorsAlphaSet[classIndex] = (color.w == 255) ? false : true;
}


// SetClassColor
void segNet::SetClassColor( uint32_t classIndex, float r, float g, float b, float a )
{
	// 这是一个便捷重载，把 4 个 float 包装成 float4 后继续调用主实现。
	SetClassColor(classIndex, make_float4(r,g,b,a));
}


// GetOverlayAlpha
float segNet::GetOverlayAlpha() const
{
	// 遍历所有类别，找到第一个“不是显式 alpha”的类别。
	// 这样可以推断当前全局 overlay alpha 大概是多少。
	const uint32_t numClasses = GetNumClasses();

	for( uint32_t n=0; n < numClasses; n++ )
	{
		if( !mColorsAlphaSet[n] )
			return mClassColors[n].w;
	}
	
	// 如果所有类别都是显式 alpha，就退回默认值。
	return SEGNET_DEFAULT_ALPHA;
}


// SetOverlayAlpha
void segNet::SetOverlayAlpha( float alpha, bool explicit_exempt )
{
	// 遍历所有类别。
	const uint32_t numClasses = GetNumClasses();

	for( uint32_t n=0; n < numClasses; n++ )
	{
		// 如果 explicit_exempt=false，就强制覆盖所有类别 alpha。
		// 如果 explicit_exempt=true，就跳过那些原本显式设置过 alpha 的类别。
		if( !explicit_exempt || !mColorsAlphaSet[n] /*mClassColors[n*4+3] == 255*/ )
			mClassColors[n].w = alpha;
	}
}


// FindClassID
int segNet::FindClassID( const char* label_name )
{
	// 没传名字就说明不查找任何类别。
	if( !label_name )
		return -1;

	// 获取当前标签数量。
	const uint32_t numLabels = mClassLabels.size();

	// 顺序扫描标签列表，做不区分大小写的比较。
	for( uint32_t n=0; n < numLabels; n++ )
	{
		if( strcasecmp(label_name, mClassLabels[n].c_str()) == 0 )
			return n;
	}

	// 没找到就返回 -1。
	return -1;
}


// Process
bool segNet::Process( float* rgba, uint32_t width, uint32_t height, const char* ignore_class )
{
	// 旧接口兼容层：
	// 把 float* RGBA 输入转交给新的通用 Process() 实现。
	return Process(rgba, width, height, IMAGE_RGBA32F, ignore_class);
}


// Process
bool segNet::Process( void* image, uint32_t width, uint32_t height, imageFormat format, const char* ignore_class )
{
	// 输入图像通常来自上游 CUDA 图像源，本身已经在 GPU 上。
	// 这里先做格式校验，避免后面的 CUDA 预处理收到不支持的数据。

	// 第 1 步：检查输入指针和尺寸是否有效。
	if( !image || width == 0 || height == 0 )
	{
		LogError(LOG_TRT "segNet::Process( 0x%p, %u, %u ) -> invalid parameters\n", image, width, height);
		return false;
	}

	// 第 2 步：检查像素格式是否属于支持的 RGB/RGBA 系列。
	if( !imageFormatIsRGB(format) )
	{
		LogError(LOG_TRT "segNet -- unsupported image format (%s)\n", imageFormatToStr(format));
		LogError(LOG_TRT "          supported formats are:\n");
		LogError(LOG_TRT "              * rgb8\n");		
		LogError(LOG_TRT "              * rgba8\n");		
		LogError(LOG_TRT "              * rgb32f\n");		
		LogError(LOG_TRT "              * rgba32f\n");

		return cudaErrorInvalidValue;
	}

	PROFILER_BEGIN(PROFILER_PREPROCESS);

	// 第 3 步：预处理。
	// 这一步把原始图像 resize / reorder / normalize 成网络真正要吃的 tensor。
	if( IsModelType(MODEL_ONNX) )
	{
		// ONNX 路径：
		// 常见 ONNX 语义分割模型，尤其是 PyTorch 导出的模型，
		// 通常期望 RGB + normalize(mean/std) 这套预处理。
		// downsample, convert to band-sequential RGB, and apply pixel normalization, mean pixel subtraction and standard deviation
		if( CUDA_FAILED(cudaTensorNormMeanRGB(image, format, width, height,
									   mInputs[0].CUDA, GetInputWidth(), GetInputHeight(),
									   make_float2(0.0f, 1.0f), 
									   make_float3(0.485f, 0.456f, 0.406f),
									   make_float3(0.229f, 0.224f, 0.225f), 
									   GetStream())) )
		{
			LogError(LOG_TRT "segNet::Process() -- cudaTensorNormMeanRGB() failed\n");
			return false;
		}
	}
	else
	{
		// 旧模型/其他格式路径：
		// 一些历史模型使用 BGR 排列和不同的均值处理，所以这里保留兼容分支。
		// downsample and convert to band-sequential BGR
		if( CUDA_FAILED(cudaTensorMeanBGR(image, format, width, height, 
								    mInputs[0].CUDA, GetInputWidth(), GetInputHeight(),
								    make_float3(0,0,0), GetStream())) )
		{
			LogError(LOG_TRT "segNet::Process() -- cudaTensorMeanBGR() failed\n");
			return false;
		}
	}

	PROFILER_END(PROFILER_PREPROCESS);
	PROFILER_BEGIN(PROFILER_NETWORK);
	
	// 第 4 步：运行 TensorRT 推理。
	// ProcessNetwork() 是 tensorNet 基类提供的通用推理入口。
	// 它会把 mInputs[0].CUDA 喂给 TensorRT，并把结果写到 mOutputs[]。
	if( !ProcessNetwork() )
		return false;

	PROFILER_END(PROFILER_NETWORK);
	PROFILER_BEGIN(PROFILER_POSTPROCESS);

	// 第 5 步：对网络输出做 argmax，生成类别图 mClassMap。
	// 把原始 [C,H,W] 分数张量压缩成更紧凑的类别图 mClassMap。
	// 后面的 Overlay()/Mask() 都依赖这一步的结果。
	if( !classify(ignore_class) )
		return false;

	PROFILER_END(PROFILER_POSTPROCESS);

	// 第 6 步：缓存这次输入图像信息。
	// 这样后面的 Overlay() 才能在生成彩色结果时重新拿到原图做叠加。
	// 缓存最近一帧原图的信息。
	// Overlay() 需要它，因为 overlay 不只是画类别颜色，还要叠回原图。
	mLastInputImg    = image;
	mLastInputWidth  = width;
	mLastInputHeight = height;
	mLastInputFormat = format;

	// 到这里，分割结果已经存在 mClassMap 中，等待后续 Mask()/Overlay() 消费。
	return true;
}


// argmax classification
bool segNet::classify( const char* ignore_class )
{
	// mOutputs[0].CPU 是网络输出在主机侧可读的内存视图。
	// 这里在 CPU 上做 argmax，把完整分数张量压缩成“每个位置一个 classID”，
	// 后续可视化 kernel 读取它会轻量很多。

	// 第 1 步：拿到原始分数张量指针。
	float* scores = mOutputs[0].CPU;

	// 第 2 步：取出输出张量的宽、高、类别数。
	const int s_w = DIMS_W(mOutputs[0].dims);
	const int s_h = DIMS_H(mOutputs[0].dims);
	const int s_c = DIMS_C(mOutputs[0].dims);
		
	//const float s_x = float(width) / float(s_w);		// TODO bug: this should use mWidth/mHeight dimensions, in case user dimensions are different
	//const float s_y = float(height) / float(s_h);
	const float s_x = float(s_w) / float(GetInputWidth());
	const float s_y = float(s_h) / float(GetInputHeight());


	// 第 3 步：如果需要忽略某个类别，就先把它的 ID 找出来。
	// 有些数据集会有 void/background 类，不希望它参与最终显示竞争，
	// 这里允许调用方按名字把它排除掉。
	const int ignoreID = FindClassID(ignore_class);
	
	//printf(LOG_TRT "segNet::Process -- s_w %i  s_h %i  s_c %i  s_x %f  s_y %f\n", s_w, s_h, s_c, s_x, s_y);
	//printf(LOG_TRT "segNet::Process -- ignoring class '%s' id=%i\n", ignore_class, ignoreID);


	// 对输出网格上的每一个位置，从所有类别里挑出分数最高的那个。
	// 这一步本质上就是 argmax(C)。

	// 第 4 步：准备写入 mClassMap。
	uint8_t* classMap = mClassMap;

	// 第 5 步：逐行遍历输出网格。
	for( uint32_t y=0; y < s_h; y++ )
	{
		// 第 6 步：逐列遍历输出网格。
		for( uint32_t x=0; x < s_w; x++ )
		{
			// p_max 记录当前位置目前遇到的最大得分。
			float p_max = -100000.0f;
			// c_max 记录最大得分对应的类别。
			int   c_max = -1;

			// 第 7 步：遍历所有类别通道，找到最大分数对应的类别。
			for( int c=0; c < s_c; c++ )
			{
				// skip ignoreID
				if( c == ignoreID )
					continue;

				// 取出当前位置在第 c 个类别上的分数。
				const float p = scores[c * s_w * s_h + y * s_w + x];

				// 如果它比当前最大值还高，就更新最大值和类别。
				if( c_max < 0 || p > p_max )
				{
					p_max = p;
					c_max = c;
				}
			}

			// 第 8 步：把 argmax 结果写到类别图里。
			classMap[y * s_w + x] = c_max;
			//printf("(%u, %u) -> class %i\n", x, y, (uint32_t)c_max);
		}
	}

	// argmax 完成，mClassMap 现在已经可以被 Mask()/Overlay() 使用。
	return true;
}


// Mask (binary)
bool segNet::Mask( uint8_t* output, uint32_t out_width, uint32_t out_height )
{
	// 这是索引 mask 版本。
	// 它不输出彩色图，而是直接输出 classID，适合调试、保存标签图、
	// 或者交给后续算法继续处理。

	// 第 1 步：检查输出缓冲区是否合法。
	if( !output || out_width == 0 || out_height == 0 )
	{
		LogError(LOG_TRT "segNet::Mask( 0x%p, %u, %u ) -> invalid parameters\n", output, out_width, out_height); 
		return false;
	}	

	PROFILER_BEGIN(PROFILER_VISUALIZE);

	// 读取 argmax 后的类别图 mClassMap。
	uint8_t* classMap = mClassMap;

	// 第 2 步：拿到类别图自身的分辨率。
	const int s_w = DIMS_W(mOutputs[0].dims);
	const int s_h = DIMS_H(mOutputs[0].dims);
		
	// 第 3 步：如果输出尺寸和类别图一样，就直接 memcpy。
	if( out_width == s_w && out_height == s_h )
	{
		memcpy(output, classMap, s_w * s_h * sizeof(uint8_t));
	}
	else
	{
		// 第 4 步：如果输出尺寸不同，就用最近邻方式把类别图重采样到目标尺寸。
		const float s_x = float(s_w) / float(out_width);
		const float s_y = float(s_h) / float(out_height);

		// overlay pixels onto original
		for( uint32_t y=0; y < out_height; y++ )
		{
			for( uint32_t x=0; x < out_width; x++ )
			{
				// 找到输出像素对应的类别图坐标。
				const int cx = float(x) * s_x;
				const int cy = float(y) * s_y;

				// get the class ID of this cell
				const uint8_t classIdx = classMap[cy * s_w + cx];

				// 把 classID 写到输出 mask。
				output[y * out_width + x] = classIdx;
			}
		}
	}
	
	PROFILER_END(PROFILER_VISUALIZE);
	// 索引 mask 生成完成。
	return true;
}


// Mask (colorized)
bool segNet::Mask( float* output, uint32_t width, uint32_t height, FilterMode filter )
{
	// 旧接口兼容层，把 float* RGBA 输出转交给通用版本。
	return Mask(output, width, height, IMAGE_RGBA32F, filter);
}


// Mask (colorized)
bool segNet::Mask( void* output, uint32_t width, uint32_t height, imageFormat format, FilterMode filter )
{
	// 第 1 步：检查输出缓冲区是否合法。
	if( !output || width == 0 || height == 0 )
	{
		LogError(LOG_TRT "segNet::Mask( 0x%p, %u, %u ) -> invalid parameters\n", output, width, height); 
		return false;
	}	

	// Mask() 和 Overlay() 共用同一套底层绘制逻辑。
	// 这里传 mask_only=true，表示“只输出分割颜色，不叠加原图”。

	// 第 2 步：根据滤波模式选择 point / linear 的可视化路径。
	if( filter == FILTER_POINT )
		return overlayPoint(NULL, 0, 0, IMAGE_UNKNOWN, output, width, height, format, true);
	else if( filter == FILTER_LINEAR )
		return overlayLinear(NULL, 0, 0, IMAGE_UNKNOWN, output, width, height, format, true);

	// 传入了未知滤波模式。
	return false;
}


// Overlay
bool segNet::Overlay( float* output, uint32_t width, uint32_t height, FilterMode filter )
{
	// 旧接口兼容层，把 float* RGBA 输出转交给通用版本。
	return Overlay(output, width, height, IMAGE_RGBA32F, filter);
}


// Overlay
bool segNet::Overlay( void* output, uint32_t width, uint32_t height, imageFormat format, segNet::FilterMode filter )
{
	// 第 1 步：检查输出缓冲区是否合法。
	if( !output || width == 0 || height == 0 )
	{
		LogError(LOG_TRT "segNet::Overlay( 0x%p, %u, %u ) -> invalid parameters\n", output, width, height); 
		return false;
	}	
	
	// 第 2 步：确保之前已经调用过 Process()。
	// 因为 overlay 必须要有原图 + mClassMap 两样东西。
	if( !mLastInputImg )
	{
		LogError(LOG_TRT "segNet -- Process() must be called before Overlay()\n");
		return false;
	}

	// Overlay() 依赖最近一次 Process() 生成的结果和缓存下来的原图。
	// 然后再根据 point/linear 选择具体可视化方式。

	// 第 3 步：根据滤波模式，选择最近邻或双线性 overlay。
	if( filter == FILTER_POINT )
		return overlayPoint(mLastInputImg, mLastInputWidth, mLastInputHeight, mLastInputFormat, output, width, height, format, false);
	else if( filter == FILTER_LINEAR )
		return overlayLinear(mLastInputImg, mLastInputWidth, mLastInputHeight, mLastInputFormat, output, width, height, format, false);

	// 未知滤波模式。
	return false;
}


#define OVERLAY_CUDA 

// declaration from segNet.cu
cudaError_t cudaSegOverlay( void* input, uint32_t in_width, uint32_t in_height,
				        void* output, uint32_t out_width, uint32_t out_height, imageFormat format,
					   float4* class_colors, uint8_t* scores, const int2& scores_dim,
					   bool filter_linear, bool mask_only, cudaStream_t stream );


// overlayLinear
bool segNet::overlayPoint( void* input, uint32_t in_width, uint32_t in_height, imageFormat in_format, void* output, uint32_t out_width, uint32_t out_height, imageFormat out_format, bool mask_only )
{
	// 如果传了 input，说明这是 overlay 模式，此时输入输出格式必须一致。
	// 因为最终是把类别颜色叠加回原图像素格式中。
	if( input != NULL && in_format != out_format )
	{
		LogError(LOG_TRT "segNet -- input image format (%s) and overlay/mask image format (%s) don't match\n", imageFormatToStr(in_format), imageFormatToStr(out_format));
		return false;
	}

	PROFILER_BEGIN(PROFILER_VISUALIZE);

#ifdef OVERLAY_CUDA
	// 主路径：把像素级绘制工作交给 segNet.cu 里的 CUDA kernel。
	// 这样 overlay/mask 的生成就能一直留在 GPU 上，避免整张图拷回 CPU 再处理。

	// 这里传 false 给 filter_linear，表示最近邻模式。
	// 如果 mask_only=true，就只输出 mask；
	// 如果 mask_only=false，就执行 overlay。
	if( CUDA_FAILED(cudaSegOverlay(input, in_width, in_height, output, out_width, out_height, out_format,
							 mClassColors, mClassMap, make_int2(DIMS_W(mOutputs[0].dims), DIMS_H(mOutputs[0].dims)),
							 false, mask_only, GetStream())) )
	{
		LogError(LOG_TRT "segNet -- failed to process %ux%u overlay/mask with CUDA\n", out_width, out_height);
		return false;
	}
#else
	// 备用 CPU 路径，只在没有启用 CUDA overlay 时使用。

	// 第 1 步：拿到类别图。
	uint8_t* classMap = mClassMap;

	// 第 2 步：拿到类别图尺寸。
	const int s_w = DIMS_W(mOutputs[0].dims);
	const int s_h = DIMS_H(mOutputs[0].dims);

	// 第 3 步：计算从输出图到类别图的缩放比例。
	const float s_x = float(s_w) / float(out_width);
	const float s_y = float(s_h) / float(out_height);


	// 第 4 步：逐像素生成输出图像。
	for( uint32_t y=0; y < out_height; y++ )
	{
		for( uint32_t x=0; x < out_width; x++ )
		{
			// 找到输出像素对应的类别图坐标。
			const int cx = float(x) * s_x;
			const int cy = float(y) * s_y;

			// get the class ID of this cell
			const uint8_t classIdx = classMap[cy * s_w + cx];

			// 读取这个类别对应的颜色。
			float* c_color = GetClassColor(classIdx);

			// 计算输出图像中当前像素的位置。
			float* px_out = output + (((y * out_width * 4) + x * 4));

			if( mask_only )
			{
				// mask 模式下，只把类别颜色写出去。
				px_out[0] = c_color[0];
				px_out[1] = c_color[1];
				px_out[2] = c_color[2];
				px_out[3] = 255.0f;
			}
			else
			{
				// overlay 模式下，需要先找到原图对应像素，再做 alpha blending。
				const uint32_t x_in = float(x) / float(out_width) * float(in_width);
				const uint32_t y_in = float(y) / float(out_height) * float(in_height);

				float* px_in = input + (((y_in * in_width * 4) + x_in * 4));
				
				// alph 是类别颜色的透明度，inva 是剩余原图权重。
				const float alph = c_color[3] / 255.0f;
				const float inva = 1.0f - alph;

				// 按 alpha 混合颜色。
				px_out[0] = alph * c_color[0] + inva * px_in[0];
				px_out[1] = alph * c_color[1] + inva * px_in[1];
				px_out[2] = alph * c_color[2] + inva * px_in[2];
				px_out[3] = 255.0f;
			}
		}
	}
#endif
	PROFILER_END(PROFILER_VISUALIZE);
	// 最近邻可视化生成完成。
	return true;
}


// overlayLinear
bool segNet::overlayLinear( void* input, uint32_t in_width, uint32_t in_height, imageFormat in_format, void* output, uint32_t out_width, uint32_t out_height, imageFormat out_format, bool mask_only )
{
	// 和 overlayPoint() 一样，overlay 模式下输入输出格式必须一致。
	if( input != NULL && in_format != out_format )
	{
		LogError(LOG_TRT "segNet -- input image format (%s) and overlay/mask image format (%s) don't match\n", imageFormatToStr(in_format), imageFormatToStr(out_format));
		return false;
	}

	PROFILER_BEGIN(PROFILER_VISUALIZE);

#ifdef OVERLAY_CUDA
	// 和 overlayPoint() 一样默认走 GPU。
	// 区别只是这里 filter_linear=true，kernel 会用双线性方式让边界更平滑。

	// 这里传 true 给 filter_linear，表示双线性插值模式。
	if( CUDA_FAILED(cudaSegOverlay(input, in_width, in_height, output, out_width, out_height, out_format,
							 mClassColors, mClassMap, make_int2(DIMS_W(mOutputs[0].dims), DIMS_H(mOutputs[0].dims)),
							 true, mask_only, GetStream())) )
	{
		LogError(LOG_TRT "segNet -- failed to process %ux%u overlay/mask with CUDA\n", out_width, out_height);
		return false;
	}
#else
	// 第 1 步：拿到类别图。
	uint8_t* classMap = mClassMap;

	// 第 2 步：拿到类别图尺寸。
	const int s_w = DIMS_W(mOutputs[0].dims);
	const int s_h = DIMS_H(mOutputs[0].dims);

	// 第 3 步：计算从输出图到类别图的缩放比例。
	const float s_x = float(s_w) / float(out_width);
	const float s_y = float(s_h) / float(out_height);


	// 第 4 步：逐像素生成输出图像。
	for( uint32_t y=0; y < out_height; y++ )
	{
		for( uint32_t x=0; x < out_width; x++ )
		{
			// 先把输出像素坐标映射到类别图坐标系。
			const float cx = float(x) * s_x;	
			const float cy = float(y) * s_y;

			// 找到左上角整数网格点。
			const int x1 = int(cx);
			const int y1 = int(cy);
			
			// 找到右下角相邻网格点。
			const int x2 = x1 + 1;
			const int y2 = y1 + 1;

			// CHK_BOUNDS 用来做边界保护，避免采样时越界。
			#define CHK_BOUNDS(x, y)		( (y < 0 ? 0 : (y >= (s_h - 1) ? (s_h - 1) : y)) * s_w + (x < 0 ? 0 : (x >= (s_w - 1) ? (s_w - 1) : x)) )

			/*const uint8_t classIdx[] = { classMap[y1 * s_w + x1],
								    classMap[y1 * s_w + x2],
								    classMap[y2 * s_w + x2],
								    classMap[y2 * s_w + x1] };*/

			// 读取双线性插值需要的 4 个邻域类别索引。
			const uint8_t classIdx[] = { classMap[CHK_BOUNDS(x1, y1)],
								    classMap[CHK_BOUNDS(x2, y1)],
								    classMap[CHK_BOUNDS(x2, y2)],
								    classMap[CHK_BOUNDS(x1, y2)] };


			// 再把 4 个类别索引映射成 4 个颜色。
			float* cc[] = { GetClassColor(classIdx[0]),
						 GetClassColor(classIdx[1]),
						 GetClassColor(classIdx[2]),
						 GetClassColor(classIdx[3]) };

			
			// 计算双线性插值权重。
			const float x1d = cx - float(x1);
			const float y1d = cy - float(y1);
		
			const float x2d = 1.0f - x1d;
			const float y2d = 1.0f - y1d;

			const float x1f = 1.0f - x1d;
			const float y1f = 1.0f - y1d;

			const float x2f = 1.0f - x1f;
			const float y2f = 1.0f - y1f;

			/*int c_index = 0;

			if( y2d > y1d )
			{
				if( x2d > y2d )			c_index = 2;
				else 					c_index = 3;
			}
			else
			{
				if( x2d > y2d )			c_index = 1;
				else						c_index = 0;
			}*/
			
			//float* c_color = GetClassColor(classIdx[c_index]);
			//printf("x %u y %u cx %f cy %f  x1d %f y1d %f  x2d %f y2d %f  c %i\n", x, y, cx, cy, x1d, y1d, x2d, y2d, c_index);

			// 对 RGBA 4 个通道分别做双线性颜色插值。
			float c_color[] = { cc[0][0] * x1f * y1f + cc[1][0] * x2f * y1f + cc[2][0] * x2f * y2f + cc[3][0] * x1f * y2f,
						     cc[0][1] * x1f * y1f + cc[1][1] * x2f * y1f + cc[2][1] * x2f * y2f + cc[3][1] * x1f * y2f,
						     cc[0][2] * x1f * y1f + cc[1][2] * x2f * y1f + cc[2][2] * x2f * y2f + cc[3][2] * x1f * y2f,
						     cc[0][3] * x1f * y1f + cc[1][3] * x2f * y1f + cc[2][3] * x2f * y2f + cc[3][3] * x1f * y2f };

			// 计算输出图像中当前像素的位置。
			float* px_out = output + (((y * out_width * 4) + x * 4));

			if( mask_only )
			{
				// mask 模式下，只输出插值后的分割颜色。
				px_out[0] = c_color[0];
				px_out[1] = c_color[1];
				px_out[2] = c_color[2];
				px_out[3] = 255.0f;
			}
			else
			{
				// overlay 模式下，再把插值后的类别颜色和原图做 alpha blending。
				const int x_in = float(x) / float(out_width) * float(in_width);
				const int y_in = float(y) / float(out_height) * float(in_height);

				float* px_in = input + (((y_in * in_width * 4) + x_in * 4));
				
				// alph 是颜色透明度，inva 是原图保留比例。
				const float alph = c_color[3] / 255.0f;
				const float inva = 1.0f - alph;

				// 按 alpha 融合。
				px_out[0] = alph * c_color[0] + inva * px_in[0];
				px_out[1] = alph * c_color[1] + inva * px_in[1];
				px_out[2] = alph * c_color[2] + inva * px_in[2];
				px_out[3] = 255.0f;
			}
		}
	}
#endif
	PROFILER_END(PROFILER_VISUALIZE);
	// 双线性可视化生成完成。
	return true;
}
	
	
