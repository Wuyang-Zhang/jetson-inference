<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="imagenet-example-python-2-CN.md">返回</a> | <a href="imagenet-camera-2-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>图像分类</sup></p>


# 编写您自己的图像识别程序 (C++)
在上一步中，我们运行了 jetson-inference 存储库附带的应用程序。


现在，我们将逐步从头开始创建一个名为 [`my-recognition`](../examples/my-recognition/my-recognition.cpp) 的图像识别新程序。  该程序将能够作为存储库之外的独立项目存在，因此如果您希望在自己的项目和应用程序中使用 jetson-inference 库，您可以按照此示例进行操作。


````.cpp
#include <jetson-inference/imageNet.h>
#include <jetson-utils/loadImage.h>


int main( int argc, char** argv )
{
	// 需要包含图像文件名的命令行参数，
	// 因此请确保我们至少有 2 个参数（第一个参数是程序）
	if( 参数 < 2 )
	{
		printf("my-recognition: 预期图像文件名作为参数\n");
		printf("用法示例: ./my-recognition my_image.jpg\n");
		返回0；
	}


// 从命令行参数数组中检索图像文件名
	const char* imgFilename = argv[1];


// 这些变量将存储图像数据指针和尺寸
	uchar3* imgPtr = NULL;   // 指向图像的共享CPU/GPU指针
	int imgWidth = 0;      // 图像的宽度（以像素为单位）
	int imgHeight = 0;      // 图像的高度（以像素为单位）


// 从磁盘加载图像为 uchar3 RGB（每像素 24 位）
	if( !loadImage(imgFilename, &imgPtr, &imgWidth, &imgHeight) )
	{
		printf("加载图像'%s'失败\n", imgFilename);
		返回0；
	}


// 使用 TensorRT 加载 GoogleNet 图像识别网络
	// 您可以使用“resnet-18”来加载 ResNet-18 模型
	imageNet* net = imageNet::Create("googlenet");


// 检查以确保网络模型正确加载
	如果（！net）
	{
		printf("加载图像识别网络失败\n");
		返回0；
	}


// 该变量将存储分类的置信度（0 到 1 之间）
	浮动置信度 = 0.0;


// 对图像进行分类，返回对象类索引（错误时返回-1）
	const int classIndex = net->Classify(imgPtr, imgWidth, imgHeight, &confidence);


// 确保返回了有效的分类结果	
	if( 类索引 >= 0 )
	{
		// 检索对象类索引的名称/描述
		const char* classDescription = net->GetClassDesc(classIndex);


// 打印出分类结果
		printf("图像被识别为'%s'（#%i 类），置信度为 %f%%\n", 
			  类描述、类索引、置信度 * 100.0f);
	}
	否则
	{
		// 如果 Classify() 返回 < 0，则发生错误
		printf("图像分类失败\n");
	}


// 关闭前释放网络资源
	删除网络；
	返回0；
}
````


为了您的方便和参考，已完成的文件可在存储库的 [`examples/my-recognition`](../examples/my-recognition) 目录中找到，但下面的指南将像它们驻留在用户的主目录或您选择的任意目录中一样。


## 设置项目


您可以将我们将创建的 `my-recognition` 示例存储在 Jetson 上的任何位置。


为简单起见，本指南将在位于 `~/my-recognition` 的用户主目录中创建它。


从终端运行这些命令以创建所需的目录和文件: 


``` bash
$ mkdir ~/my-recognition
$ cd ~/my-recognition
$ touch my-recognition.cpp
$ touch CMakeLists.txt
$ wget https://github.com/dusty-nv/jetson-inference/raw/master/data/images/black_bear.jpg 
$ wget https://github.com/dusty-nv/jetson-inference/raw/master/data/images/brown_bear.jpg
$ wget https://github.com/dusty-nv/jetson-inference/raw/master/data/images/polar_bear.jpg 
```


一些测试图像也会使用上面的 `wget` 命令下载到文件夹中。


接下来，我们将程序的代码添加到我们在此处创建的空源文件中。


## 源代码


在您选择的编辑器中打开 `my-recognition.cpp` （或运行 `gedit my-recognition.cpp`）。


让我们开始添加必要的代码以使用 [`imageNet`](../c/imageNet.h) 类来识别图像。


#### 包括


首先，包含我们需要的几个标头: 


````.cpp
// 包含用于图像识别的 imageNet 标头
#include <jetson-inference/imageNet.h>


// 包含用于加载图像的 loadImage 标头
#include <jetson-utils/loadImage.h>
````

> **注意**: 这些标头在[构建](building-repo-2-CN.md#compiling-the-project)的`sudo make install`步骤期间安装在`/usr/local/include`下。

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;如果您没有运行 `sudo make install`，那么当我们编译示例时将找不到这些标头。


#### 声明 main() 并解析命令行


接下来，声明您的 `main()` 方法并验证该程序是否已使用图像文件名作为命令行上的参数启动: 


````.cpp
// 主入口点
int main( int argc, char** argv )
{
	// 需要包含图像文件名的命令行参数，
	// 因此请确保我们至少有 2 个参数（第一个参数是程序）
	if( 参数 < 2 )
	{
		printf("my-recognition: 预期图像文件名作为参数\n");
		printf("用法示例: ./my-recognition my_image.jpg\n");
		返回0；
	}


// 从命令行参数数组中检索图像文件名
	const char* imgFilename = argv[1];
````


此示例加载并分类用户指定的图像。  预计它将像这样运行: 


``` bash
$ ./my-recognition my_image.jpg
```


所需加载的图像文件名应替换为 `my_image.jpg`。  上面的代码确保将此命令行参数提供给程序。


#### 从磁盘加载图像


声明一些变量来存储图像的尺寸和指向其内存的指针，然后使用 [`loadImage()`](https://github.com/dusty-nv/jetson-utils/blob/master/image/imageIO.h) 函数从磁盘加载图像。


````.cpp
	// 这些变量将存储图像数据指针和尺寸
	uchar3* imgPtr = NULL;   // 指向图像的共享CPU/GPU指针
	int imgWidth = 0;      // 图像的宽度（以像素为单位）
	int imgHeight = 0;      // 图像的高度（以像素为单位）


// 从磁盘加载图像为 uchar3 RGB（每像素 24 位）
	if( !loadImage(imgFilename, &imgPtr, &imgWidth, &imgHeight) )
	{
		printf("加载图像'%s'失败\n", imgFilename);
		返回0；
	}
````


加载的图像将存储在映射到 CPU 和 GPU 的共享内存中。  图像以 `uchar3` RGB 格式加载，但像素类型可以是 `uchar3, uchar4, float3, float4`。  有关详细信息，请参阅[使用 CUDA 进行图像操作](aux-image-CN.md) 页面。


#### 加载图像识别网络


使用 [`imageNet::Create()`](../c/imageNet.h#L108) 函数，以下代码将使用 TensorRT 加载 GoogleNet 模型，该模型在您最初[构建存储库](building-repo-2-CN.md#downloading-models) 时已下载。  该模型在 ImageNet ILSVRC12 数据集上进行了预训练，该数据集可以识别多达 [1000 种不同类别](../data/networks/ilsvrc12_synset_words.txt) 的物体，例如不同种类的水果和蔬菜、许多不同种类的动物，以及日常人造物体，例如车辆、办公家具、运动器材等。


````.cpp
	// 使用 TensorRT 加载 GoogleNet 图像识别网络
	// 您可以使用 imageNet::RESNET_18 来加载 ResNet-18
	imageNet* net = imageNet::Create(imageNet::GOOGLENET);


// 检查以确保网络模型正确加载
	如果（！net）
	{
		printf("加载图像识别网络失败\n");
		返回0；
	}
````


如果需要，您可以通过将传递给 `imageNet::Create()` 的枚举更改为[此表](imagenet-console-2-CN.md#downloading-other-classification-models) 中列出的枚举之一来加载其他预训练模型。  默认情况下，ResNet-18 (`imageNet::RESNET_18`) 可与 GoogleNet 一起使用。  对于其他模型，您可能需要返回并[下载](imagenet-console-2-CN.md#downloading-other-classification-models)您最初在构建过程中未选择的模型。



#### 对图像进行分类


接下来，我们将使用 [`imageNet::Classify()`](../c/imageNet.h#L159) 函数通过图像识别网络对图像进行分类: 


````.cpp
	// 该变量将存储分类的置信度（0 到 1 之间）
	浮动置信度 = 0.0;


// 对图像进行分类，返回对象类索引（错误时返回-1）
	const int classIndex = net->Classify(imgPtr, imgWidth, imgHeight, &confidence);
````


[`imageNet::Classify()`](../c/imageNet.h#L159) 接受 GPU 内存中的图像指针，并使用 TensorRT 执行推理。


它返回图像被识别为的对象类的索引以及结果的置信度值。


#### 解释结果


除非调用 [`imageNet::Classify()`](../c/imageNet.h#L159) 导致错误，否则让我们打印出已识别对象的分类信息: 


````.cpp
	// 确保返回了有效的分类结果	
	if( 类索引 >= 0 )
	{
		// 检索对象类索引的名称/描述
		const char* classDescription = net->GetClassDesc(classIndex);


// 打印出分类结果
		printf("图像被识别为'%s'（#%i 类），置信度为 %f%%\n", 
			  类描述、类索引、置信度 * 100.0f);
	}
	否则
	{
		// 如果 Classify() 返回 < 0，则发生错误
		printf("图像分类失败\n");
	}
````


由于 [`imageNet::Classify()`](../c/imageNet.h#L159) 返回对象类的基于整数的索引（对于 ILSVRC12，索引在 0 到 1000 之间），因此我们使用 [`imageNet::GetClassDesc()`](../c/imageNet.h#L140) 函数来检索对象的人类可读描述。


当网络加载时，这 1000 个类的描述是从 [`ilsvrc12_synset_words.txt`](../data/networks/ilsvrc12_synset_words.txt) 解析的（此文件之前是在构建 jetson-inference 存储库时下载的）。


#### 关闭


在退出程序之前，`delete` 网络对象以销毁 TensorRT 引擎并释放 CUDA 资源: 


````.cpp
	// 关闭前释放网络资源
	删除网络；


// 示例到此结束！
	返回0；
}
````


就是这样！  请记住将 return 语句和右大括号添加到 main() 方法中。


接下来我们只需要使用 CMake 为我们的新识别程序创建一个简单的 makefile。


## 创建 CMakeLists.txt


在编辑器中打开文件`~/my-recognition/CMakeLists.txt`，并添加以下代码: 


``` cmake
# 需要 CMake 2.8 或更高版本
cmake_minimum_required（版本2.8）


# 声明我的识别项目
项目（我的认可）


# 导入 jetson-inference 和 jetson-utils 包。
# 请注意，如果您没有执行“sudo make install”
# 在构建 jetson-inference 时，这会出错。
find_package(jetson-utils)
find_package(jetson-inference)


# 需要CUDA
查找包（CUDA）


# 添加 libnvbuf-utils 的目录进行编程
link_directories(/usr/lib/aarch64-linux-gnu/tegra)


# 编译my-recognition程序
cuda_add_executable（我的识别我的识别.cpp）


# 将 my-recognition 链接到 jetson-inference 库
target_link_libraries（我的识别jetson推理）
````


将来您可以使用此 CMakeLists 作为模板来编译您自己的使用 `jetson-inference` 库的项目。  最相关的位是: 


*  拉入 `jetson-utils` 和 `jetson-inference` 项目: 

``` cmake
		find_package(jetson-utils)
		find_package(jetson-inference)
	```

*  针对 `libjetson-inference` 的链接: 

``` cmake
		target_link_libraries(my-recognition jetson-inference)
	```


> **注意**: 这些库在[构建](building-repo-2-CN.md#compiling-the-project)的`sudo make install`步骤期间安装在`/usr/local/lib`下。

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;如果您没有运行 `sudo make install`，那么当我们编译示例时将找不到这些库。


## 构建示例


现在我们的源文件已完成，运行以下 shell 命令来编译 `my-recognition` 程序: 


``` bash
$ cd ~/my-recognition
$ cmake .
$ make
```


如果遇到错误，请确保在[构建 jetson-inference 存储库](building-repo-2-CN.md#compiling-the-project) 时运行 `sudo make install`。


您还可以从存储库的 [`examples/my-recognition`](../examples/my-recognition) 目录下载此示例的完整工作代码。


## 运行示例


现在我们的程序已编译完毕，让我们对本指南开头[下载](#setting-up-the-project)的测试图像进​​行分类: 


``` bash
$ ./my-recognition polar_bear.jpg
image is recognized as 'ice bear, polar bear, Ursus Maritimus, Thalarctos maritimus' (class #296) with 99.999878% confidence
```
<img src="https://github.com/dusty-nv/jetson-inference/raw/master/data/images/polar_bear.jpg" width="400">


``` bash
$ ./my-recognition brown_bear.jpg
image is recognized as 'brown bear, bruin, Ursus arctos' (class #294) with 99.928925% confidence
```
<img src="https://github.com/dusty-nv/jetson-inference/raw/master/data/images/brown_bear.jpg" width="400">


``` bash
$ ./my-recognition black_bear.jpg
image is recognized as 'American black bear, black bear, Ursus americanus, Euarctos americanus' (class #295) with 98.898628% confidence
```
<img src="https://github.com/dusty-nv/jetson-inference/raw/master/data/images/black_bear.jpg" width="400">


这是本教程这一部分的结论。  接下来，我们将对 Jetson 机载摄像头的实时视频源进行分类。


##
<palign="right">下一个| <b><a href="imagenet-camera-2-CN.md">运行实时摄像头识别演示</a></b>
<br/>
返回 | <b><a href="imagenet-example-python-2-CN.md">编写您自己的图像识别程序 (Python)</a></b></p>
<palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>