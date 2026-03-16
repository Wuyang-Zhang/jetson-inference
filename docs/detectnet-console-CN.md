<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="detectnet-snapshot-CN.md">返回</a> | <a href="detectnet-camera-CN.md">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>物体检测</sup></s></p>


# 从命令行检测对象


要在 Jetson 上使用 [`detectNet`](../c/detectNet.h) 和 TensorRT 处理测试图像，我们可以使用 [`detectnet-console`](../detectnet-console/detectnet-console.cpp) 程序。


[`detectnet-console`](../detectnet-console/detectnet-console.cpp) 接受表示输入图像路径和输出图像路径（渲染边界框覆盖）的命令行参数。  存储库中还包含一些测试图像。


要指定您在上一步中从 DIGITS 下载的模型，请使用下面的 `detectnet-console` 语法。  首先，为了方便起见，将提取的快照的路径设置为 `$NET` 变量: 


`` 重击
$ NET=20170504-190602-879f_epoch_100


$ ./detectnet-consoledog_0.jpg 输出_0.jpg \
--prototxt=$NET/deploy.prototxt \
--model=$NET/snapshot_iter_38600.caffemodel \
--input_blob=数据 \ 
--output_cvg=覆盖范围\
--output_bbox=bbox
````


> **注意: **如果您的 DetectNet 图层名称与上述默认值匹配（即，如果您使用本教程中的 prototxt），则可以省略 `input_blob`、`output_cvg` 和 `output_bbox` 参数。如果您使用具有不同层名称的自定义 DetectNet，则会提供这些可选命令行参数。


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-tensorRT-dog-0.jpg)


### 使用预训练模型启动


或者，要加载存储库附带的预训练快照之一，您可以指定可选的 `--network` 标志，该标志会更改正在使用的检测模型（默认网络是 PedNet）。


以下是使用默认 PedNet 模型在图像中定位人体的示例: 


#### C++


``` bash
$ ./detectnet-console peds-004.jpg output.jpg
```


#### Python


``` bash
$ ./detectnet-console.py peds-004.jpg output.jpg
```


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-peds-00.jpg" width="900">



### 提供预训练的检测模型


下面是可用于[下载](building-repo-CN.md#downloading-models)的预训练目标检测网络的表格，以及用于加载预训练模型的 `detectnet-console` 的相关 `--network` 参数: 


|型号| CLI 参数 |网络类型枚举 |对象类 |
| ------------------------|--------------------|--------------------|----------------------|
| SSD-Mobilenet-v1 | `ssd-mobilenet-v1` | `SSD_MOBILENET_V1` | 91 ([COCO 类](https://raw.githubusercontent.com/AastaNV/TRT_object_detection/master/coco.py)) |
| SSD-Mobilenet-v2 | `ssd-mobilenet-v2` | `SSD_MOBILENET_V2` | 91 ([COCO 类](https://raw.githubusercontent.com/AastaNV/TRT_object_detection/master/coco.py)) |
| SSD-Inception-v2 | `ssd-inception-v2` | `SSD_INCEPTION_V2` | 91 ([COCO 类](https://raw.githubusercontent.com/AastaNV/TRT_object_detection/master/coco.py)) |
| DetectNet-COCO-Dog | `coco-dog` | `COCO_DOG` |狗 |
| DetectNet-COCO-瓶子| `coco-bottle` | `COCO_BOTTLE` |瓶子|
| DetectNet-COCO-主席 | `coco-chair` | `COCO_CHAIR` |椅子 |
| DetectNet-COCO-飞机 | `coco-airplane` | `COCO_AIRPLANE` |飞机 |
| ped-100 | 佩德-100 `pednet` | `PEDNET` |行人|
| 500 倍 | `multiped` | `PEDNET_MULTI` |行人、行李|
| Facenet-120 | Facenet-120 | `facenet` | `FACENET` |面孔 |


> **注意**: 要下载其他网络，请运行[模型下载器](building-repo-CN.md#downloading-models)工具<br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ cd jetson-inference/tools` <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ ./download-models.sh` <br/>



### 运行不同的检测模型


您可以通过将命令行上的 `--network` 标志设置为上表中相应的 CLI 参数之一来指定要加载的模型。  默认情况下，如果未指定可选的 `--network` 标志，则加载 PedNet（行人检测）。


让我们尝试运行其他一些 COCO 模型: 


`` 重击
# C++
$ ./detectnet-console --network=coco-dog dog_1.jpg output_1.jpg


# Python
$ ./detectnet-console.py --network=coco-dog dog_1.jpg output_1.jpg
````


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-tensorRT-dog-1.jpg)


`` 重击
# C++
$ ./detectnet-console --network=coco-bottle Bottle_0.jpg output_2.jpg


# Python
$ ./detectnet-console.py --network=coco-bottle Bottle_0.jpg output_2.jpg
````


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-tensorRT-bottle-0.jpg)


`` 重击
# C++
$ ./detectnet-console --network=coco-airplaneplane_0.jpgoutput_3.jpg


# Python
$ ./detectnet-console.py --network=coco-airplaneplane_0.jpgoutput_3.jpg
````


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-tensorRT-airplane-0.jpg)



### 多类目标检测模型


一些模型支持检测多种类型的物体。  例如，当在除行人之外还包含行李或行李的图像上使用 `multiped` 模型时，第二个对象类将使用绿色覆盖层进行渲染: 


`` 重击
# C++
$ ./detectnet-console --network=multiped peds-003.jpg output_4.jpg


# Python
$ ./detectnet-console.py --network=multiped peds-003.jpg output_4.jpg
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-peds-01.jpg" width="900">


接下来，我们将在实时摄像机流上运行目标检测。



##
<palign="right">下一个| <b><a href="detectnet-camera-CN.md">运行实时摄像头检测演示</a></b>
<br/>
返回 | <b><a href="detectnet-snapshot-CN.md">将检测模型下载到 Jetson</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>