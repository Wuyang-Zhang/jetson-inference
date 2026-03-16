<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="imagenet-tagging-CN.md">返回</a> | <a href="detectnet-camera-2-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>物体检测</sup></s></p>


# 使用 DetectNet 定位对象
前面的识别示例输出代表整个输入图像的类概率。  接下来，我们将重点关注**目标检测**，并通过提取其边界框来查找各种对象在帧中的位置。  与图像分类不同，目标检测网络能够在每帧中检测许多不同的对象。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet.jpg" >


[`detectNet`](../c/detectNet.h) 对象接受图像作为输入，并输出检测到的边界框的坐标列表及其类别和置信度值。  [`detectNet`](../c/detectNet.h) 可在 [Python](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#detectNet) 和 [C++](../c/detectNet.h) 中使用。  请参阅下文，了解可供下载的各种[预训练检测模型](#pre-trained-detection-models-available)。  使用的默认模型是在 MS COCO 数据集上训练的 [91 级](../data/networks/ssd_coco_labels.txt) SSD-Mobilenet-v2 模型，它通过 TensorRT 在 Jetson 上实现了实时推理性能。


作为使用 `detectNet` 类的示例，我们提供了 C++ 和 Python 的示例程序: 


- [`detectnet.cpp`](../examples/detectnet/detectnet.cpp) (C++)

- [`detectnet.py`](../python/examples/detectnet.py) (Python)


这些样本能够检测图像、视频和摄像头中的对象。  有关支持的各种类型的输入/输出流的详细信息，请参阅[相机流和多媒体](aux-streaming-CN.md) 页面。


### 从图像中检测对象


首先，让我们尝试使用 `detectnet` 程序来定位静态图像中的对象。  除了输入/输出路径之外，还有一些附加的命令行选项: 


- 可选 `--network` 标志，用于更改正在使用的[检测模型](detectnet-console-2-CN.md#pre-trained-detection-models-available)（默认为 SSD-Mobilenet-v2）。

- 可选的 `--overlay` 标志，可以是 `box`、`lines`、`labels`、`conf` 和 `none` 的逗号分隔组合

	- 默认值为 `--overlay=box,labels,conf`，显示框、标签和置信度值

	- `box` 选项绘制填充的边界框，而 `lines` 只绘制未填充的轮廓

- 可选 `--alpha` 值，用于设置叠加期间使用的 alpha 混合值（默认为 `120`）。

- 可选 `--threshold` 值，设置检测的最小阈值（默认为 `0.5`）。


如果您使用 [Docker 容器](aux-docker-CN.md)，建议将输出图像保存到 `images/test` 安装目录。  然后，您可以在主机设备的 `jetson-inference/data/images/test` 下轻松查看这些图像（有关详细信息，请参阅[已安装的数据卷](aux-docker-CN.md#mounted-data-volumes)）。


以下是使用默认 SSD-Mobilenet-v2 模型检测图像中行人的一些示例: 


`` 重击
# C++
$ ./detectnet --network=ssd-mobilenet-v2 images/peds_0.jpg images/test/output.jpg # --network 标志是可选的


# Python
$ ./detectnet.py --network=ssd-mobilenet-v2 images/peds_0.jpg images/test/output.jpg # --network 标志是可选的
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-ssd-peds-0.jpg" >


`` 重击
# C++
$ ./Detectnet 图片/peds_1.jpg 图片/test/output.jpg


# Python
$ ./detectnet.py 图片/peds_1.jpg 图片/test/output.jpg
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-ssd-peds-1.jpg" >


> **注意**: 第一次运行每个模型时，TensorRT 将花费几分钟来优化网络。 <br/>

> 然后，此优化的网络文件会缓存到磁盘，因此将来使用该模型的运行将加载得更快。


以下是控制台程序输出的更多检测示例。  基于 SSD 的模型所训练的 [91 类](../data/networks/ssd_coco_labels.txt) MS COCO 数据集包括要检测的人、车辆、动物和各种类型的家用物品。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-animals.jpg" >


在`images/`下找到各种图像进行测试，例如`cat_*.jpg`、`dog_*.jpg`、`horse_*.jpg`、`peds_*.jpg`等。


### 处理图像目录或序列


如果您想要一次处理多个图像，则可以使用包含图像或通配符序列的目录路径启动 `detectnet` 程序: 


````bash
# C++
./Detectnet“图像/peds_*.jpg”图像/test/peds_output_%i.jpg


# Python
./detectnet.py“images/peds_*.jpg”图像/test/peds_output_%i.jpg
````


> **注意: ** 使用通配符时，请始终将其括在引号 (`"*.jpg"`) 中。否则，操作系统将自动扩展序列并修改命令行上的参数顺序，这可能会导致输入图像之一被输出覆盖。


有关加载/保存图像序列的详细信息，请参阅[相机流和多媒体](aux-streaming-CN.md#sequences) 页面。


### 处理视频文件


您还可以处理磁盘中的视频。  有关加载/保存视频的详细信息，请参阅[此处](aux-streaming-CN.md#video-files)。


`` 重击
# 下载测试视频
wget https://nvidia.box.com/shared/static/veuuimq6pwvd62p9fresqhrrmfqz0e2f.mp4 -O 行人.mp4


# C++
./Detectnet 行人.mp4 图片/测试/pedestrians_ssd.mp4


# Python
./Detectnet.py 行人.mp4 图片/测试/pedestrians_ssd.mp4
````


<a href="https://www.youtube.com/watch?v=EbTyTJS9jOQ" target="_blank"><img src=https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-ssd-pedestrians-video.jpg width="750"></a>


`` 重击
# 下载测试视频
wget https://nvidia.box.com/shared/static/i5i81mkd9wdh4j7wx04th961zks0lfh9.avi -O parking.avi


# C++
./Detectnet parking.avi 图片/test/parking_ssd.avi


# Python
./Detectnet.py parking.avi 图片/test/parking_ssd.avi
````


<a href="https://www.youtube.com/watch?v=iB86W-kloPE" target="_blank"><img src=https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-ssd-parking-video.jpg width="585"></a>


请记住，您可以使用 `--threshold` 设置来向上或向下更改检测灵敏度（默认值为 0.5）。


### 提供预训练的检测模型


下面是可供使用的预训练目标检测网络的表格，以及用于加载预训练模型的 `detectnet` 的关联 `--network` 参数: 


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


### 运行不同的检测模型


您可以通过将命令行上的 `--network` 标志设置为上表中相应的 CLI 参数之一来指定要加载的模型。  默认情况下，如果未指定可选的 `--network` 标志，则为 SSD-Mobilenet-v2。


例如，如果您选择使用[Model Downloader](building-repo-2-CN.md#downloading-models)工具下载SSD-Inception-v2，则可以像这样使用它: 


`` 重击
# C++
$ ./detectnet --network=ssd-inception-v2 输入.jpg 输出.jpg


# Python
$ ./detectnet.py --network=ssd-inception-v2 输入.jpg 输出.jpg
````


### 源代码


作为参考，下面是 [`detectnet.py`](../python/examples/detectnet.py) 的源代码: 


```` 蟒蛇
导入 jetson.inference
导入jetson.utils


导入argparse
导入系统


# 解析命令行
parser = argparse.ArgumentParser(description="使用目标检测 DNN 定位实时摄像机流中的对象。")


parser.add_argument("input_URI", type=str, default="", nargs='?', help="输入流的 URI")
parser.add_argument("output_URI", type=str, default="", nargs='?', help="输出流的 URI")
parser.add_argument("--network", type=str, default="ssd-mobilenet-v2", help="要加载的预训练模型（选项见下文）")
parser.add_argument("--overlay", type=str, default="box,labels,conf", help="检测覆盖标志（例如 --overlay=box,labels,conf）\n有效组合为:  'box', 'labels', 'conf', 'none'")
parser.add_argument("--threshold", type=float, default=0.5, help="要使用的最小检测阈值")


尝试: 
	opt = parser.parse_known_args()[0]
除外: 
	打印（“”）
	parser.print_help()
	系统退出(0)


# 加载目标检测网络
net = jetson.inference.detectNet(opt.network, sys.argv, opt.threshold)


# 创建视频源和输出
输入 = jetson.utils.videoSource(opt.input_URI, argv=sys.argv)
输出 = jetson.utils.videoOutput(opt.output_URI, argv=sys.argv)


# 处理帧直到用户退出
而真实: 
	# 捕获下一张图像
	img = 输入.Capture()


# 检测图像中的对象（带覆盖）
	检测= net.Detect（img，overlay=opt.overlay）


# 打印检测结果
	print("在图像中检测到 {:d} 个对象".format(len(detections)))


用于检测中的检测: 
		打印（检测）


# 渲染图像
	输出.渲染(img)


# 更新标题栏
	output.SetStatus("{:s} | Network {:.0f} FPS".format(opt.network, net.GetNetworkFPS()))


# 打印出性能信息
	net.PrintProfilerTimes()


# 输入/输出 EOS 时退出
	如果不是 input.IsStreaming() 或不是 output.IsStreaming(): 
		打破
````


接下来，我们将在实时摄像机流上运行目标检测。


##
<palign="right">下一个| <b><a href="detectnet-camera-2-CN.md">运行实时摄像头检测演示</a></b>
<br/>
返回 | <b><a href="imagenet-tagging-CN.md">图像标记的多标签分类</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>