<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">


# 部署深度学习
欢迎使用我们针对 **[NVIDIA Jetson](https://developer.nvidia.com/embedded-computing)** 设备的推理和实时视觉 [DNN 库](#api-reference) 的指导指南。  该项目使用 **[TensorRT](https://developer.nvidia.com/tensorrt)** 在 GPU 上通过 C++ 或 Python 运行优化的网络，并使用 PyTorch 来训练模型。


支持的 DNN 视觉基元包括用于图像分类的 [`imageNet`](docs/imagenet-console-2-CN.md)、用于目标检测的 [`detectNet`](docs/detectnet-console-2-CN.md)、用于语义分割的 [`segNet`](docs/segnet-console-2-CN.md)、用于姿势估计的 [`poseNet`](docs/posenet-CN.md) 和用于动作识别的 [`actionNet`](docs/actionnet-CN.md)。  提供了来自实时摄像机源的流式传输、使用 WebRTC 制作 Web 应用程序以及对 ROS/ROS2 的支持的示例。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-primitives.jpg">


按照 **[Hello AI World](#hello-ai-world)** 教程在 Jetson 上运行推理和迁移学习，包括收集您自己的数据集、使用 PyTorch 训练您自己的模型以及使用 TensorRT 部署它们。


### 目录


* [你好人工智能世界](#hello-ai-world)

* [Jetson 人工智能实验室](#jetson-ai-lab)

* [视频演练](#video-walkthroughs)

* [API 参考](#api-reference)

* [代码示例](#code-examples)

* [预训练模型](#pre-trained-models)

* [系统要求](#recommended-system-requirements)

* [更改日志](CHANGELOG-CN.md)


> > &nbsp; Orin 设备现已支持 JetPack 6 ([developer.nvidia.com/jetpack](https://developer.nvidia.com/embedded/jetpack)) <br/>

> > &nbsp;查看 [Jetson AI Lab](https://www.jetson-ai-lab.com/) 上的生成式 AI 和 LLM 教程！ <br/>

> > &nbsp;请参阅[更改日志](CHANGELOG-CN.md) 了解最新更新和新功能。 <br/>


## 你好人工智能世界


Hello AI World 可以完全在 Jetson 上运行，包括使用 TensorRT 进行实时推理和使用 PyTorch 进行迁移学习。  有关安装说明，请参阅[系统设置](#system-setup)。  建议先从[推理](#inference) 部分开始熟悉概念，然后再深入[训练](#training) 您自己的模型。


#### 系统设置


* [使用 JetPack 设置 Jetson](docs/jetpack-setup-2-CN.md)

* [运行 Docker 容器](docs/aux-docker-CN.md)

* [从源代码构建项目](docs/building-repo-2-CN.md)


#### 推理


* [图像分类](docs/imagenet-console-2-CN.md)

	* [在 Jetson 上使用 ImageNet 程序](docs/imagenet-console-2-CN.md)

	* [编写自己的图像识别程序（Python）](docs/imagenet-example-python-2-CN.md)

	* [编写自己的图像识别程序（C++）](docs/imagenet-example-2-CN.md)

	* [运行实时摄像头识别演示](docs/imagenet-camera-2-CN.md)

	* [图像标记的多标签分类](docs/imagenet-tagging-CN.md)

* [物体检测](docs/detectnet-console-2-CN.md)

	* [从图像中检测对象](docs/detectnet-console-2-CN.md#detecting-objects-from-the-command-line)

	* [运行实时摄像头检测演示](docs/detectnet-camera-2-CN.md)

	* [编写您自己的目标检测程序](docs/detectnet-example-2-CN.md)

	* [使用 TAO 检测模型](docs/detectnet-tao-CN.md)

	* [视频上的对象跟踪](docs/detectnet-tracking-CN.md)

* [语义分割](docs/segnet-console-2-CN.md)

	* [从命令行分割图像](docs/segnet-console-2-CN.md#segmenting-images-from-the-command-line)

	* [运行实时摄像头分割演示](docs/segnet-camera-2-CN.md)

* [姿势估计](docs/posenet-CN.md)

* [动作识别](docs/actionnet-CN.md)

* [背景去除](docs/backgroundnet-CN.md)

* [单眼深度](docs/depthnet-CN.md)


#### 训练


* [使用 PyTorch 进行迁移学习](docs/pytorch-transfer-learning-CN.md)

* 分类/识别 (ResNet-18)

	* [猫/狗数据集的重新训练](docs/pytorch-cat-dog-CN.md)

	* [PlantCLEF 数据集的重新训练](docs/pytorch-plants-CN.md)

	* [收集您自己的分类数据集](docs/pytorch-collect-CN.md)

* 物体检测（SSD-Mobilenet）

	* [重新训练SSD-Mobilenet](docs/pytorch-ssd-CN.md)

	* [收集您自己的检测数据集](docs/pytorch-collect-detection-CN.md)


#### 网络应用程序框架


* [WebRTC 服务器](docs/webrtc-server-CN.md)

* [HTML / JavaScript](docs/webrtc-html-CN.md)

* [烧瓶 + 休息](docs/webrtc-flask-CN.md)

* [Plotly 仪表板](docs/webrtc-dash-CN.md)

* [识别器（互动训练）](docs/webrtc-recognizer-CN.md)


#### 附录


* [相机流媒体和多媒体](docs/aux-streaming-CN.md)

* [使用 CUDA 进行图像处理](docs/aux-image-CN.md)

* [ROS/ROS2 的 DNN 推理节点](https://github.com/dusty-nv/ros_deep_learning)


## Jetson 人工智能实验室


<a href="https://www.jetson-ai-lab.com"><imgalign="右"宽度="200"高度="200"src="https://nvidia-ai-iot.github.io/jetson-generative-ai-playground/images/JON_Gen-AI-panels.png"></a>


[**Jetson AI Lab**](https://www.jetson-ai-lab.com) 提供有关在 Orin（有时还包括 Xavier）上运行的 LLM、视觉 Transformers (ViT) 和视觉语言模型 (VLM) 的附加教程。  查看其中一些: 


<a href="https://www.jetson-ai-lab.com/tutorial_nanoowl.html"><img src="https://github.com/NVIDIA-AI-IOT/nanoowl/raw/main/assets/jetson_person_2x.gif"></a>

> [NanoOWL - 开放词汇目标检测 ViT](https://www.jetson-ai-lab.com/tutorial_nanoowl.html)（容器: [`nanoowl`](/packages/vit/nanoowl)）


<a href="https://youtu.be/X-OXxPiUTuU"><img 宽度="600px" src="https://raw.githubusercontent.com/dusty-nv/jetson-containers/docs/docs/images/live_llava.gif"></a>

> [Jetson AGX Orin 上的实时 Llava](https://youtu.be/X-OXxPiUTuU)（容器: [`local_llm`](/packages/llm/local_llm#live-llava)）


<a href="https://youtu.be/dRmAGGuupuE"><img 宽度="600px" src="https://raw.githubusercontent.com/dusty-nv/jetson-containers/docs/docs/images/live_llava_bear.jpg"></a>

> [Live Llava 2.0 - VILA + Jetson Orin 上的多模式 NanoDB](https://youtu.be/X-OXxPiUTuU)（容器: [`local_llm`](/packages/llm/local_llm#live-llava)）


<a href="https://youtu.be/ayqKpQNd1Jw"><img src="https://raw.githubusercontent.com/dusty-nv/jetson-containers/docs/docs/images/nanodb_horse.gif"></a>

> [NVIDIA Jetson 上的实时多模式 VectorDB](https://www.youtube.com/watch?v=wzLHAgDxMjQ)（容器: [`nanodb`](/packages/vectordb/nanodb)）


## 视频演练


以下是为 [Jetson AI 认证](https://developer.nvidia.com/embedded/learn/jetson-ai-certification-programs) 课程录制的 Hello AI World 截屏: 


|描述 |视频 |
|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| <a href="https://www.youtube.com/watch?v=QXIwdsyK7Rw&list=PL5B692fm6--uQRRDTPsJDp4o0xbzkoyf8&index=9" target="_blank">**Hello AI World 设置**</a><br/>在 Jetson Nano 上下载并运行 Hello AI World 容器，测试您的相机源，并了解如何通过 RTP 通过网络进行流式传输。 | <a href="https://www.youtube.com/watch?v=QXIwdsyK7Rw&list=PL5B692fm6--uQRRDTPsJDp4o0xbzkoyf8&index=9" target="_blank"><img src=https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/thumbnail_setup.jpg width="750"></a> |
| <a href="https://www.youtube.com/watch?v=QatH8iF0Efk&list=PL5B692fm6--uQRRDTPsJDp4o0xbzkoyf8&index=10" target="_blank">**图像分类推理**</a><br/>使用 Jetson Nano 和深度学习编写您自己的图像分类 Python 程序，然后在实时摄像头流上进行实时分类实验。 | <a href="https://www.youtube.com/watch?v=QatH8iF0Efk&list=PL5B692fm6--uQRRDTPsJDp4o0xbzkoyf8&index=10" target="_blank"><img src=https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/thumbnail_imagenet.jpg width="750"></a> |
| <a href="https://www.youtube.com/watch?v=sN6aT9TpltU&list=PL5B692fm6--uQRRDTPsJDp4o0xbzkoyf8&index=11" target="_blank">**训练图像分类模型**</a><br/>了解如何使用 Jetson Nano 上的 PyTorch 训练图像分类模型，并收集您自己的分类数据集来创建自定义模型。 | <a href="https://www.youtube.com/watch?v=sN6aT9TpltU&list=PL5B692fm6--uQRRDTPsJDp4o0xbzkoyf8&index=11" target="_blank"><img src=https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/thumbnail_imagenet_training.jpg width="750"></a> |
| <a href="https://www.youtube.com/watch?v=obt60r8ZeB0&list=PL5B692fm6--uQRRDTPsJDp4o0xbzkoyf8&index=12" target="_blank">**目标检测推理**</a><br/>使用 Jetson Nano 和深度学习编写您自己的 Python 程序以进行目标检测，然后在实时摄像头流上进行实时检测实验。 | <a href="https://www.youtube.com/watch?v=obt60r8ZeB0&list=PL5B692fm6--uQRRDTPsJDp4o0xbzkoyf8&index=12" target="_blank"><img src=https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/thumbnail_detectnet.jpg width="750"></a> |
| <a href="https://www.youtube.com/watch?v=2XMkPW_sIGg&list=PL5B692fm6--uQRRDTPsJDp4o0xbzkoyf8&index=13" target="_blank">**训练目标检测模型**</a><br/>了解如何使用 Jetson Nano 上的 PyTorch 训练目标检测模型，并收集您自己的检测数据集以创建自定义模型。 | <a href="https://www.youtube.com/watch?v=2XMkPW_sIGg&list=PL5B692fm6--uQRRDTPsJDp4o0xbzkoyf8&index=13" target="_blank"><img src=https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/thumbnail_detectnet_training.jpg width="750"></a> |
| <a href="https://www.youtube.com/watch?v=AQhkMLaB_fY&list=PL5B692fm6--uQRRDTPsJDp4o0xbzkoyf8&index=14" target="_blank">**语义分割**</a><br/>在 Jetson Nano 上试验全卷积语义分割网络，并在实时摄像头流上运行实时分割。 | <a href="https://www.youtube.com/watch?v=AQhkMLaB_fY&list=PL5B692fm6--uQRRDTPsJDp4o0xbzkoyf8&index=14" target="_blank"><img src=https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/thumbnail_segnet.jpg width="750"></a> |


## API参考


以下是存储库中 [C++](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/index.html) 和 [Python](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.html) 库的参考文档的链接: 


#### 杰森推理


|                    | [C++](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/group__deepVision.html) | [Python](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html) |
|--------------------|--------------|--------------|
|图像识别 | [`imageNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/group__imageNet.html#classimageNet) | [`imageNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#imageNet) |
|物体检测| [`detectNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/group__detectNet.html#classdetectNet) | [`detectNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#detectNet)
|细分| [`segNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/group__segNet.html#classsegNet) | [`segNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#segNet) |
|姿势估计| [`poseNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/group__poseNet.html#classposeNet) | [`poseNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#poseNet) |
|动作识别 | [`actionNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/group__actionNet.html#classactionNet) | [`actionNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#actionNet) |
|背景去除| [`backgroundNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/group__backgroundNet.html#classbackgroundNet) | [`actionNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#backgroundNet) |
|单眼深度| [`depthNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/group__depthNet.html#classdepthNet) | [`depthNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#depthNet) |


#### jetson-utils 工具


* [C++](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/group__util.html)

* [Python](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.utils.html)


这些库可以通过链接到 `libjetson-inference` 和 `libjetson-utils` 在外部项目中使用。


## 代码示例


Hello AI World 教程的以下步骤涵盖了使用该库的介绍性代码演练: 


* [编写自己的图像识别程序（Python）](docs/imagenet-example-python-2-CN.md)

* [编写自己的图像识别程序（C++）](docs/imagenet-example-2-CN.md)


可以在此处找到用于在图像和实时摄像头流上运行网络的其他 C++ 和 Python 示例: 


|                   | C++ |蟒蛇 |
|--------------------|---------------------|---------------------|
|图像识别 | [`imagenet.cpp`](examples/imagenet/imagenet.cpp) | [`imagenet.py`](python/examples/imagenet.py) |
| &nbsp;&nbsp;&nbsp;物体检测 | [`detectnet.cpp`](examples/detectnet/detectnet.cpp) | [`detectnet.py`](python/examples/detectnet.py) |
| &nbsp;&nbsp;&nbsp;细分 | [`segnet.cpp`](examples/segnet/segnet.cpp) | [`segnet.py`](python/examples/segnet.py) |
| &nbsp;&nbsp;&nbsp;姿势估计 | [`posenet.cpp`](examples/posenet/posenet.cpp) | [`posenet.py`](python/examples/posenet.py) |
| &nbsp;&nbsp;&nbsp;动作识别 | [`actionnet.cpp`](examples/actionnet/actionnet.cpp) | [`actionnet.py`](python/examples/actionnet.py) |
| &nbsp;&nbsp;&nbsp;背景去除 | [`backgroundnet.cpp`](examples/backgroundnet/backgroundnet.cpp) | [`backgroundnet.py`](python/examples/backgroundnet.py) |
| &nbsp;&nbsp;&nbsp;单眼景深 | [`depthnet.cpp`](examples/depthnet/segnet.cpp) | [`depthnet.py`](python/examples/depthnet.py) |


> **注意**: 请参阅 [Array Interfaces](docs/aux-image-CN.md#array-interfaces) 部分，了解如何将内存与其他 Python 库（如 Numpy、PyTorch 等）一起使用


这些示例将在[从源代码构建项目](docs/building-repo-2-CN.md) 时自动编译，并且除了用户提供的自定义模型之外，还能够运行下面列出的预训练模型。  使用 `--help` 启动每个示例以获取使用信息。


## 预训练模型


该项目附带了许多可供使用的预训练模型，并将自动下载: 


#### 图像识别


|网络| CLI 参数 |网络类型枚举 |
| --------------|----------------|--------------------|
|亚历克斯网| `alexnet` | `ALEXNET` |
|谷歌网络 | `googlenet` | `GOOGLENET` |
| GoogleNet-12 | `googlenet-12` | `GOOGLENET_12` |
| ResNet-18 | `resnet-18` | `RESNET_18` |
| ResNet-50|ResNet-50 `resnet-50` | `RESNET_50` |
| ResNet-101 | ResNet-101 `resnet-101` | `RESNET_101` |
| ResNet-152 | ResNet-152 `resnet-152` | `RESNET_152` |
| VGG-16 | `vgg-16` | `VGG-16` |
| VGG-19 | `vgg-19` | `VGG-19` |
| Inception-v4 | `inception-v4` | `INCEPTION_V4` |


#### 物体检测


|型号| CLI 参数 |网络类型枚举 |对象类 |
| ------------------------|--------------------|--------------------|----------------------|
| SSD-Mobilenet-v1 | `ssd-mobilenet-v1` | `SSD_MOBILENET_V1` | 91 ([COCO 类](../data/networks/ssd_coco_labels.txt)) |
| SSD-Mobilenet-v2 | `ssd-mobilenet-v2` | `SSD_MOBILENET_V2` | 91 ([COCO 类](../data/networks/ssd_coco_labels.txt)) |
| SSD-Inception-v2 | `ssd-inception-v2` | `SSD_INCEPTION_V2` | 91 ([COCO 类](../data/networks/ssd_coco_labels.txt)) |
|陶人民网| `peoplenet` | `PEOPLENET` |人、包、脸 |
| TAO 人民网（已删减）| `peoplenet-pruned` | `PEOPLENET_PRUNED` |人、包、脸 |
| TAO 行车记录仪 | `dashcamnet` | `DASHCAMNET` |人、车、自行车、标志|
| TAO TrafficCamNet | `trafficcamnet` | `TRAFFICCAMNET` |人、车、自行车、标志| 
| TAO 人脸检测 | `facedetect` | `FACEDETECT` |脸 |


<详情>
<summary>遗留检测模型</summary>


|型号| CLI 参数 |网络类型枚举 |对象类 |
| ------------------------|--------------------|--------------------|----------------------|
| DetectNet-COCO-Dog | `coco-dog` | `COCO_DOG` |狗 |
| DetectNet-COCO-瓶子| `coco-bottle` | `COCO_BOTTLE` |瓶子|
| DetectNet-COCO-主席 | `coco-chair` | `COCO_CHAIR` |椅子 |
| DetectNet-COCO-飞机 | `coco-airplane` | `COCO_AIRPLANE` |飞机 |
| ped-100 | 佩德-100 `pednet` | `PEDNET` |行人|
| 500 倍 | `multiped` | `PEDNET_MULTI` |行人、行李|
| Facenet-120 | Facenet-120 | `facenet` | `FACENET` |面孔 |


</详情>


#### 语义分割


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
| [SUN RGB-D](http://rgbd.cs.princeton.edu/) | 640x512 | `fcn-resnet18-sun-640x512` | 65.1% | 17 帧/秒 | 224 帧/秒 |


* 如果 CLI 参数中省略分辨率，则加载最低分辨率模型

* 准确度表示模型验证数据集中的像素分类准确度

* 使用 JetPack 4.2.1、`nvpmodel 0` (MAX-N) 测量 GPU FP16 模式的性能


<详情>
<summary>传统细分模型</summary>


|网络| CLI 参数 |网络类型枚举 |课程 |
| ------------------------|---------------------------------|---------------------------------|----------|
|城市景观 (2048x2048) | `fcn-alexnet-cityscapes-hd` | `FCN_ALEXNET_CITYSCAPES_HD` |    21 | 21
|城市景观 (1024x1024) | `fcn-alexnet-cityscapes-sd` | `FCN_ALEXNET_CITYSCAPES_SD` |    21 | 21
|帕斯卡 VOC (500x356) | `fcn-alexnet-pascal-voc` | `FCN_ALEXNET_PASCAL_VOC` |    21 | 21
|辛西娅 (CVPR16) | `fcn-alexnet-synthia-cvpr` | `FCN_ALEXNET_SYNTHIA_CVPR` |    14 | 14
| Synthia（夏季-高清）| `fcn-alexnet-synthia-summer-hd` | `FCN_ALEXNET_SYNTHIA_SUMMER_HD` |    14 | 14
| Synthia（夏季-SD）| `fcn-alexnet-synthia-summer-sd` | `FCN_ALEXNET_SYNTHIA_SUMMER_SD` |    14 | 14
|航拍-FPV (1280x720) | `fcn-alexnet-aerial-fpv-720p` | `FCN_ALEXNET_AERIAL_FPV_720p` |     2 |


</详情>


#### 姿势估计


|型号| CLI 参数 |网络类型枚举 |要点 |
| ------------------------|--------------------|--------------------|------------------------|
| Pose-ResNet18-Body | 姿势-ResNet18-Body `resnet18-body` | `RESNET18_BODY` | 18 | 18
| Pose-ResNet18-手 | `resnet18-hand` | `RESNET18_HAND` | 21 | 21
| Pose-DenseNet121-身体 | `densenet121-body` | `DENSENET121_BODY` | 18 | 18


#### 动作识别


|型号| CLI 参数 |课程 |
| ------------------------|--------------|---------|
| Action-ResNet18-动力学 | `resnet18` |  1040 | 1040
| Action-ResNet34-动力学 | `resnet34` |  1040 | 1040


## 推荐系统要求


* 带有 JetPack 4.2 或更高版本的 Jetson Nano 开发人员套件 (Ubuntu 18.04 aarch64)。

* Jetson Nano 2GB 开发者套件，带有 JetPack 4.4.1 或更高版本 (Ubuntu 18.04 aarch64)。

* 带有 JetPack 5.0 或更高版本的 Jetson Orin Nano 开发人员套件 (Ubuntu 20.04 aarch64)。

* Jetson Xavier NX 开发者套件，带有 JetPack 4.4 或更高版本 (Ubuntu 18.04 aarch64)。

* 带有 JetPack 4.0 或更高版本的 Jetson AGX Xavier 开发人员套件 (Ubuntu 18.04 aarch64)。

* 带有 JetPack 5.0 或更高版本的 Jetson AGX Orin 开发人员套件 (Ubuntu 20.04 aarch64)。

* Jetson TX2 开发者套件，带有 JetPack 3.0 或更高版本 (Ubuntu 16.04 aarch64)。

* Jetson TX1 开发者套件，带有 JetPack 2.3 或更高版本 (Ubuntu 16.04 aarch64)。


本教程的[使用 PyTorch 进行迁移学习](#training) 部分从在 Jetson 上运行 PyTorch 来训练 DNN 的角度出发，但是相同的 PyTorch 代码可以在具有 NVIDIA 独立 GPU 的 PC、服务器或云实例上使用，以实现更快的训练。



## 额外资源


在该区域中，列出了深度学习的链接和资源: 


* [ros_deep_learning](http://www.github.com/dusty-nv/ros_deep_learning) - TensorRT 推理 ROS 节点

* [NVIDIA AI IoT](https://github.com/NVIDIA-AI-IOT) - NVIDIA Jetson GitHub 存储库

* [Jetson eLinux 维基](https://www.eLinux.org/Jetson) - Jetson eLinux 维基



## 两天演示（数字）


> **注意: ** 下面的 DIGITS/Caffe 教程已被弃用。  建议遵循 Hello AI World 中的[使用 PyTorch 进行迁移学习](#training) 教程。


<详情>
<summary>展开此部分可查看原始 DIGITS 教程（已弃用）</summary>
<br/>
DIGITS 教程包括在云或 PC 中训练 DNN，以及使用 TensorRT 在 Jetson 上进行推理，可能需要大约两天或更长时间，具体取决于系统设置、下载数据集以及 GPU 的训练速度。


* [数字工作流程](docs/digits-workflow-CN.md)

* [DIGITS 系统设置](docs/digits-setup-CN.md)

* [使用 JetPack 设置 Jetson](docs/jetpack-setup-CN.md)

* [从源代码构建项目](docs/building-repo-CN.md)

* [使用 ImageNet 对图像进行分类](docs/imagenet-console-CN.md)

	* [在Jetson上使用控制台程序](docs/imagenet-console-CN.md#using-the-console-program-on-jetson)

	* [编写您自己的图像识别程序](docs/imagenet-example-CN.md)

	* [运行实时摄像头识别演示](docs/imagenet-camera-CN.md)

	* [用数字重新训练网络](docs/imagenet-training-CN.md)

	* [下载图像识别数据集](docs/imagenet-training-CN.md#downloading-image-recognition-dataset)

	* [自定义对象类](docs/imagenet-training-CN.md#customizing-the-object-classes)

	* [将分类数据集导入 DIGITS](docs/imagenet-training-CN.md#importing-classification-dataset-into-digits)

	* [使用 DIGITS 创建图像分类模型](docs/imagenet-training-CN.md#creating-image-classification-model-with-digits)

	* [测试 DIGITS 中的分类模型](docs/imagenet-training-CN.md#testing-classification-model-in-digits)

	* [将模型快照下载到 Jetson](docs/imagenet-snapshot-CN.md)

	* [在 Jetson 上加载自定义模型](docs/imagenet-custom-CN.md)

* [使用DetectNet定位物体](docs/detectnet-training-CN.md)

	* [检测数据的数字格式](docs/detectnet-training-CN.md#detection-data-formatting-in-digits)

	* [下载检测数据集](docs/detectnet-training-CN.md#downloading-the-detection-dataset)

	* [将检测数据集导入DIGITS](docs/detectnet-training-CN.md#importing-the-detection-dataset-into-digits)

	* [使用数字创建 DetectNet 模型](docs/detectnet-training-CN.md#creating-detectnet-model-with-digits)

	* [在 DIGITS 中测试 DetectNet 模型推理](docs/detectnet-training-CN.md#testing-detectnet-model-inference-in-digits)

	* [将检测模型下载到Jetson](docs/detectnet-snapshot-CN.md)

	* [TensorRT 的 DetectNet 补丁](docs/detectnet-snapshot-CN.md#detectnet-patches-for-tensorrt)

	* [从命令行检测对象](docs/detectnet-console-CN.md)

	* [多类目标检测模型](docs/detectnet-console-CN.md#multi-class-object-detection-models)

	* [在 Jetson 上运行实时摄像头检测演示](docs/detectnet-camera-CN.md)

* [SegNet 语义分割](docs/segnet-dataset-CN.md)

	* [下载航拍无人机数据集](docs/segnet-dataset-CN.md#downloading-aerial-drone-dataset)

	* [将航拍数据集导入 DIGITS](docs/segnet-dataset-CN.md#importing-the-aerial-dataset-into-digits)

	* [生成预训练的 FCN-Alexnet](docs/segnet-pretrained-CN.md)

	* [使用 DIGITS 训练 FCN-Alexnet](docs/segnet-training-CN.md)

	* [测试 DIGITS 中的推理模型](docs/segnet-training-CN.md#testing-inference-model-in-digits)

	* [TensorRT 的 FCN-Alexnet 补丁](docs/segnet-patches-CN.md)

	* [在 Jetson 上运行分割模型](docs/segnet-console-CN.md)


</详情>


##
<palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="#deploying-deep-learning"><sup>目录</sup></a></p>

