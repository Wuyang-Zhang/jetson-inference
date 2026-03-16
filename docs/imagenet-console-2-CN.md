<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="building-repo-2-CN.md">返回</a> | <a href="imagenet-example-python-2-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>图像分类</sup></p>


# 使用 ImageNet 对图像进行分类
有多种类型的深度学习网络可用，包括识别、检测/定位和语义分割。  我们在本教程中强调的第一个深度学习功能是**图像识别**，使用在大型数据集上训练的分类网络来识别场景和对象。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet.jpg" width="1000">


[`imageNet`](../c/imageNet.h) 对象接受输入图像并输出每个类别的概率。  在 **[1000 个对象](../data/networks/ilsvrc12_synset_words.txt)** 的 ImageNet ILSVRC 数据集上进行训练后，GoogleNet 和 ResNet-18 模型会在构建步骤中自动下载。  有关也可以下载和使用的其他分类模型，请参阅[下文](#downloading-other-classification-models)。


作为使用 [`imageNet`](../c/imageNet.h) 类的示例，我们提供了 C++ 和 Python 的示例程序: 


- [`imagenet.cpp`](../examples/imagenet/imagenet.cpp) (C++)

- [`imagenet.py`](../python/examples/imagenet.py) (Python)


这些样本能够对图像、视频和摄像头源进行分类。  有关支持的各种类型的输入/输出流的详细信息，请参阅[相机流和多媒体](aux-streaming-CN.md) 页面。



### 在 Jetson 上使用 ImageNet 程序


首先，我们尝试使用 `imagenet` 程序在一些示例图像上测试 imageNet 识别。  它加载一张或多张图像，使用 TensorRT 和 `imageNet` 类执行推理，然后叠加分类结果并保存输出图像。  该项目附带位于 `images/` 目录下的示例图像供您使用。


[构建](building-repo-2-CN.md) 项目后，确保您的终端位于 `aarch64/bin` 目录中: 


``` bash
$ cd jetson-inference/build/aarch64/bin
```


接下来，让我们使用 [C++](../examples/imagenet/imagenet.cpp) 或 [Python](../python/examples/imagenet.py) 变体，通过 `imagenet` 程序对示例图像进行分类。  如果您使用 [Docker 容器](aux-docker-CN.md)，建议将分类输出图像保存到 `images/test` 挂载目录中。  然后，您可以从主机设备的 `jetson-inference/data/images/test` 目录中轻松查看这些图像（有关详细信息，请参阅[已安装的数据卷](aux-docker-CN.md#mounted-data-volumes)）。


`` 重击
# C++
$ ./imagenet images/orange_0.jpg images/test/output_0.jpg # (默认网络是googlenet)


# Python
$ ./imagenet.py images/orange_0.jpg images/test/output_0.jpg # (默认网络是googlenet)
````


> **注意**: 第一次运行每个模型时，TensorRT 将花费几分钟来优化网络。 <br/>

> 然后，此优化的网络文件会缓存到磁盘，因此将来使用该模型的运行将加载得更快。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet-orange.jpg" width="650">


`` 重击
# C++
$ ./imagenet 图片/strawberry_0.jpg 图片/test/output_1.jpg


# Python
$ ./imagenet.py 图片/strawberry_0.jpg 图片/test/output_1.jpg
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet-strawberry.jpg" width="650">


除了加载单个图像之外，您还可以加载目录或图像序列或视频文件。  有关详细信息，请参阅[相机流和多媒体](aux-streaming-CN.md) 页面或使用 `--help` 标志启动应用程序。


### 下载其他分类模型


以下预训练的图像分类模型可供使用，并将自动下载（默认为 `googlenet`）: 


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


一般来说，越复杂的网络可以具有更高的分类精度，同时运行时间也更长。


### 使用不同的分类模型


您可以通过将命令行上的 `--network` 标志设置为上表中相应的 CLI 参数之一来指定要加载的模型。  默认情况下，如果未指定可选的 `--network` 标志，则会加载 GoogleNet。


以下是使用 ResNet-18 模型的一些示例: 


`` 重击
# C++
$ ./imagenet --network=resnet-18 图片/jellyfish.jpg 图片/test/output_jellyfish.jpg


# Python
$ ./imagenet.py --network=resnet-18 图片/jellyfish.jpg 图片/test/output_jellyfish.jpg
````


<img src="https://raw.githubusercontent.com/dusty-nv/jetson-inference/master/docs/images/imagenet_jellyfish.jpg" width="650">


`` 重击
# C++
$ ./imagenet --network=resnet-18 图片/stingray.jpg 图片/test/output_stingray.jpg


# Python
$ ./imagenet.py --network=resnet-18 图片/stingray.jpg 图片/test/output_stingray.jpg
````


<img src="https://raw.githubusercontent.com/dusty-nv/jetson-inference/master/docs/images/imagenet_stingray.jpg" width="650">


`` 重击
# C++
$ ./imagenet --network=resnet-18 图片/coral.jpg 图片/test/output_coral.jpg


# Python
$ ./imagenet.py --network=resnet-18 图片/coral.jpg 图片/test/output_coral.jpg
````


<img src="https://raw.githubusercontent.com/dusty-nv/jetson-inference/master/docs/images/imagenet_coral.jpg" width="650">


请随意尝试使用不同的模型，看看它们的准确性和性能有何不同 - 您可以使用 [模型下载器](building-repo-2-CN.md#downloading-models) 工具下载更多模型。  在`images/`下还找到了各种测试图像


### 处理视频


[相机流和多媒体](aux-streaming-CN.md) 页面显示 `imagenet` 程序可以处理的不同类型的流。


以下是在磁盘上的视频上运行它的示例: 


`` 重击
# 下载测试视频（感谢jell.yfish.us）
$ wget https://nvidia.box.com/shared/static/tlswont1jnyu3ix2tbf7utaekpzcx4rc.mkv -O jellyfish.mkv


# C++
$ ./imagenet --network=resnet-18 jellyfish.mkv 图片/test/jellyfish_resnet18.mkv


# Python
$ ./imagenet.py --network=resnet-18 jellyfish.mkv 图片/test/jellyfish_resnet18.mkv
````


<a href="https://www.youtube.com/watch?v=GhTleNPXqyU" target="_blank"><img src=https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet-jellyfish-video.jpg width="750"></a>


接下来，我们将完成从头开始编写您自己的图像识别程序的步骤，首先使用 Python，然后使用 C++。


##
<palign="right">下一个| <b><a href="imagenet-example-python-2-CN.md">编写您自己的图像识别程序 (Python)</a></b>
<br/>
返回 | <b><a href="building-repo-2-CN.md">从源代码构建存储库</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>