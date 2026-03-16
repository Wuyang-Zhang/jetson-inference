<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="detectnet-tracking-CN.md">返回</a> | <a href="segnet-camera-2-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>语义分割</sup></s></p>


# 使用 SegNet 进行语义分割
我们将在本教程中介绍的下一个深度学习功能是**语义分割**。  语义分割基于图像识别，但分类发生在像素级别而不是整个图像。  这是通过对预训练的图像识别主干进行“卷积化”来实现的，该主干将模型转换为能够进行每像素标记的[全卷积网络 (FCN)](https://arxiv.org/abs/1605.06211)。  对于环境感知特别有用，分割可以对每个场景的许多不同潜在对象（包括场景前景和背景）产生密集的每像素分类。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation.jpg">


[`segNet`](../c/segNet.h) 接受 2D 图像作为输入，并输出带有每像素分类掩码叠加的第二个图像。  掩模的每个像素对应于被分类的对象的类别。  [`segNet`](../c/segNet.h) 可在 [Python](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#segNet) 和 [C++](../c/segNet.h) 中使用。


作为使用 `segNet` 类的示例，我们提供了 C++ 和 Python 示例程序: 


- [`segnet.cpp`](../examples/segnet/segnet.cpp) (C++)

- [`segnet.py`](../python/examples/segnet.py) (Python)


这些样本能够分割图像、视频和摄像头源。  有关支持的各种类型的输入/输出流的详细信息，请参阅[相机流和多媒体](aux-streaming-CN.md) 页面。


请参阅[下文](#pretrained-segmentation-models-available)，了解各种可用的预训练分割模型，这些模型使用在 Jetson 上具有实时性能的 FCN-ResNet18 网络。  提供了适用于各种环境和主题的模型，包括城市、越野步道以及室内办公空间和家庭。


### 提供预训练的分割模型


下面是可供使用的预训练语义分割模型的表格，以及用于加载它们的 `segnet` 的关联 `--network` 参数。  它们基于 21 类 FCN-ResNet18 网络，并使用 [PyTorch](https://github.com/dusty-nv/pytorch-segmentation) 在各种数据集和分辨率上进行了训练，并导出为 [ONNX 格式](https://onnx.ai/) 以使用 TensorRT 加载。


|数据集 |分辨率| CLI 参数 |准确度|杰特森纳米|杰特森·泽维尔 |
|:------------:|:----------:|--------------|:--------:|:------------:|:-------------:|
| [城市景观](https://www.cityscapes-dataset.com/) | 512x256 | `fcn-resnet18-cityscapes-512x256` | 83.3% | 48 帧/秒 | 480 帧/秒 |
| [城市景观](https://www.cityscapes-dataset.com/) | 1024x512 | `fcn-resnet18-cityscapes-1024x512` | 87.3% | 12 帧/秒 | 175 帧/秒 |
| [城市景观](https://www.cityscapes-dataset.com/) | 2048x1024 | 2048x1024 `fcn-resnet18-cityscapes-2048x1024` | 89.6% | 3 FPS | 47 帧/秒 |
| [DeepScene](http://deepscene.cs.uni-freiburg.de/) | 576x320 | `fcn-resnet18-deepscene-576x320` | 96.4% | 26 帧/秒 | 360 FPS |
| [DeepScene](http://deepscene.cs.uni-freiburg.de/) | 864x480 | 864x480 `fcn-resnet18-deepscene-864x480` | 96.9% | 14 帧/秒 | 190 帧/秒 |
| [多人](https://lv-mhp.github.io/) | 512x320 | `fcn-resnet18-mhp-512x320` | 86.5% | 34 帧/秒 | 370 帧/秒 |
| [多人](https://lv-mhp.github.io/) | 640x360 | `fcn-resnet18-mhp-640x360` | 87.1% | 23 帧/秒 | 325 帧/秒 |
| [帕斯卡 VOC](http://host.robots.ox.ac.uk/pascal/VOC/) | 320x320 | 320x320 `fcn-resnet18-voc-320x320` | 85.9% | 45 帧/秒 | 508 帧 |
| [帕斯卡 VOC](http://host.robots.ox.ac.uk/pascal/VOC/) | 512x320 | `fcn-resnet18-voc-512x320` | 88.5% | 34 帧/秒 | 375 帧/秒 |
| [SUN RGB-D](http://rgbd.cs.princeton.edu/) | 512x400 | 512x400 `fcn-resnet18-sun-512x400` | 64.3% | 28 帧/秒 | 340 帧/秒 |
| [SUN RGB-D](http://rgbd.cs.princeton.edu/) | 640x512 | 640x512 `fcn-resnet18-sun-640x512` | 65.1% | 17 帧/秒 | 224 帧/秒 |


* 如果 CLI 参数中省略分辨率，则加载最低分辨率模型

* 准确度表示模型验证数据集中的像素分类准确度

* 使用 JetPack 4.2.1、`nvpmodel 0` (MAX-N) 测量 GPU FP16 模式的性能


### 从命令行分割图像


首先，我们尝试使用 `segnet` 程序来分割静态图像。  除了输入/输出路径之外，还有一些附加的命令行选项: 


- 可选 `--network` 标志更改正在使用的分段模型（请参阅[上文](#pre-trained-segmentation-models-available)）

- 可选 `--visualize` 标志接受 `mask` 和/或 `overlay` 模式（默认为 `overlay`）

- 可选 `--alpha` 标志设置 `overlay` 的 alpha 混合值（默认为 `120`）

- 可选 `--filter-mode` 标志接受 `point` 或 `linear` 采样（默认为 `linear`）


使用 `--help` 标志启动应用程序以获取更多信息，并参阅[相机流和多媒体](aux-streaming-CN.md) 页面以获取支持的输入/输出协议。


以下是该程序的一些用法示例: 


#### C++


``` bash
$ ./segnet --network=<model> input.jpg output.jpg                  # overlay segmentation on original
$ ./segnet --network=<model> --alpha=200 input.jpg output.jpg      # make the overlay less opaque
$ ./segnet --network=<model> --visualize=mask input.jpg output.jpg # output the solid segmentation mask
```


#### Python


``` bash
$ ./segnet.py --network=<model> input.jpg output.jpg                  # overlay segmentation on original
$ ./segnet.py --network=<model> --alpha=200 input.jpg output.jpg      # make the overlay less opaque
$ ./segnet.py --network=<model> --visualize=mask input.jpg output.jpg # output the segmentation mask
```


### 城市景观


让我们看看一些不同的场景。  以下是使用 [Cityscapes](https://www.cityscapes-dataset.com/) 模型分割城市街道场景的示例: 


`` 重击
# C++
$ ./segnet --network=fcn-resnet18-cityscapes 图片/city_0.jpg 图片/test/output.jpg


# Python
$ ./segnet.py --network=fcn-resnet18-cityscapes 图片/city_0.jpg 图片/test/output.jpg
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-city.jpg" width="1000">


在 `images/` 子目录下可以找到更多名为 `city-*.jpg` 的测试图像，用于尝试 Cityscapes 模型。


### 深景


[DeepScene 数据集](http://deepscene.cs.uni-freiburg.de/) 由越野森林步道和植被组成，有助于户外机器人的路径跟踪。  
以下是通过指定 `--visualize` 参数生成分段覆盖和掩码的示例: 


#### C++
``` bash
$ ./segnet --network=fcn-resnet18-deepscene images/trail_0.jpg images/test/output_overlay.jpg                # overlay
$ ./segnet --network=fcn-resnet18-deepscene --visualize=mask images/trail_0.jpg images/test/output_mask.jpg  # mask
```


####Python
``` bash
$ ./segnet.py --network=fcn-resnet18-deepscene images/trail_0.jpg images/test/output_overlay.jpg               # overlay
$ ./segnet.py --network=fcn-resnet18-deepscene --visualize=mask images/trail_0.jpg images/test/output_mask.jpg # mask
```


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-deepscene-0-overlay.jpg" width="850">
<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-deepscene-0-mask.jpg">


`images/` 子目录下还有更多名为 `trail-*.jpg` 的示例图像。


### 多人解析（MHP）


[多人解析](https://lv-mhp.github.io/) 提供身体部位的密集标签，例如手臂、腿、头部和不同类型的衣服。  
 请参阅在 `images/` 下找到的一些名为 `humans-*.jpg` 的测试图像来尝试 MHP 模型: 


`` 重击
# C++
$ ./segnet --network=fcn-resnet18-mhp 图片/people_0.jpg 图片/test/output.jpg


# Python
$ ./segnet.py --network=fcn-resnet18-mhp 图片/ humans_0.jpg 图片/test/output.jpg
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-mhp-0.jpg" width="825">
<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-mhp-1.jpg" width="825">


#### MHP 课程


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-mhp-legend.jpg">


### 帕斯卡VOC


[Pascal VOC](http://host.robots.ox.ac.uk/pascal/VOC/) 是用于语义分割的原始数据集之一，包含各种人、动物、车辆和家居物品。  其中包含一些名为 `object-*.jpg` 的示例图像，用于测试 Pascal VOC 模型: 


`` 重击
# C++
$ ./segnet --network=fcn-resnet18-voc 图片/object_0.jpg 图片/test/output.jpg


# Python
$ ./segnet.py --network=fcn-resnet18-voc 图片/object_0.jpg 图片/test/output.jpg
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-voc.jpg" width="1000">


#### 挥发性有机化合物类别


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-voc-legend.jpg">


### 太阳 RGB-D


[SUN RGB-D](http://rgbd.cs.princeton.edu/) 数据集为办公空间和家庭中常见的许多室内物体和场景提供分割地面实况。  请参阅在 `images/` 子目录下找到的名为 `room-*.jpg` 的图像来测试 SUN 模型: 


`` 重击
# C++
$ ./segnet --network=fcn-resnet18-sun 图片/room_0.jpg 图片/test/output.jpg


# Python
$ ./segnet.py --network=fcn-resnet18-sun images/room_0.jpg images/test/output.jpg
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-sun.jpg" width="1000">


#### 太阳课程


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-sun-legend.jpg">


### 处理图像目录或序列


如果要处理目录或图像序列，可以使用包含图像或通配符序列的目录路径启动程序: 


`` 重击
# C++
$ ./segnet --network=fcn-resnet18-sun "images/room_*.jpg" images/test/room_output_%i.jpg


# Python
$ ./segnet.py --network=fcn-resnet18-sun "images/room_*.jpg" images/test/room_output_%i.jpg
````


> **注意: ** 使用通配符时，请始终将其括在引号 (`"*.jpg"`) 中。否则，操作系统将自动扩展序列并修改命令行上的参数顺序，这可能会导致输入图像之一被输出覆盖。


有关加载/保存图像序列的详细信息，请参阅[相机流和多媒体](aux-streaming-CN.md#sequences) 页面。  接下来，我们将对实时摄像机或视频流运行分段。


##
<palign="right">下一个| <b><a href="segnet-camera-2-CN.md">运行实时摄像头分割演示</a></b>
<br/>
返回 | <b><a href="detectnet-tracking-CN.md">视频上的对象跟踪</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>