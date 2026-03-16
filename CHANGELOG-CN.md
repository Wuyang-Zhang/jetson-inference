<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">


# 变更日志


本文档将列出该项目的主要更新和新功能。


## 2023 年 5 月 5 日


* [WebRTC](docs/aux-streaming-CN.md#webrtc) 支持和 [WebApp 框架](README-CN.md#webapp-frameworks) 教程: 

   * [WebRTC 服务器](docs/webrtc-server-CN.md)

   * [HTML / JavaScript](docs/webrtc-html-CN.md)

   * [烧瓶 + 休息](docs/webrtc-flask-CN.md)

   * [Plotly 仪表板](docs/webrtc-dash-CN.md)

   * [识别器（互动训练）](docs/webrtc-recognizer-CN.md)

* 支持 `detectNet` 中的[TAO 检测模型](docs/detectnet-tao-CN.md)

* 添加了 [`actionNet`](docs/actionnet-CN.md)（动作/活动识别）

* 添加了 [`backgroundNet`](docs/backgroundnet-CN.md)（前景/背景分割/删除）

* 为 detectorNet 添加了 [`objectTracker`](c/tracking) - [IoU 对象跟踪](docs/detectnet-tracking-CN.md)

* [图像标记和多标签分类](docs/imagenet-tagging-CN.md)（支持 imageNet 中的 [`topK`](https://github.com/dusty-nv/jetson-inference/blob/b50bf1d5eefed73acda5c963513e0d8c79d18be3/c/imageNet.h#L201)）

* imageNet 中的[分类结果的时间平滑](https://github.com/dusty-nv/jetson-inference/blob/b50bf1d5eefed73acda5c963513e0d8c79d18be3/c/imageNet.h#L271)

* 自动模型下载器 => [`data/networks/models.json`](data/networks/models.json)

* 构建 TensorRT 时序缓存以快速加载更新的模型（或共享层配置的模型）

* Python [cudaImage](https://github.com/dusty-nv/jetson-inference/blob/master/docs/aux-image.md#image-capsules-in-python) 与其他库的零拷贝互操作性: 

   * [`__cuda_array_interface__`](docs/aux-image-CN.md#cuda-array-interface)（Numba、PyTorch、CuPy、PyCUDA、VPI 和[其他](https://numba.readthedocs.io/en/stable/cuda/cuda_array_interface.html#interoperability)）

   * [`__array__`](docs/aux-image-CN.md#accessing-as-a-numpy-array) 接口 ([Numpy](https://numpy.org/doc/stable/reference/arrays.interface.html))

* 使用 [`train_ssd.py --resolution=N`](https://github.com/dusty-nv/pytorch-ssd/blob/86155c0c410e0959df0184b24af6a8f59f49fbe5/train_ssd.py#L49) 训练更高分辨率的检测模型

* 使用 [`train_ssd.py --validate-mean-ap`](https://github.com/dusty-nv/pytorch-ssd/blob/86155c0c410e0959df0184b24af6a8f59f49fbe5/train_ssd.py#L98) 计算每类平均精度 (mAP)

* 张量板登录 [`train.py`](https://github.com/dusty-nv/pytorch-classification/blob/819b105087c397c23cd81fd9446b5f0a0213db94/train.py#L95) / [`train_ssd.py`](https://github.com/dusty-nv/pytorch-ssd/blob/86155c0c410e0959df0184b24af6a8f59f49fbe5/train_ssd.py#L114)

* 添加了[RTSP服务器](docs/aux-streaming-CN.md#rtsp)视频输出

* 向 [`videoSource.Capture()`](https://github.com/dusty-nv/jetson-utils/blob/0bcb19b498326eb866a80d7d13388b2e59bc9dfd/video/videoSource.h#L235) 添加了可选的超时状态代码

* 除了主 I/O 流之外，还添加了 [`--input-save`](docs/aux-streaming-CN.md#input-options) 和 [`--output-save`](docs/aux-streaming-CN.md#output-options)，用于将视频转储到磁盘

* 添加了 [`ros_deep_learning`](https://github.com/dusty-nv/ros_deep_learning) 包作为子模块并添加到容器构建中

* 为 [Docker 容器](docs/aux-docker-CN.md#x86-support) 添加了 x86_64 + dGPU 支持和 WSL2

* 使用 [`test-models.py`](tools/test-models.py) 和 [`test-cuda.sh`](https://github.com/dusty-nv/jetson-utils/blob/master/python/examples/test-cuda.sh) 进行自动化测试



## 2022 年 4 月 8 日


* 添加了对 JetPack 5.0 和 [Jetson AGX Orin](https://developer.nvidia.com/embedded/jetson-agx-orin-developer-kit) 的支持

* 在 JetPack 5.0 及更高版本上有条件地使用基于 NVIDIA V4L2 的硬件编解码器

* 小错误修复和改进


## 2021 年 8 月 3 日


* 添加了带有预训练模型的 [Pose Estimation with PoseNet](docs/posenet-CN.md)

* 添加了带有预训练模型的[单目深度与 DepthNet](docs/depthnet-CN.md)

* 添加了对 [`cudaMemcpy()` 来自 Python](docs/aux-image-CN.md#copying-images) 的支持

* 添加了对[使用 CUDA 绘制 2D 形状](docs/aux-image-CN.md#drawing-shapes) 的支持


## 2020 年 8 月 31 日


* 添加了对[在 Docker 容器中运行](docs/aux-docker-CN.md) 的初始支持

* 更改了 OpenGL 行为以在第一帧上显示窗口

* 小错误修复和改进


## 2020 年 7 月 15 日


> **注意: ** 此更新中的 API 更改旨在向后兼容，因此以前的代码仍应运行。


* [重新训练 SSD-Mobilenet](docs/pytorch-ssd-CN.md) 使用 PyTorch 进行目标检测教程

* 支持[目标检测数据集的集合](docs/pytorch-collect-detection-CN.md) 和 `camera-capture` 工具中的边界框标记

* 用于支持多种类型视频流的 C++/Python 的 [`videoSource`](docs/aux-streaming-CN.md#source-code) 和 [`videoOutput`](docs/aux-streaming-CN.md#source-code) API: 

   * [MIPI CSI 相机](docs/aux-streaming-CN.md#mipi-csi-cameras)

   * [V4L2 相机](docs/aux-streaming-CN.md#v4l2-cameras)

   * [RTP](docs/aux-streaming-CN.md#rtp) / [RTSP](docs/aux-streaming-CN.md#rtsp)

   * [视频](docs/aux-streaming-CN.md#video-files) 和 [图片](docs/aux-streaming-CN.md#image-files)

   * [图像序列](docs/aux-streaming-CN.md#image-files)

   * [OpenGL 窗口](docs/aux-streaming-CN.md#output-streams)

* 统一 `-console` 和 `-camera` 样本来处理图像和视频流

   * [`imagenet.cpp`](examples/imagenet/imagenet.cpp) / [`imagenet.py`](python/examples/imagenet.py)

   * [`detectnet.cpp`](examples/detectnet/detectnet.cpp) / [`detectnet.py`](python/examples/detectnet.py)

   * [`segnet.cpp`](examples/segnet/segnet.cpp) / [`segnet.py`](python/examples/segnet.py)

* 支持 `uchar3/uchar4/float3/float4` 图像（默认值为 `uchar3`，而不是 `float4`）

* 将不透明的 Python 内存胶囊替换为 [`jetson.utils.cudaImage`](docs/aux-image-CN.md#image-capsules-in-python) 对象

   * 有关详细信息，请参阅[Python 中的图像胶囊](docs/aux-image-CN.md#image-capsules-in-python)

   * 图像现在可以从 Python 进行下标/索引，以直接访问像素数据集

   * Numpy ndarray 转换现在支持 `uchar3/uchar4/float3/float4` 格式

* [`cudaConvertColor()`](https://github.com/dusty-nv/jetson-utils/blob/a587c20ad95d71efd47f9c91e3fbf703ad48644d/cuda/cudaColorspace.h#L31) 自动色彩空间转换功能（RGB、BGR、YUV、Bayer、灰度等）

* `cudaResize()`、`cudaCrop()`、`cudaNormalize()`、`cudaOverlay()` 的 Python CUDA 绑定

   * 有关使用这些的示例，请参阅[使用 CUDA 进行图像处理](docs/aux-image-CN.md) 和 [`cuda-examples.py`](https://github.com/dusty-nv/jetson-utils/blob/master/python/examples/cuda-examples.py)

* 由于 Python 2.7 现已 EOL，因此默认转换为使用 Python3

* DIGITS 教程现已标记为已弃用（由 PyTorch 迁移学习教程取代）

* 现在可以从命令行控制/禁用日志记录（例如 `--log-level=verbose`）


感谢论坛和 GitHub 中提前帮助测试这些更新的所有人！


## 2019 年 10 月 3 日


* 添加了新的预训练 FCN-ResNet18 语义分割模型: 


|数据集 |分辨率| CLI 参数 |准确度|杰特森纳米|杰特森·泽维尔 |
|:------------:|:----------:|--------------|:--------:|:------------:|:-------------:|
| [城市景观](https://www.cityscapes-dataset.com/) | 512x256 | `fcn-resnet18-cityscapes-512x256` | 83.3% | 48 帧/秒 | 480 帧/秒 |
| [城市景观](https://www.cityscapes-dataset.com/) | 1024x512 | `fcn-resnet18-cityscapes-1024x512` | 87.3% | 12 帧/秒 | 175 帧/秒 |
| [城市景观](https://www.cityscapes-dataset.com/) | 2048x1024 | 2048x1024 `fcn-resnet18-cityscapes-2048x1024` | 89.6% | 3 FPS | 47 帧/秒 |
| [DeepScene](http://deepscene.cs.uni-freiburg.de/) | 576x320 | `fcn-resnet18-deepscene-576x320` | 96.4% | 26 帧/秒 | 360 FPS |
| [DeepScene](http://deepscene.cs.uni-freiburg.de/) | 864x480 | 864x480 `fcn-resnet18-deepscene-864x480` | 96.9% | 14 帧/秒 | 190 帧/秒 |
| [多人](https://lv-mhp.github.io/) | 512x320 | `fcn-resnet18-mhp-512x320` | 86.5% | 34 帧/秒 | 370 帧/秒 |
| [多人](https://lv-mhp.github.io/) | 640x360 | `fcn-resnet18-mhp-512x320` | 87.1% | 23 帧/秒 | 325 帧/秒 |
| [帕斯卡 VOC](http://host.robots.ox.ac.uk/pascal/VOC/) | 320x320 | 320x320 `fcn-resnet18-voc-320x320` | 85.9% | 45 帧/秒 | 508 帧 |
| [帕斯卡 VOC](http://host.robots.ox.ac.uk/pascal/VOC/) | 512x320 | `fcn-resnet18-voc-512x320` | 88.5% | 34 帧/秒 | 375 帧/秒 |
| [SUN RGB-D](http://rgbd.cs.princeton.edu/) | 512x400 | 512x400 `fcn-resnet18-sun-512x400` | 64.3% | 28 帧/秒 | 340 帧/秒 |
| [SUN RGB-D](http://rgbd.cs.princeton.edu/) | 640x512 | 640x512 `fcn-resnet18-sun-640x512` | 65.1% | 17 帧/秒 | 224 帧/秒 |


## 2019 年 7 月 19 日


* 对 imageNet、DetectNet 和相机/显示实用程序的 Python API 支持</li>

* 用于处理静态图像和实时摄像头流的 Python 示例</li>

* 支持与 CUDA 中的 numpy ndarray 交互</li>

* 使用 PyTorch 对 ResNet-18 模型进行板载重新训练</li>

* 示例数据集: 800MB Cat/Dog 和 1.5GB PlantCLEF</li>

* 基于摄像头的工具，用于收集和标记自定义数据集</li>

* 用于选择/下载预训练模型的文本 UI 工具</li>

* 新的预训练图像分类模型（在 1000 类 ImageNet ILSVRC 上）

   * ResNet-18、ResNet-50、ResNet-101、ResNet-152</li>

   * VGG-16、VGG-19</li>

   * Inception-v4</li>

* 新的预训练目标检测模型（在 90 级 MS-COCO 上）

   * SSD-Mobilenet-v1</li>

   * SSD-Mobilenet-v2</li>

   * SSD-Inception-v2</li>

* C++ 和 Python 的 API 参考文档</li>

   * 所有示例的命令行使用信息，使用 --help 运行</li>

   * 网络分析器时间的输出，包括预处理/后处理</li>

   * 使用系统 TTF 字体改进字体光栅化</li>



##
<palign="center"><sup>© 2016-2020 NVIDIA | </sup><a href="README-CN.md#hello-ai-world"><sup>目录</sup></a></p>