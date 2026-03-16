<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg">
<palign="right"><sup><a href="jetpack-setup-CN.md">返回</a> | <a href="imagenet-console-CN.md">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>系统设置</sup></p>


# 从源代码构建项目


该存储库提供了一个 TensorRT 加速深度学习网络库，用于图像识别、定位目标检测（即边界框）和语义分割。  此推理库 (`libjetson-inference`) 旨在在 Jetson 上构建和运行，并包括对 C++ 和 Python 的支持。


自动下载各种预先训练的 DNN 模型，让您快速启动并运行。  它还设置为接受您可能自己训练过的自定义模型，包括对 Caffe、TensorFlow UFF 和 ONNX 的支持。


可以从 [GitHub](http://github.com/dusty-nv/jetson-inference) 获取最新源代码，并在[使用 JetPack 刷新](jetpack-setup-CN.md) 或使用 Jetson Nano 的预填充 [SD 卡映像](https://developer.nvidia.com/embedded/learn/get-started-jetson-nano-devkit#write) 设置后，在 Jetson Nano、Jetson TX1/TX2 和 Jetson AGX Xavier 上进行编译。


### 快速参考


以下是下载、构建和安装项目的命令的精简形式: 


``` bash
$ sudo apt-get update
$ sudo apt-get install git cmake libpython3-dev python3-numpy
$ git clone --recursive https://github.com/dusty-nv/jetson-inference
$ cd jetson-inference
$ mkdir build
$ cd build
$ cmake ../
$ make -j$(nproc)
$ sudo make install
$ sudo ldconfig
```
下面我们将逐步完成每个步骤并讨论各种构建选项。


### 克隆存储库


要下载代码，请导航到您在 Jetson 上选择的文件夹。  首先，确保已安装 git 和 cmake: 


``` bash
$ sudo apt-get update
$ sudo apt-get install git cmake
```


然后克隆 `jetson-inference` 项目: 


``` bash
$ git clone https://github.com/dusty-nv/jetson-inference
$ cd jetson-inference
$ git submodule update --init
```


请记住运行 `git submodule update --init` 步骤（或使用 `--recursive` 标志进行克隆）。


### Python 开发包


该项目的 Python 功能是通过 Python 扩展模块实现的，这些扩展模块使用 Python C API 提供与本机 C++ 代码的绑定。  配置项目时，存储库会搜索系统上安装了开发包的 Python 版本，然后为现有的每个 Python 版本（例如 Python 2.7、3.6 和 3.7）构建绑定。  它还将为已安装的 numpy 版本构建 numpy 绑定。


默认情况下，Ubuntu 预装了 `libpython-dev` 和 `python-numpy` 软件包（适用于 Python 2.7）。  尽管 Ubuntu 预装了 Python 3.6 解释器，但 Python 3.6 开发包 (`libpython3-dev`) 和 `python3-numpy` 却没有预装。  使用 Python C API 构建绑定需要这些开发包。


因此，如果您希望项目为 Python 3.6 创建绑定，请在继续之前安装这些包: 


``` bash
$ sudo apt-get install libpython3-dev python3-numpy
```


安装这些附加包将使存储库能够为 Python 3.6 以及 Python 2.7（已预安装）构建扩展绑定。  然后，在构建过程之后，[`jetson.inference`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html) 和 [`jetson.utils`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.utils.html) 包将可以在您的 Python 环境中使用。



### 使用 CMake 配置


接下来，在项目中创建一个构建目录并运行 `cmake` 来配置构建。  运行 `cmake` 时，将启动一个脚本 ([`CMakePreBuild.sh`](../CMakePreBuild.sh))，它将安装任何所需的依赖项并为您下载 DNN 模型。


``` bash
$ cd jetson-inference    # omit if working directory is already jetson-inference/ from above
$ mkdir build
$ cd build
$ cmake ../
```


> **注意**: 此命令将启动 [`CMakePreBuild.sh`](../CMakePreBuild.sh) 脚本，该脚本在 Jetson 上安装一些必备软件包时需要 sudo 权限。该脚本还从 Web 服务下载预先训练的网络。



### 下载模型


该项目附带了许多预先训练的网络，您可以选择通过 **模型下载器** 工具 ([`download-models.sh`](../tools/download-models.sh)) 下载和安装。  默认情况下，最初不会选择下载所有模型以节省磁盘空间。  您可以选择所需的模型，也可以稍后再次运行该工具以下载更多模型。


最初配置项目时，`cmake` 将自动为您运行下载器工具: 


<img src="https://raw.githubusercontent.com/dusty-nv/jetson-inference/master/docs/images/download-models.jpg" width="650">


> **注意**: 对于无法连接Box.com下载模型的用户，这里提供了一个镜像: <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[`https://github.com/dusty-nv/jetson-inference/releases`](https://github.com/dusty-nv/jetson-inference/releases)


要稍后再次运行模型下载器工具，您可以使用以下命令: 


``` bash
$ cd jetson-inference/tools
$ ./download-models.sh
```


### 安装 PyTorch


如果您使用的是 JetPack 4.2 或更高版本，现在将运行另一个工具，如果您想在本教程后面使用 [迁移学习](pytorch-transfer-learning-CN.md) 重新训练网络，则可以选择在 Jetson 上安装 PyTorch。  此步骤是可选的，如果您不想执行迁移学习步骤，则无需安装 PyTorch，可以跳过此步骤。


如果需要，请选择要安装的 Python 2.7 和/或 Python 3.6 的 PyTorch 软件包版本，然后点击 `Enter` 继续。  否则，不选择这些选项，它将跳过 PyTorch 的安装。


<img src="https://raw.githubusercontent.com/dusty-nv/jetson-inference/master/docs/images/pytorch-installer.jpg" width="650">


> **注意**: 自动化 PyTorch 安装工具需要 JetPack 4.2（或更高版本）<br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;对于其他版本，请参阅 [`http://eLinux.org/Jetson_Zoo`](https://elinux.org/Jetson_Zoo#PyTorch_.28Caffe2.29) 从源代码构建。


如果您决定要在其他时间安装 PyTorch，您也可以稍后再次运行此工具: 


``` bash
$ cd jetson-inference/build
$ ./install-pytorch.sh
```


运行这些命令将提示您显示与上面相同的对话框。


### 编译项目


确保您仍在上面步骤 #3 中创建的 `jetson-inference/build` 目录中。


然后运行 ​​`make` ，然后运行 ​​`sudo make install` 来构建库、Python 扩展绑定和代码示例: 


``` bash
$ cd jetson-inference/build          # omit if working directory is already build/ from above
$ make
$ sudo make install
$ sudo ldconfig
```


该项目将构建到`jetson-inference/build/aarch64`，目录结构如下: 


```
|-build
   \aarch64
      \bin             where the sample binaries are built to
         \networks     where the network models are stored
         \images       where the test images are stored
      \include         where the headers reside
      \lib             where the libraries are build to
```


在构建树中，您可以在 `build/aarch64/bin/` 中找到二进制文件，在 `build/aarch64/include/` 中找到标头，在 `build/aarch64/lib/` 中找到库。  这些也会在 `sudo make install` 步骤期间安装在 `/usr/local/` 下。


[`jetson.inference`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html) 和 [`jetson.utils`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.utils.html) 模块的 Python 绑定也会在 `/usr/lib/python*/dist-packages/` 下的 `sudo make install` 步骤期间安装。  如果您更新了代码，请记住再次运行它。



### 深入研究代码


请参阅 **[API 参考](../README-CN.md#api-reference)** 文档，了解 `libjetson-inference` 中可用的视觉基元，包括用于图像识别的 `imageNet`、用于对象定位的 `detectNet` 和用于语义分割的 `segNet`。  熟悉这些对象的 C++ 或 Python 版本，具体取决于您喜欢使用哪种语言。


#### C++


下面是我们将在本教程后续步骤中使用的 [`imageNet`](../c/imageNet.h) C++ 类的部分列表: 


``c++
imageNet 类: 公共张量网络
{
公众: 
	/**

	 * 网络选择枚举。

*/
	枚举网络类型
	{
		CUSTOM, /**< 用户提供的自定义模型 */
		ALEXNET, /**< AlexNet 在 1000 级 ILSVRC12 上训练 */
		GOOGLENET, /**< GoogleNet 训练的 1000 级 ILSVRC12 */
		GOOGLENET_12, /**< GoogleNet 在教程中的 ImageNet ILSVRC12 的 12 类子集上进行训练 */
		RESNET_18, /**< 在 1000 级 ILSVRC15 上训练的 ResNet-18 */
		RESNET_50, /**< 在 1000 级 ILSVRC15 上训练的 ResNet-50 */
		RESNET_101, /**< ResNet-101 在 1000 级 ILSVRC15 上训练 */
		RESNET_152, /**< ResNet-50 在 1000 级 ILSVRC15 上训练 */
		VGG_16, /**< VGG-16 在 1000 级 ILSVRC14 上训练 */
		VGG_19, /**< VGG-19 在 1000 级 ILSVRC14 上训练 */
		INCEPTION_V4, /**< 在 1000 级 ILSVRC12 上训练的 Inception-v4 */
	};


/**

	 * 加载新的网络实例

*/
	静态 imageNet* 创建( NetworkType networkType=GOOGLENET, uint32_t maxBatchSize=DEFAULT_MAX_BATCH_SIZE, 
                              precisionType precision=TYPE_FASTEST,
                              deviceType device=DEVICE_GPU, bool allowedGPUFallback=true );


/**

	 * 加载新的网络实例

	 * @param prototxt_path 可部署网络prototxt的文件路径

	 * @param model_path caffemodel 的文件路径

	 * @param Mean_binary 平均值二进制原型的文件路径（可以为 NULL）

	 * @param class_labels 类名标签列表的文件路径

	 * @param input 输入层 blob 的名称。

	 * @param output 输出层 blob 的名称。

	 * @param maxBatchSize 网络将支持和优化的最大批量大小。

*/
	静态 imageNet* 创建( const char* prototxt_path, const char* model_path, 
                              const char* Mean_binary、const char* class_labels、 
                              const char* 输入=IMAGENET_DEFAULT_INPUT， 
                              const char* 输出=IMAGENET_DEFAULT_OUTPUT， 
                              uint32_t maxBatchSize=DEFAULT_MAX_BATCH_SIZE, 
                              precisionType precision=TYPE_FASTEST,
                              deviceType device=DEVICE_GPU, bool allowedGPUFallback=true );


/**

	 * 确定最大似然图像类别。

	 * 该函数对图像执行预处理（应用平均值减法和 NCHW 格式），@see PreProcess()

	 * @param rgba float4 输入图像在 CUDA 设备内存中。

	 * @param width 输入图像的宽度（以像素为单位）。

	 * @param height 输入图像的高度（以像素为单位）。

	 * @paramconfidence 可选指针，指向充满置信度值的浮点数。

	 * @returns 最大类的索引，如果出错则为 -1。

*/
	int Classify( float* rgba, uint32_t 宽度, uint32_t 高度, float* 置信度=NULL );


/**

	 * 检索图像识别类别的数量（通常为 1000）

*/
	内联 uint32_t GetNumClasses() const { return mOutputClasses; }


/**

	 * 检索特定类的描述。

*/
	内联 const char* GetClassDesc( uint32_t index ) const { return mClassDesc[index].c_str(); }
};
````


存储库中的所有 DNN 对象都继承自共享 [`tensorNet`](../c/tensorNet.h) 对象，其中包含公共 TensorRT 代码。


#### Python


以下是来自 [`jetson.inference`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html) 包的 Python [`imageNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#imageNet) 对象的缩写 pydoc 输出: 


```
jetson.inference.imageNet = class imageNet(tensorNet)
 |  Image Recognition DNN - classifies an image
 |  
 |  __init__(...)
 |       Loads an image recognition model.
 |  
 |       Parameters:
 |         network (string) -- name of a built-in network to use
 |                             values can be:  'alexnet', 'googlenet', 'googlenet-12', 'resnet-18`, ect.
 |                             the default is 'googlenet'
 |  
 |         argv (strings) -- command line arguments passed to imageNet,
 |                           for loading a custom model or custom settings
 |
 |  Classify(...)
 |      Classify an RGBA image and return the object's class and confidence.
 |      
 |      Parameters:
 |        image  (capsule) -- CUDA memory capsule
 |        width  (int) -- width of the image (in pixels)
 |        height (int) -- height of the image (in pixels)
 |      
 |      Returns:
 |        (int, float) -- tuple containing the object's class index and confidence
 |  
 |  GetClassDesc(...)
 |      Return the class description for the given object class.
 |      
 |      Parameters:
 |        (int) -- index of the class, between [0, GetNumClasses()]
 |      
 |      Returns:
 |        (string) -- the text description of the object class
 |
 |  GetNumClasses(...)
 |      Return the number of object classes that this network model is able to classify.
 |      
 |      Parameters:  (none)
 |      
 |      Returns:
 |        (int) -- number of object classes that the model supports
----------------------------------------------------------------------
```


接下来，我们将使用 `imageNet` 对象在 Python 或 C++ 中执行图像识别。


##
<palign="right">下一个| <b><a href="imagenet-console-CN.md">使用 ImageNet 对图像进行分类</a></b>
<br/>
返回 | <b><a href="jetpack-setup-CN.md">使用 JetPack 设置 Jetson</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>