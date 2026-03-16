<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="aux-streaming-CN.md">返回</a> | <a href="https://github.com/dusty-nv/ros_deep_learning">Next</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>附录</sup></p>


# 使用 CUDA 进行图像处理


本页介绍了 jetson-utils 与 CUDA 实现的许多图像格式、转换和预处理/后处理功能: 


**图像管理**

* [图像格式](#image-formats)

* [图像分配](#image-allocation)

* [复制图像](#copying-images)

* [Python 中的图像胶囊](#image-capsules-in-python)

	* [数组接口](#array-interfaces)

	* [在Python中访问图像数据](#accessing-image-data-in-python)

	* [作为 Numpy 数组访问](#accessing-as-a-numpy-array)

	* [CUDA 数组接口](#cuda-array-interface)

	* [共享内存指针](#sharing-the-memory-pointer)


**CUDA例程**

* [颜色转换](#color-conversion)

* [调整大小](#resizing)

* [裁剪](#cropping)

* [标准化](#normalization)

* [叠加](#overlay)

* [绘制形状](#drawing-shapes)


有关使用这些函数的示例，请参阅 [`cuda-examples.py`](https://github.com/dusty-nv/jetson-utils/tree/master/python/examples/cuda-examples.py) 除了下面的伪代码之外。  在深入讨论之前，建议阅读[相机流媒体和多媒体](aux-streaming-CN.md) 上的上一页，了解有关视频捕获和输出、加载/保存图像等的信息。


## 图像格式


尽管[视频流](aux-streaming#source-code) API 和 DNN 对象（例如 [`imageNet`](c/imageNet.h)、[`detectNet`](c/detectNet.h) 和 [`segNet`](c/segNet.h)）期望图像采用 RGB/RGBA 格式，但为传感器采集和低级 I/O 定义了多种其他格式: 


|                 |格式化字符串| [`imageFormat` 枚举](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/group__imageFormat.html#ga931c48e08f361637d093355d64583406) |数据类型 |位深度|
|----------------|-------------|--------------------|------------|-----------|
| **RGB/RGBA** | `rgb8` | `IMAGE_RGB8` | `uchar3` | 24 |
|                 | `rgba8` | `IMAGE_RGBA8` | `uchar4` | 32 | 32
|                 | `rgb32f` | `IMAGE_RGB32F` | `float3` | 96 | 96
|                 | `rgba32f` | `IMAGE_RGBA32F` | `float4` | 128 | 128
| **BGR/BGRA** | `bgr8` | `IMAGE_BGR8` | `uchar3` | 24 |
|                 | `bgra8` | `IMAGE_BGRA8` | `uchar4` | 32 | 32
|                 | `bgr32f` | `IMAGE_BGR32F` | `float3` | 96 | 96
|                 | `bgra32f` | `IMAGE_BGRA32F` | `float4` | 128 | 128
| **YUV (4:2:2)** | `yuyv` | `IMAGE_YUYV` | `uint8` | 16 | 16
|                 | `yuy2` | `IMAGE_YUY2` | `uint8` | 16 | 16
|                 | `yvyu` | `IMAGE_YVYU` | `uint8` | 16 | 16
|                 | `uyvy` | `IMAGE_UYVY` | `uint8` | 16 | 16
| **YUV (4:2:0)** | `i420` | `IMAGE_I420` | `uint8` | 12 | 12
|                 | `yv12` | `IMAGE_YV12` | `uint8` | 12 | 12
|                 | `nv12` | `IMAGE_NV12` | `uint8` | 12 | 12
| **拜耳** | `bayer-bggr` | `IMAGE_BAYER_BGGR` | `uint8` | 8 |
|                 | `bayer-gbrg` | `IMAGE_BAYER_GBRG` | `uint8` | 8 |
|                 | `bayer-grbg` | `IMAGE_BAYER_GRBG` | `uint8` | 8 |
|                 | `bayer-rggb` | `IMAGE_BAYER_RGGB` | `uint8` | 8 |
| **灰度** | `gray8` | `IMAGE_GRAY8` | `uint8` | 8 |
|                 | `gray32f` | `IMAGE_GRAY32F` | `float` | 32 | 32

* 位深度表示每个像素的有效位数

* 有关 YUV 格式的详细规范，请参阅 [fourcc.org](http://fourcc.org/yuv.php)


> **注意: ** 在 C++ 中，RGB/RGBA 格式是唯一应与 `uchar3`/`uchar4`/`float3`/`float4` 向量类型一起使用的格式。  假设使用这些类型时，图像为 RGB/RGBA 格式。


要在数据格式和/或色彩空间之间转换图像，请参阅下面的[颜色转换](#color-conversion)部分。


## 图像分配


要分配空 GPU 内存来存储中间/输出图像（即处理期间的工作内存），请使用 C++ 中的一个 [`cudaAllocMapped()`](https://github.com/dusty-nv/jetson-utils/tree/master/cuda/cudaMappedMemory.h) 或 Python 中的 [`cudaImage`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.utils.html#cudaImage) 对象。  请注意，[`videoSource`](aux-streaming#source-code) 输入流会自动分配自己的 GPU 内存，并返回最新的图像，因此您无需为这些图像分配自己的内存。


[`cudaAllocMapped()`](https://github.com/dusty-nv/jetson-utils/tree/master/cuda/cudaMappedMemory.h) 分配的内存驻留在共享的 CPU/GPU 内存空间中，因此可以从 CPU 和 GPU 访问它，而无需在它们之间执行内存复制（因此也称为 ZeroCopy 内存）。


然而，同步是必需的 - 因此，如果您想在 GPU 处理完成后从 CPU 访问图像，请首先调用 [`cudaDeviceSynchronize()`](https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__DEVICE.html#group__CUDART__DEVICE_1g10e20b05a95f638a4071a655503df25d)。  要释放 C++ 中的内存，请使用 [`cudaFreeHost()`](https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__MEMORY.html#group__CUDART__MEMORY_1g71c078689c17627566b2a91989184969) 函数。  在 Python 中，内存将由垃圾收集器自动释放，但您可以使用 `del` 运算符显式释放它。


下面是用于分配/同步/释放 ZeroCopy 内存的 Python 和 C++ 伪代码: 


####Python
````蟒蛇
从 jetson_utils 导入 cudaImage、cudaDeviceSynchronize


# 分配一个rgb8格式的1920x1080图像
img = cudaImage(宽度=1920，高度=1080，格式='rgb8')


# 在这里在 GPU 上做一些处理
...


#等待GPU完成处理
cudaDeviceSynchronize()


# Python 会自动释放内存，但你可以使用 'del' 显式地释放内存
删除图像
````


#### C++
````cpp
#include <jetson-utils/cudaMappedMemory.h>


无效* img = NULL;


// 分配一个rgb8格式的1920x1080图像
if( !cudaAllocMapped(&img, 1920, 1080, IMAGE_RGB8) )
	返回假；	//内存错误


// 这里在GPU上做一些处理 
...


// 等待GPU处理完成
CUDA(cudaDeviceSynchronize());


//释放内存
CUDA(cudaFreeHost(img));
````


在 C++ 中，如果指针类型为 `uchar3/uchar4/float3/float4`，则通常可以省略显式 [`imageFormat`](#image-formats) 枚举。  下面的功能与上面的分配相同: 


````cpp
uchar3* img = NULL;	// 可以是 uchar3 (rgb8)、uchar4 (rgba8)、float3 (rgb32f)、float4 (rgba32f)


if( !cudaAllocMapped(&img, 1920, 1080) )
	返回假；	
````


> **注意: ** 使用这些矢量类型时，将假定这些图像位于各自的 RGB/RGBA 色彩空间中。  因此，如果您使用 `uchar3/uchar4/float3/float4` 表示包含 BGR/BGRA 数据的图像，则它可能会被某些处理函数解释为 RGB/RGBA，除非您明确指定正确的[图像格式](#image-formats)。


## 复制图像


[`cudaMemcpy()`](https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__MEMORY.html#group__CUDART__MEMORY_1gc263dbe6574220cc776b45438fc351e8) 可用于在相同格式和尺寸的图像之间复制内存。  它是 C++ 中的标准 CUDA 函数，jetson_utils 库中有一个类似的 Python 版本: 


####Python
````蟒蛇
从 jetson_utils 导入 cudaMemcpy、cudaImage、loadImage


# 加载图像并分配内存以将其复制到
img_a = loadImage("my_image.jpg")
img_b = cudaImage(like=img_a) # 指定宽度、高度、格式的简写


# 复制图像（dst，src）
cudaMemcpy（img_b，img_a）


# 或者你可以使用这个快捷方式，这将返回一个重复的
img_c = cudaMemcpy(img_a)
````


#### C++
````cpp
#include <jetson-utils/cudaMappedMemory.h>
#include <jetson-utils/imageIO.h>


uchar3* img_a = NULL;
uchar3* img_b = NULL;


整数宽度=0；
int 高度 = 0;


// 加载示例图像
if( !loadImage("my_image.jpg", &img_a, &width, &height) )
	返回假；	// 加载错误


// 分配内存以将其复制到
if( !cudaAllocMapped(&img_b, 宽度, 高度) )
	返回假；  //内存错误


// 复制图像 (dst, src)
if( CUDA_FAILED(cudaMemcpy(img_b, img_a, 宽度 * 高度 * sizeof(uchar3), cudaMemcpyDeviceToDevice)) )
	返回假；  // 内存复制错误
````


## Python 中的图像胶囊


当您在 Python 中分配图像或使用 [`videoSource.Capture()`](aux-streaming#source-code) 从视频源捕获图像时，它将返回一个独立的内存胶囊对象（类型为 [`<jetson_utils.cudaImage>`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.utils.html#cudaImage)），该对象可以在无需复制底层内存的情况下进行传递。


[`cudaImage`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.utils.html#cudaImage) 对象具有以下成员: 


```python
<cudaImage object>
  .ptr       # memory address (not typically used)
  .size      # size in bytes
  .shape     # (height,width,channels) tuple
  .width     # width in pixels
  .height    # height in pixels
  .channels  # number of color channels
  .format    # format string
  .mapped    # true if ZeroCopy
  .timestamp # timestamp in nanoseconds
```


因此，您可以执行 `img.width` 和 `img.height` 之类的操作来访问有关图像的属性。


### 数组接口


为了与其他库进行零拷贝互操作，有多种方法可以从 Python 访问 `cudaImage` 内存: 


* [直接从 Python 索引图像](#accessing-image-data-in-python)

* [Numpy `__array__` 接口](#accessing-as-a-numpy-array)、[`cudaToNumpy()`](#converting-to-numpy-arrays)、[`cudaFromNumpy()`](#converting-from-numpy-arrays)

* [Numba `__cuda_array_interface__`](#cuda-array-interface)（PyTorch、CuPy、PyCUDA、VPI 等）

* [共享内存指针](#sharing-the-memory-pointer)


这些的实现是为了映射底层内存并与其他库共享，以避免内存复制。


### 在 Python 中访问图像数据


CUDA 图像是可下标的，这意味着您可以对它们进行索引以直接从 CPU 访问像素数据: 


```python
for y in range(img.height):
    for x in range(img.width):
        pixel = img[y,x]    # returns a tuple, i.e. (r,g,b) for RGB formats or (r,g,b,a) for RGBA formats
        img[y,x] = pixel    # set a pixel from a tuple (tuple length must match the number of channels)
```


> **注意: ** Python 下标索引运算符仅在 cudaImage 分配有 `mapped=True` （这是默认值）时才可用。  否则，CPU 无法访问数据，并且会抛出异常。


用于访问图像的索引元组可以采用以下形式: 


* `img[y,x]` - 注意 `(y,x)` 元组的顺序，与 numpy 相同

* `img[y,x,channel]` - 仅访问特定通道（即 0 表示红色，1 表示绿色，2 表示蓝色，3 表示 Alpha）

* `img[y*img.width+x]` - 平面一维索引，访问该像素中的所有通道


尽管支持图像下标，但不建议通过 Python 单独访问大图像的每个像素，因为这会显着减慢应用程序的速度。  假设 GPU 实现不可用，更好的替代方案是使用 Numpy。


### 作为 Numpy 数组访问


cudaImage支持Numpy [`__array__`](https://numpy.org/doc/stable/reference/arrays.interface.html)接口协议，因此它可以像Numpy数组一样在许多Numpy函数中使用，而无需来回复制。  有关简单示例，请参阅 [`cuda-to-numpy.py`](https://github.com/dusty-nv/jetson-utils/blob/master/python/examples/cuda-to-numpy.py): 


```` 蟒蛇
将 numpy 导入为 np
从 jetson_utils 导入 cudaImage


cuda_img = cudaImage(320, 240, 'rgb32f')
数组 = np.ones(cuda_img.shape, np.float32)


打印（np.add（cuda_img，数组））
````


> **注意: ** Numpy 在 CPU 上运行，因此 cudaImage 应该分配有 `mapped=True` （默认值），以便 CPU 和 GPU 都可以访问它的内存。  Numpy 对它的任何更改都将反映在底层 cudaImage 的内存中。


您需要使用独立的 [numpy 例程](https://numpy.org/doc/stable/reference/routines.html)，而不是 [ndarray 类方法](https://numpy.org/doc/stable/reference/generated/numpy.ndarray.html)（例如使用 `numpy.mean(array) vs array.mean()`），因为虽然 cudaImage 导出 `__array__` 接口来访问其内存，但它没有实现 Numpy 所实现的类方法。  要使用所有这些，请参阅下面的 [`cudaToNumpy()`](#converting-to-numpy-arrays) 函数。


#### 转换为 Numpy 数组


您可以通过调用 `cudaToNumpy()` 显式获取映射到 `cudaImage` 的 [`numpy.ndarray`](https://numpy.org/doc/stable/reference/generated/numpy.ndarray.html) 对象: 


```` 蟒蛇
将 numpy 导入为 np
从 jetson_utils 导入 cudaImage、cudaToNumpy


cuda_img = cudaImage(320, 240, 'rgb32f')
数组 = cudaToNumpy(cuda_img)


打印(数组.mean())
````


和以前一样，底层内存不会被复制，Numpy 将直接访问它 - 因此，如果您通过 Numpy 就地更改数据，它也会在底层 `cudaImage` 中发生更改。  有关使用 `cudaToNumpy()` 的示例，请参阅 jetson-utils 中的 [`cuda-to-numpy.py`](https://github.com/dusty-nv/jetson-utils/blob/master/python/examples/cuda-to-numpy.py) 示例。


请注意，如果您打算在 OpenCV 中使用图像，OpenCV 需要 BGR 色彩空间中的图像，因此您应该首先调用 [`cudaConvertColor()`](#color-conversion) 将其从 RGB 转换为 BGR（有关示例，请参阅 [`cuda-to-cv.py`](https://github.com/dusty-nv/jetson-utils/blob/master/python/examples/cuda-to-cv.py)）。


#### 从 Numpy 数组转换


假设您在 Numpy ndarray 中有一个图像，可能由 OpenCV 提供 - 作为 Numpy 数组，它只能从 CPU 访问。  您可以使用 `cudaFromNumpy()` 将其复制到 GPU（共享 CPU/GPU 映射内存）。  有关示例，请参阅 [`cuda-from-numpy.py`](https://github.com/dusty-nv/jetson-utils/blob/master/python/examples/cuda-from-numpy.py) 示例: 


```` 蟒蛇
将 numpy 导入为 np
从 jetson_utils 导入 cudaFromNumpy


数组 = np.zeros((240, 320, 3), dtype=np.float32)
cuda_img = cudaFromNumpy(数组)
````


与之前一样，如果您使用 OpenCV，OpenCV 图像位于 BGR 色彩空间中，您应该在之后调用 [`cudaConvertColor()`](#color-conversion) 将其从 BGR 转换为 RGB（有关示例，请参阅 [`cuda-from-cv.py`](https://github.com/dusty-nv/jetson-utils/blob/master/python/examples/cuda-from-cv.py)）。


### CUDA 阵列接口


`cudaImage` 还支持 Numba 的 [`__cuda_array_interface__`](https://numba.readthedocs.io/en/stable/cuda/cuda_array_interface.html)，它在使用 GPU 内存的库（包括 PyTorch、CuPy、PyCUDA、VPI 和其他[此处列出](https://numba.readthedocs.io/en/stable/cuda/cuda_array_interface.html#interoperability)）之间提供零拷贝互操作性。   与 `cudaImage` 实现 Numpy [`__array__`](#accessing-as-a-numpy-array) 接口的方式类似，`__cuda_array_interface__` 对于将 cudaImage 传递到 Numba/PyTorch/CuPy/PyCUDA 函数是透明的，并且内存与这些库共享，因此没有内存副本或额外开销。


有关示例，请参阅 [`cuda-array-interface.py`](https://github.com/dusty-nv/jetson-utils/blob/master/python/examples/cuda-array-interface.py) 中的代码: 


```` 蟒蛇
导入丘比
从 jetson_utils 导入 cudaImage


cuda_img = cudaImage(640, 480, 'rgb8')
cupy_array = cupy.ones((480, 640, 3))


打印（cupy.add（cuda_img，cupy_array））
````


> **注意: ** `cudaImage` 还实现了 [PyCUDA 的 `gpudata` 接口](https://documen.tician.de/pycuda/array.html)，并且可以像 PyCUDA `GPUArray` 一样使用。


另一个与 PyTorch 张量共享内存的示例可以在 [`cuda-to-pytorch.py`](https://github.com/dusty-nv/jetson-utils/blob/master/python/examples/cuda-to-pytorch.py) 中找到: 


```` 蟒蛇
进口火炬
从 jetson_utils 导入 cudaImage


# 分配cuda内存
cuda_img = cudaImage(640, 480, 'rgb8')


# 使用 __cuda_array_interface__ 映射到 torch 张量
张量 = torch.as_tensor(cuda_img, device='cuda')
````


这使得 PyTorch GPU 张量可以使用 cudaImage 中的 GPU 内存，而无需复制它 - PyTorch 对张量内容所做的任何更改都将反映在 cudaImage 中。  请注意，应指定 `device='cuda'` 以便 PyTorch 执行零拷贝映射 - 您可以通过确认 cudaImage 和 PyTorch 张量对象之间的数据指针匹配来检查这一点。


### 共享内存指针


对于不支持上述接口之一的库，cudaImage 通过其 `.ptr` 属性公开其内存的原始数据指针，可用于将其导入到其他数据结构中而无需复制它。  相反，cudaImage 初始化程序也有一个 `ptr` 参数，可以设置为外部分配的缓冲区 - 在这种情况下，cudaImage 将共享内存而不是分配它自己的内存。


有关执行此操作的示例，请参阅 [`cuda-from-pytorch.py`](https://github.com/dusty-nv/jetson-utils/blob/master/python/examples/cuda-from-pytorch.py)，其中现有 PyTorch GPU 张量映射到 cudaImage: 


```` 蟒蛇
进口火炬
从 jetson_utils 导入 cudaImage


# 分配一个具有 NCHW 布局的 GPU 张量（跨步颜色）
张量 = torch.rand(1, 3, 480, 640, dtype=torch.float32, device='cuda')


# 将通道转置为 NHWC 布局（交错颜色）
张量 = 张量.to(memory_format=torch.channels_last) # 或张量.permute(0, 3, 2, 1)


# 使用相同的底层内存映射到 cudaImage（任何更改都将反映在 PyTorch 张量中）
cuda_img = cudaImage(ptr=tensor.data_ptr(), width=tensor.shape[-1], height=tensor.shape[-2], format='rgb32f')
````


> **注意: ** 请注意 NCHW [通道布局](https://pytorch.org/blog/tensor-memory-format-matters/#memory-formats-supported-by-pytorch-operators)（跨步颜色）与 NHWC 布局（交错颜色），因为 cudaImage 期望后者。


当外部指针映射到 cudaImage 时，默认情况下 cudaImage 不会获取底层内存的所有权，并且在释放 cudaImage 时不会释放它（要更改此设置，请在初始化程序中设置 `freeOnDelete=True` ）。  处理库之间的同步应该由用户实现（例如，这样 PyTorch 就不会在使用 cudaImage 的同时访问内存）。


## 颜色转换


[`cudaConvertColor()`](https://github.com/dusty-nv/jetson-utils/tree/master/cuda/cudaColorspace.h) 函数使用 GPU 在图像格式和色彩空间之间进行转换。  例如，您可以从 RGB 转换为 BGR（反之亦然）、从 YUV 转换为 RGB、RGB 转换为灰度等。  您还可以更改数据类型和通道数（例如 RGB8 至 RGBA32F）。  有关可相互转换的不同格式的详细信息，请参阅上面的[图像格式](#image-formats) 部分。


[`cudaConvertColor()`](https://github.com/dusty-nv/jetson-utils/tree/master/cuda/cudaColorspace.h) 具有以下限制和不受支持的转换: 

* YUV 格式不支持 BGR/BGRA 或灰度（仅限 RGB/RGBA）

* YUV NV12、YUYV、YVYU 和 UYVY 只能转换为 RGB/RGBA（不能从）

* Bayer 格式只能转换为 RGB8 (`uchar3`) 和 RGBA8 (`uchar4`)


以下 Python/C++ 伪代码加载 RGB8 格式的图像，并将其转换为 RGBA32F（请注意，这纯粹是说明性的，因为图像可以直接作为 RGBA32F 加载）。  有关更全面的示例，请参阅 [`cuda-examples.py`](https://github.com/dusty-nv/jetson-utils/tree/master/python/examples/cuda-examples.py)。


#### Python


````蟒蛇
导入jetson_utils


# 加载输入图像（默认格式为rgb8）
imgInput = jetson_utils.loadImage('my_image.jpg', format='rgb8') # 默认格式为 'rgb8'，但也可以是 'rgba8', 'rgb32f', 'rgba32f'


# 将输出分配为rgba32f，与输入具有相同的宽度/高度
imgOutput = jetson_utils.cudaAllocMapped（宽度=imgInput.width，高度=imgInput.height，格式='rgba32f'）


# 从 rgb8 转换为 rgba32f（转换使用的格式取自图像胶囊）
jetson_utils.cudaConvertColor（imgInput，imgOutput）
````


#### C++


``c++
#include <jetson-utils/cudaColorspace.h>
#include <jetson-utils/cudaMappedMemory.h>
#include <jetson-utils/imageIO.h>


uchar3* imgInput = NULL;   // 输入为 rgb8 (uchar3)
float4* imgOutput = NULL;  // 输出为 rgba32f (float4)


整数宽度=0；
int 高度 = 0;


// 将图像加载为 rgb8 (uchar3)
if( !loadImage("my_image.jpg", &imgInput, &width, &height) )
	返回假；


// 将输出分配为 rgba32f (float4)，具有相同的宽度/高度
if( !cudaAllocMapped(&imgOutput, 宽度, 高度) )
	返回假；


//从rgb8转换为rgba32f
if( CUDA_FAILED(cudaConvertColor(imgInput, IMAGE_RGB8, imgOutput, IMAGE_RGBA32F, 宽度, 高度)) )
	返回假；	// 发生错误或不支持的转换
````


## 调整大小


[`cudaResize()`](https://github.com/dusty-nv/jetson-utils/tree/master/cuda/cudaResize.h) 函数使用 GPU 将图像重新缩放为不同的大小（下采样或上采样）。  以下 Python/C++ 伪代码加载图像，并按特定因子调整其大小（在示例中将采样缩减一半）。  有关更全面的示例，请参阅 [`cuda-examples.py`](https://github.com/dusty-nv/jetson-utils/tree/master/python/examples/cuda-examples.py)。


#### Python


````蟒蛇
导入jetson_utils


# 加载输入图像
imgInput = jetson_utils.loadImage('my_image.jpg')


# 分配输出，大小为输入的一半
imgOutput = jetson_utils.cudaAllocMapped(宽度=imgInput.width * 0.5, 
                                         高度=imgInput.高度 * 0.5, 
                                         格式=imgInput.format)


# 重新缩放图像（尺寸取自图像胶囊）
jetson_utils.cudaResize（imgInput，imgOutput）
````


#### C++


``c++
#include <jetson-utils/cudaResize.h>
#include <jetson-utils/cudaMappedMemory.h>
#include <jetson-utils/imageIO.h>


// 加载输入图像
uchar3* imgInput = NULL;


int 输入宽度 = 0;
int 输入高度 = 0;


if( !loadImage("my_image.jpg", &imgInput, &inputWidth, &inputHeight) )
	返回假；


// 分配输出图像，大小为输入图像的一半
uchar3* imgOutput = NULL;


int 输出宽度 = 输入宽度 * 0.5f；
int 输出高度 = 输入高度 * 0.5f；


if( !cudaAllocMapped(&imgOutput, 输出宽度, 输出高度) )
	返回假；


// 重新缩放图像
if（ CUDA_FAILED（cudaResize（imgInput，inputWidth，inputHeight，imgOutput，outputWidth，outputHeight）））
	返回假；
````


## 裁剪


[`cudaCrop()`](https://github.com/dusty-nv/jetson-utils/tree/master/cuda/cudaCrop.h) 函数使用 GPU 将图像裁剪到特定的感兴趣区域 (ROI)。  以下 Python/C++ 伪代码加载图像，并将其裁剪在图像的中半部分周围。  有关更全面的示例，请参阅 [`cuda-examples.py`](https://github.com/dusty-nv/jetson-utils/tree/master/python/examples/cuda-examples.py)。


请注意，ROI 矩形以 `(left, top, right, bottom)` 坐标形式提供。


#### Python


````蟒蛇
导入jetson_utils


# 加载输入图像
imgInput = jetson_utils.loadImage('my_image.jpg')


# 确定边框像素的数量（围绕中心裁剪一半）
作物系数 = 0.5
作物边框 = ((1.0 - 作物系数) * 0.5 * imgInput.width,
               (1.0 - 作物因子) * 0.5 * imgInput.height)


# 将 ROI 计算为（左、上、右、下）
crop_roi = (crop_border[0],crop_border[1],imgInput.width -crop_border[0],imgInput.height -crop_border[1])


# 分配输出图像，并具有裁剪后的尺寸
imgOutput = jetson_utils.cudaAllocMapped(宽度=imgInput.width *crop_factor,
                                         高度=imgInput.height *crop_factor，
                                         格式=imgInput.format)


# 将图像裁剪到 ROI
jetson_utils.cudaCrop（imgInput，imgOutput，crop_roi）
````


#### C++


``c++
#include <jetson-utils/cudaCrop.h>
#include <jetson-utils/cudaMappedMemory.h>
#include <jetson-utils/imageIO.h>


// 加载输入图像
uchar3* imgInput = NULL;


int 输入宽度 = 0;
int 输入高度 = 0;


if( !loadImage("my_image.jpg", &imgInput, &inputWidth, &inputHeight) )
	返回假；


// 确定边框像素的数量（围绕中心裁剪一半）
常量浮点作物因子 = 0.5
const int2crop_border = make_int2((1.0f-crop_factor) * 0.5f * inputWidth,
                                    (1.0f - 作物因子) * 0.5f * 输入高度);


// 将 ROI 计算为（左、上、右、下）
const int4crop_roi = make_int4(crop_border.x,crop_border.y,inputWidth -crop_border.x,inputHeight -crop_border.y);


// 分配输出图像，大小为输入图像的一半
uchar3* imgOutput = NULL;


if( !cudaAllocMapped(&imgOutput, inputWidth *crop_factor, inputHeight *cropFactor) )
	返回假；


// 裁剪图像
if（ CUDA_FAILED（cudaCrop（imgInput，imgOutput，crop_roi，inputWidth，inputHeight）））
	返回假；
````


## 正常化


[`cudaNormalize()`](https://github.com/dusty-nv/jetson-utils/tree/master/cuda/cudaNormalize.h) 函数使用 GPU 更改图像中像素强度的范围。  例如，将像素值介于 `[0,1]` 之间的图像转换为像素值介于 `[0,255]` 之间的图像。  像素值的另一个常见范围是在 `[-1,1]` 之间。


> **注意: ** jetson-inference 和 jetson-utils 中的所有其他函数都期望像素范围在 `[0,255]` 之间的图像，因此您通常不需要使用 `cudaNormalize()`，但如果您正在使用来自替代源或目标的数据，则可以使用它。


以下 Python/C++ 伪代码加载图像，并将其从 `[0,255]` 标准化为 `[0,1]`。


#### Python


````蟒蛇
导入jetson_utils


# 加载输入图像（其像素范围为0-255）
imgInput = jetson_utils.loadImage('my_image.jpg')


# 分配输出图像，其尺寸与输入相同
imgOutput = jetson_utils.cudaAllocMapped（宽度=imgInput.width，高度=imgInput.height，格式=imgInput.format）


# 将图像从 [0,255] 标准化为 [0,1]
jetson_utils.cudaNormalize(imgInput, (0,255), imgOutput, (0,1))
````


#### C++


``c++
#include <jetson-utils/cudaNormalize.h>
#include <jetson-utils/cudaMappedMemory.h>
#include <jetson-utils/imageIO.h>


uchar3* imgInput = NULL;
uchar3* imgOutput = NULL;


整数宽度=0；
int 高度 = 0;


// 加载输入图像（其像素范围为0-255）
if( !loadImage("my_image.jpg", &imgInput, &width, &height) )
	返回假；


// 分配输出图像，其尺寸与输入相同
if( !cudaAllocMapped(&imgOutput, 宽度, 高度) )
	返回假；


// 将图像从 [0,255] 标准化为 [0,1]
CUDA(cudaNormalize(imgInput, make_float2(0,255),
                   imgOutput, make_float2(0,1),
                   宽度、高度））；
````


## 覆盖


[`cudaOverlay()`](https://github.com/dusty-nv/jetson-utils/tree/master/cuda/cudaOverlay.h) 函数使用 GPU 将输入图像合成到特定位置的输出图像之上。  叠加操作通常按顺序调用以形成多个图像的合成。


以下 Python/C++ 伪代码加载两个图像，并将它们并排合成到输出图像中。


#### Python


````蟒蛇
导入jetson_utils


# 加载输入图像
imgInputA = jetson_utils.loadImage('my_image_a.jpg')
imgInputB = jetson_utils.loadImage('my_image_b.jpg')


# 分配输出图像，其尺寸可以并排适应两个输入
imgOutput = jetson_utils.cudaAllocMapped(宽度=imgInputA.width + imgInputB.width, 
                                         高度=max(imgInputA.高度,imgInputB.高度),
                                         格式=imgInputA.format)


# 合成两个图像（最后两个参数是输出图像中的 x,y 坐标）
jetson_utils.cudaOverlay(imgInputA, imgOutput, 0, 0)
jetson_utils.cudaOverlay(imgInputB, imgOutput, imgInputA.width, 0)
````


#### C++


``c++
#include <jetson-utils/cudaOverlay.h>
#include <jetson-utils/cudaMappedMemory.h>
#include <jetson-utils/imageIO.h>


#include <算法> // 对于 std::max()


uchar3* imgInputA = NULL;
uchar3* imgInputB = NULL;
uchar3* imgOutput = NULL;


int2 dimsA = make_int2(0,0);
int2 dimsB = make_int2(0,0);


// 加载输入图像
if( !loadImage("my_image_a.jpg", &imgInputA, &dimsA.x, &dimsA.y) )
	返回假；


if( !loadImage("my_image_b.jpg", &imgInputB, &dimsB.x, &dimsB.y) )
	返回假；


// 分配输出图像，其尺寸可以并排适应两个输入
const int2 dimsOutput = make_int2(dimsA.x + dimsB.x, std::max(dimsA.y, dimsB.y));


if( !cudaAllocMapped(&imgOutput, dimsOutput.x, dimsOutput.y) )
	返回假；


// 合成两个图像（最后两个参数是输出图像中的 x,y 坐标）
CUDA(cudaOverlay(imgInputA, dimsA, imgOutput, dimsOutput, 0, 0));
CUDA（cudaOverlay（imgInputB，dimsB，imgOutput，dimsOutput，dimsA.x，0））;
````


## 绘制形状


[`cudaDraw.h`](https://github.com/dusty-nv/jetson-utils/tree/master/cuda/cudaDraw.h) 定义了几个用于绘制基本形状的函数，包括圆形、直线和矩形。


下面是使用它们的简单 Python 和 C++ 伪代码 - 有关功能示例，请参阅 [`cuda-examples.py`](https://github.com/dusty-nv/jetson-utils/tree/master/python/examples/cuda-examples.py)。


#### Python


```` 蟒蛇
# 加载输入图像
输入 = jetson_utils.loadImage("my_image.jpg")


# cudaDrawCircle(输入，(cx，cy)，半径，(r，g，b，a)，输出=无)
jetson_utils.cudaDrawCircle(输入, (50,50), 25, (0,255,127,200))


# cudaDrawRect(输入，(左，上，右，下)，(r，g，b，a)，输出=无)
jetson_utils.cudaDrawRect(输入, (200,25,350,250), (255,127,0,200))


＃cudaDrawLine（输入，（x1，y1），（x2，y2），（r，g，b，a），line_width，输出=无）
jetson_utils.cudaDrawLine(输入, (25,150), (325,15), (255,0,200,200), 10)
````


> **注意: ** 如果未指定可选的 `output` 图像，则操作将在 `input` 图像上就地执行。


#### C++


````.cpp
#include <jetson-utils/cudaDraw.h>
#include <jetson-utils/imageIO.h>


uchar3* img = NULL;
整数宽度=0；
int 高度 = 0;


// 加载示例图像
if( !loadImage("my_image.jpg", &img, &width, &height) )
	返回假；	// 加载错误


// 参见 cudaDraw.h 的定义
CUDA(cudaDrawCircle(img, 宽度, 高度, 50, 50, 25, make_float4(0,255,127,200)));
CUDA(cudaDrawRect(img, 宽度, 高度, 200, 25, 350, 250, make_float4(255,127,0,200)));
CUDA(cudaDrawLine(img, 宽度, 高度, 25, 150, 325, 15, make_float4(255,0,200,200), 10));
````


##
<palign="right">下一个| <b><a href="https://github.com/dusty-nv/ros_deep_learning">Deep ROS/ROS2学习节点</a></b>
<br/>
返回 | <b><a href="aux-streaming-CN.md">相机流媒体和多媒体</a></p>
<palign="center"><sup>© 2016-2020 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>

