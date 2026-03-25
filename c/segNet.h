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
 
#ifndef __SEGMENTATION_NET_H__
#define __SEGMENTATION_NET_H__


#include "tensorNet.h"

/*
 * 这 3 个文件的分工是：
 *   segNet.h   = 对外接口和成员定义
 *   segNet.cpp = 主机侧流程控制（加载模型、预处理、推理、argmax、分发可视化）
 *   segNet.cu  = 在 GPU 上生成 overlay/mask 的 CUDA kernel
 *
 * 这样拆开的原因：
 *   1. .h 只暴露接口，外部代码不需要关心实现细节
 *   2. .cpp 放普通 C++ / TensorRT 逻辑，便于维护
 *   3. .cu 放 __global__ 代码，交给 NVCC 单独编译
 */


/**
 * Name of default input blob for segmentation model.
 * @ingroup segNet
 */
#define SEGNET_DEFAULT_INPUT   "input_0"

/**
 * Name of default output blob for segmentation model.
 * @ingroup segNet
 */
#define SEGNET_DEFAULT_OUTPUT  "output_0"

/**
 * Default alpha blending value used during overlay
 * @ingroup segNet
 */
#define SEGNET_DEFAULT_ALPHA 150

/**
 * The model type for segNet in data/networks/models.json
 * @ingroup segNet
 */
#define SEGNET_MODEL_TYPE "segmentation"

/**
 * Standard command-line options able to be passed to segNet::Create()
 * @ingroup segNet
 */
#define SEGNET_USAGE_STRING  "segNet arguments: \n" 							\
		  "  --network=NETWORK    pre-trained model to load, one of the following:\n" 	\
		  "                           * fcn-resnet18-cityscapes-512x256\n"			\
		  "                           * fcn-resnet18-cityscapes-1024x512\n"			\
		  "                           * fcn-resnet18-cityscapes-2048x1024\n"			\
		  "                           * fcn-resnet18-deepscene-576x320\n"			\
		  "                           * fcn-resnet18-deepscene-864x480\n"			\
		  "                           * fcn-resnet18-mhp-512x320\n"					\
		  "                           * fcn-resnet18-mhp-640x360\n"					\
		  "                           * fcn-resnet18-voc-320x320 (default)\n"			\
		  "                           * fcn-resnet18-voc-512x320\n"					\
		  "                           * fcn-resnet18-sun-512x400\n"					\
		  "                           * fcn-resnet18-sun-640x512\n"                  	\
		  "  --model=MODEL        path to custom model to load (caffemodel, uff, or onnx)\n" 			\
		  "  --prototxt=PROTOTXT  path to custom prototxt to load (for .caffemodel only)\n" 				\
		  "  --labels=LABELS      path to text file containing the labels for each class\n" 				\
		  "  --colors=COLORS      path to text file containing the colors for each class\n" 				\
		  "  --input-blob=INPUT   name of the input layer (default: '" SEGNET_DEFAULT_INPUT "')\n" 		\
		  "  --output-blob=OUTPUT name of the output layer (default: '" SEGNET_DEFAULT_OUTPUT "')\n" 		\
            "  --alpha=ALPHA        overlay alpha blending value, range 0-255 (default: 150)\n"			\
		  "  --visualize=VISUAL   visualization flags (e.g. --visualize=overlay,mask)\n"				\
		  "                       valid combinations are:  'overlay', 'mask'\n"						\
		  "  --profile            enable layer profiling in TensorRT\n\n"


/**
 * Image segmentation with FCN-Alexnet or custom models, using TensorRT.
 * @ingroup segNet
 */
class segNet : public tensorNet
{
public:
	/**
 	 * Enumeration of mask/overlay filtering modes.
	 */
	enum FilterMode
	{
		FILTER_POINT = 0,	/**< Nearest point sampling */
		FILTER_LINEAR		/**< Bilinear filtering */
	};

	/**
	 * Visualization flags.
	 */
	enum VisualizationFlags
	{
		VISUALIZE_OVERLAY = (1 << 0),  /**< Overlay the segmentation class colors with alpha blending */
		VISUALIZE_MASK    = (1 << 1),  /**< View just the colorized segmentation class mask */
	};

	/**
	 * Parse a string of one of more VisualizationMode values.
	 * Valid strings are "overlay" "mask" "overlay|mask" "overlay,mask" ect.
	 */
	static uint32_t VisualizationFlagsFromStr( const char* str, uint32_t default_value=VISUALIZE_OVERLAY );

	/**
	 * Parse a string from one of the FilterMode values.
	 * Valid strings are "point", and "linear"
	 * @returns one of the segNet::FilterMode enums, or default segNet::FILTER_LINEAR on an error.
	 */
	static FilterMode FilterModeFromStr( const char* str, FilterMode default_value=FILTER_LINEAR );

	/**
	 * Load a pre-trained model.
	 * @see SEGNET_USAGE_STRING for the models available.
	 */
	static segNet* Create( const char* network="fcn-resnet18-voc", uint32_t maxBatchSize=DEFAULT_MAX_BATCH_SIZE,
					   precisionType precision=TYPE_FASTEST, deviceType device=DEVICE_GPU, bool allowGPUFallback=true );
	
	/**
	 * Load a new network instance
	 * @param prototxt_path File path to the deployable network prototxt
	 * @param model_path File path to the caffemodel
	 * @param class_labels File path to list of class name labels
	 * @param class_colors File path to list of class colors
	 * @param input Name of the input layer blob. @see SEGNET_DEFAULT_INPUT
	 * @param output Name of the output layer blob. @see SEGNET_DEFAULT_OUTPUT
	 * @param maxBatchSize The maximum batch size that the network will support and be optimized for.
	 */
	static segNet* Create( const char* prototxt_path, const char* model_path, 
					   const char* class_labels, const char* class_colors=NULL,
					   const char* input = SEGNET_DEFAULT_INPUT, 
					   const char* output = SEGNET_DEFAULT_OUTPUT,
					   uint32_t maxBatchSize=DEFAULT_MAX_BATCH_SIZE, 
					   precisionType precision=TYPE_FASTEST, 
					   deviceType device=DEVICE_GPU, bool allowGPUFallback=true );
	

	/**
	 * Load a new network instance by parsing the command line.
	 */
	static segNet* Create( int argc, char** argv );
	
	/**
	 * Load a new network instance by parsing the command line.
	 */
	static segNet* Create( const commandLine& cmdLine );
	
	/**
	 * Usage string for command line arguments to Create()
	 */
	static inline const char* Usage() 		{ return SEGNET_USAGE_STRING; }

	/**
	 * Destroy
	 */
	virtual ~segNet();
	
	/**
 	 * 执行分割推理这一侧的主流程。
	 * 这一步会：
	 *   1. 把输入图像转换成网络需要的输入 tensor
	 *   2. 调用 TensorRT 跑推理
	 *   3. 对输出做 argmax，生成 mClassMap
	 *
	 * Process() 不会直接生成可视化图像。
	 * 它先把中间分割结果存下来，后面的 Overlay()/Mask() 再继续使用。
	 *
	 * The results can then be visualized using the Overlay() and Mask() functions.
	 * @param input the input image in CUDA device memory, with pixel values 0-255.
	 * @param width width of the input image in pixels.
	 * @param height height of the input image in pixels.
	 * @param ignore_class label name of class to ignore in the classification (or NULL to process all).
	 */
	template<typename T> bool Process( T* input, uint32_t width, uint32_t height, const char* ignore_class="void" )		{ return Process((void*)input, width, height, imageFormatFromType<T>(), ignore_class); }
	
	/**
 	 * Perform the initial inferencing processing portion of the segmentation.
	 * The results can then be visualized using the Overlay() and Mask() functions.      
	 * @param input the input image in CUDA device memory, with pixel values 0-255.
	 * @param width width of the input image in pixels.
	 * @param height height of the input image in pixels.
	 * @param ignore_class label name of class to ignore in the classification (or NULL to process all).
	 */
	bool Process( void* input, uint32_t width, uint32_t height, imageFormat format, const char* ignore_class="void" );

	/**
 	 * Perform the initial inferencing processing portion of the segmentation.
	 * The results can then be visualized using the Overlay() and Mask() functions.
      * @deprecated this overload is for legacy compatibility.  It expects float4 RGBA image.
	 * @param input float4 input image in CUDA device memory, RGBA colorspace with values 0-255.
	 * @param width width of the input image in pixels.
	 * @param height height of the input image in pixels.
	 * @param ignore_class label name of class to ignore in the classification (or NULL to process all).
	 */
	bool Process( float* input, uint32_t width, uint32_t height, const char* ignore_class="void" );

	/**
	 * Produce a colorized segmentation mask.
	 * 这里只生成彩色分割结果，不和原图做叠加。
	 */
	template<typename T> bool Mask( T* output, uint32_t width, uint32_t height, FilterMode filter=FILTER_LINEAR )				{ return Mask((void*)output, width, height, imageFormatFromType<T>(), filter); }
	
	/**
	 * Produce a colorized segmentation mask.
	 */
	bool Mask( void* output, uint32_t width, uint32_t height, imageFormat format, FilterMode filter=FILTER_LINEAR );

	/**
	 * Produce a colorized RGBA segmentation mask.
	 * @deprecated this overload is for legacy compatibility.  It expects float4 RGBA image.
	 */
	bool Mask( float* output, uint32_t width, uint32_t height, FilterMode filter=FILTER_LINEAR );

	/**
	 * Produce a grayscale binary segmentation mask, where the pixel values
	 * correspond to the class ID of the corresponding class type.
	 */
	bool Mask( uint8_t* output, uint32_t width, uint32_t height );

	/**
	 * Produce the segmentation overlay alpha blended on top of the original image.
	 * 这一步依赖最近一次 Process() 保存下来的 mLastInputImg 和 mClassMap。
	 * @param output output image in CUDA device memory, RGB/RGBA colorspace with values 0-255.
	 * @param width width of the input image in pixels.
	 * @param height height of the input image in pixels.
	 * @param ignore_class label name of class to ignore in the classification (or NULL to process all).
	 * @param type overlay visualization options
	 * @returns true on success, false on error.
	 */
	template<typename T> bool Overlay( T* output, uint32_t width, uint32_t height, FilterMode filter=FILTER_LINEAR )			{ return Overlay((void*)output, width, height, imageFormatFromType<T>(), filter); }
	
	/**
	 * Produce the segmentation overlay alpha blended on top of the original image.
	 * @param output output image in CUDA device memory, RGB/RGBA colorspace with values 0-255.
	 * @param width width of the input image in pixels.
	 * @param height height of the input image in pixels.
	 * @param ignore_class label name of class to ignore in the classification (or NULL to process all).
	 * @param type overlay visualization options
	 * @returns true on success, false on error.
	 */
	bool Overlay( void* output, uint32_t width, uint32_t height, imageFormat format, FilterMode filter=FILTER_LINEAR );

	/**
	 * Produce the segmentation overlay alpha blended on top of the original image.
	 * @deprecated this overload is for legacy compatibility.  It expects float4 RGBA image.
	 * @param input float4 input image in CUDA device memory, RGBA colorspace with values 0-255.
	 * @param output float4 output image in CUDA device memory, RGBA colorspace with values 0-255.
	 * @param width width of the input image in pixels.
	 * @param height height of the input image in pixels.
	 * @param ignore_class label name of class to ignore in the classification (or NULL to process all).
	 * @param type overlay visualization options
	 * @returns true on success, false on error.
	 */
	bool Overlay( float* output, uint32_t width, uint32_t height, FilterMode filter=FILTER_LINEAR );

	/**
	 * Find the ID of a particular class (by label name).
	 */
	int FindClassID( const char* label_name );

	/**
	 * Retrieve the number of object classes supported in the detector
	 */
	inline uint32_t GetNumClasses() const						{ return DIMS_C(mOutputs[0].dims); }

	/**
	 * Retrieve the description of a particular class.
	 */
	inline const char* GetClassLabel( uint32_t id ) const			{ return GetClassDesc(id); }
	
	/**
	 * Retrieve the description of a particular class.
	 */
	inline const char* GetClassDesc( uint32_t id ) const			{ return id < mClassLabels.size() ? mClassLabels[id].c_str() : NULL; }
	
	/**
	 * Retrieve the RGBA visualization color a particular class.
	 */
	inline float4 GetClassColor( uint32_t id ) const				{ return mClassColors[id]; }

	/**
	 * Set the visualization color of a particular class of object.
	 */
	void SetClassColor( uint32_t classIndex, const float4& color );
	
	/**
	 * Set the visualization color of a particular class of object.
	 */
	void SetClassColor( uint32_t classIndex, float r, float g, float b, float a=255.0f );
	
	/**
	 * Retrieve the overlay alpha blending value for classes that don't have it explicitly set.
	 */
	float GetOverlayAlpha() const;
	
	/**
 	 * Set overlay alpha blending value for all classes (between 0-255),
	 * (optionally except for those that have been explicitly set).
	 */
	void SetOverlayAlpha( float alpha, bool explicit_exempt=true );

	/**
 	 * Retrieve the path to the file containing the class label descriptions.
	 */
	inline const char* GetClassPath() const						{ return mClassPath.c_str(); }

	/**
	 * Retrieve the number of columns in the classification grid.
	 * This indicates the resolution of the raw segmentation output.
	 */
	inline uint32_t GetGridWidth() const						{ return DIMS_W(mOutputs[0].dims); }

	/**
	 * Retrieve the number of rows in the classification grid.
	 * This indicates the resolution of the raw segmentation output.
	 */
	inline uint32_t GetGridHeight() const						{ return DIMS_H(mOutputs[0].dims); }

protected:
	segNet();
	
	/*
	 * classify() 的作用是把 TensorRT 的原始输出 mOutputs[0]
	 * 转成更紧凑的 mClassMap。
	 *
	 * 网络输出通常是 [C,H,W] 的分数张量，
	 * 但后续可视化更适合使用“每个位置一个 classID”的类别图。
	 */
	bool classify( const char* ignore_class );

	/*
	 * 这两个函数负责把 mClassMap 转成真正的输出图像：
	 *   overlayPoint()  = 最近邻采样，边界更硬，速度更快
	 *   overlayLinear() = 双线性插值，边界更平滑，显示更自然
	 *
	 * 正常情况下它们会继续调用 segNet.cu 里的 cudaSegOverlay()。
	 */
	bool overlayPoint( void* input, uint32_t in_width, uint32_t in_height, imageFormat in_format, void* output, uint32_t out_width, uint32_t out_height, imageFormat out_format, bool mask_only );
	bool overlayLinear( void* input, uint32_t in_width, uint32_t in_height, imageFormat in_format, void* output, uint32_t out_width, uint32_t out_height, imageFormat out_format, bool mask_only );
	
	bool loadClassColors( const char* filename );
	bool loadClassLabels( const char* filename );
	bool saveClassLegend( const char* filename );

	std::vector<std::string> mClassLabels;  /**< CPU: 类别名称列表，只在主机侧用于查找和显示 */
	std::string mClassPath;                 /**< CPU: 类别标签文件路径 */

	bool*    mColorsAlphaSet;	/**< CPU: 记录哪些类别颜色的 alpha 是显式设置的，便于 SetOverlayAlpha() 有选择地跳过 */
	float4*  mClassColors;		/**< CPU/GPU 共享映射内存: 每个类别的 RGBA 颜色，GPU kernel 会直接读取 */
	uint8_t* mClassMap;			/**< CPU/GPU 共享映射内存: argmax 后的类别图，每个输出网格位置一个 classID */
	
	/*
	 * 最近一次输入图像的缓存信息。
	 * 这些不是网络输出的一部分，而是为了让 Overlay() 在 Process() 结束后
	 * 仍然能拿到原图做 alpha blending。
	 */
	void*  	  mLastInputImg;	/**< GPU: 最近一次输入图像的设备指针，只缓存引用，不拥有其生命周期 */
	uint32_t 	  mLastInputWidth;	/**< 最近一次输入图像的宽度 */
	uint32_t 	  mLastInputHeight;	/**< 最近一次输入图像的高度 */
	imageFormat mLastInputFormat; /**< 最近一次输入图像的像素格式 */
};


#endif

