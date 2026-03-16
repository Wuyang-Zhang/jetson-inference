<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="building-repo-CN.md">返回</a> | <a href="imagenet-example-CN.md">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>图像识别</sup></p>


# 使用 ImageNet 对图像进行分类
有多种类型的深度学习网络可用，包括识别、检测/定位和语义分割。  我们在本教程中强调的第一个深度学习功能是使用经过训练来识别场景和对象的分类网络进行图像识别。


[`imageNet`](../c/imageNet.h) 对象接受输入图像并输出每个类别的概率。  在 **[1000 个对象](../data/networks/ilsvrc12_synset_words.txt)** 的 ImageNet ILSVRC 数据集上进行训练后，GoogleNet 和 ResNet-18 模型会在构建步骤中自动下载。  有关也可以下载和使用的其他分类模型，请参阅[下文](#downloading-other-classification-models)。


作为使用 [`imageNet`](../c/imageNet.h) 的示例，我们提供了 C++ 和 Python 的命令行界面版本: 


- [`imagenet-console.cpp`](../examples/imagenet-console/imagenet-console.cpp) (C++)

- [`imagenet-console.py`](../python/examples/imagenet-console.py) (Python)


在本教程的后面部分，我们还将介绍 C++ 和 Python 的实时摄像头识别程序的版本: 


- [`imagenet-camera.cpp`](../examples/imagenet-camera/imagenet-camera.cpp) (C++)

- [`imagenet-camera.py`](../python/examples/imagenet-camera.py) (Python)



### 在 Jetson 上使用控制台程序


首先，我们尝试使用 `imagenet-console` 程序在一些示例图像上测试 imageNet 识别。  它加载图像，使用 TensorRT 和 `imageNet` 类执行推理，然后叠加分类结果并保存输出图像。  该存储库附带了一些示例图像供您使用。


[构建](building-repo-2-CN.md) 存储库后，确保您的终端位于 `aarch64/bin` 目录中: 


``` bash
$ cd jetson-inference/build/aarch64/bin
```


接下来，让我们使用 [C++](../examples/imagenet-console/imagenet-console.cpp) 或 [Python](../python/examples/imagenet-console.py) 变体，通过 `imagenet-console` 程序对示例图像进行分类。


`imagenet-console` 接受 3 个命令行参数: 


- 输入图像的路径 (`jpg, png, tga, bmp`)

- 输出图像的可选路径 (`jpg, png, tga, bmp`)

- 可选 `--network` 标志，用于更改正在使用的分类模型（默认网络是 GoogleNet）。


请注意，还有其他命令行参数可用于加载自定义模型。  使用 `--help` 标志启动应用程序以接收有关使用它们的更多信息，或参阅 [`Code Examples`](../README-CN.md#code-examples) 自述文件。


以下是在 C++ 或 Python 中运行程序的几个示例: 


#### C++
``` bash
$ ./imagenet-console --network=googlenet orange_0.jpg output_0.jpg  # --network flag is optional
```


####Python
``` bash
$ ./imagenet-console.py --network=googlenet orange_0.jpg output_0.jpg  # --network flag is optional
```


> **注意**: 第一次运行程序时，TensorRT 可能需要长达几分钟的时间来优化网络。 <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;此优化的网络文件在首次运行后缓存到磁盘，因此以后的运行加载速度会更快。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet-orange.jpg" width="500">


#### C++
``` bash
$ ./imagenet-console granny_smith_1.jpg output_1.jpg
```


####Python
``` bash
$ ./imagenet-console.py granny_smith_1.jpg output_1.jpg
```


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet-apple.jpg" width="500">



### 下载其他分类模型


默认情况下，存储库设置为在构建步骤期间下载 GoogleNet 和 ResNet-18 网络。


如果您选择[下载](building-repo-2-CN.md#downloading-models)，您还可以使用其他预训练模型: 


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


> **注意**: 要下载其他网络，请运行[模型下载器](building-repo-2-CN.md#downloading-models)工具<br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ cd jetson-inference/tools` <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ ./download-models.sh` <br/>


一般来说，越复杂的网络可以具有更高的分类精度，同时运行时间也更长。


### 使用不同的分类模型


您可以通过将命令行上的 `--network` 标志设置为上表中相应的 CLI 参数之一来指定要加载的模型。  默认情况下，如果未指定可选的 `--network` 标志，则会加载 GoogleNet。


以下是使用 ResNet-18 模型的一些示例: 


`` 重击
# C++
$ ./imagenet-console --network=resnet-18 jellyfish.jpg 输出_jellyfish.jpg


# Python
$ ./imagenet-console.py --network=resnet-18 jellyfish.jpg 输出_jellyfish.jpg
````


<img src="https://raw.githubusercontent.com/dusty-nv/jetson-inference/master/docs/images/imagenet_jellyfish.jpg" width="650">


`` 重击
# C++
$ ./imagenet-console --network=resnet-18 stingray.jpg output_stingray.jpg


# Python
$ ./imagenet-console.py --network=resnet-18 stingray.jpg output_stingray.jpg
````


<img src="https://raw.githubusercontent.com/dusty-nv/jetson-inference/master/docs/images/imagenet_stingray.jpg" width="650">


`` 重击
# C++
$ ./imagenet-console.py --network=resnet-18 Coral.jpg Output_coral.jpg


# Python
$ ./imagenet-console.py --network=resnet-18 Coral.jpg Output_coral.jpg
````


<img src="https://raw.githubusercontent.com/dusty-nv/jetson-inference/master/docs/images/imagenet_coral.jpg" width="650">


请随意尝试使用不同的模型，看看它们的准确性和性能有何不同。


接下来，我们将完成从头开始编写您自己的图像识别程序的步骤。


##
<palign="right">下一个| <b><a href="imagenet-example-CN.md">编写自己的图像识别程序</a></b>
<br/>
返回 | <b><a href="building-repo-CN.md">从源代码构建存储库</a></b></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>