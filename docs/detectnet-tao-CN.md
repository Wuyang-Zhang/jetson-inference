<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="detectnet-example-2-CN.md">返回</a> | <a href="detectnet-tracking-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>物体检测</sup></p>


# 使用 TAO 检测模型


NVIDIA 的 [TAO 工具包](https://developer.nvidia.com/tao-toolkit) 包括高精度高分辨率目标检测模型，针对 INT8 精度进行了优化/修剪和量化。  jetson-inference 支持基于 [DetectNet_v2](https://docs.nvidia.com/tao/tao-toolkit/text/object_detection/detectnet_v2.html) DNN 架构的 TAO 模型，包括以下预训练模型: 


|型号| CLI 参数 |对象类 |
| ------------------------|--------------------|----------------------|
| [TAO 人民网](https://catalog.ngc.nvidia.com/orgs/nvidia/teams/tao/models/peoplenet) | `peoplenet` |人、包、脸 |
| [TAO PeopleNet](https://catalog.ngc.nvidia.com/orgs/nvidia/teams/tao/models/peoplenet) <sup>（修剪）</sup> | `peoplenet-pruned` |人、包、脸 |
| [TAO DashCamNet](https://catalog.ngc.nvidia.com/orgs/nvidia/teams/tao/models/dashcamnet) | `dashcamnet` |人、车、自行车、标志|
| [TAO TrafficCamNet](https://catalog.ngc.nvidia.com/orgs/nvidia/teams/tao/models/trafficcamnet) | `trafficcamnet` |人、车、自行车、标志| 
| [TAO 人脸检测](https://catalog.ngc.nvidia.com/orgs/nvidia/teams/tao/models/facenet) | `facedetect` |脸 |


尽管[下面的部分](#importing-your-own-tao-detection-models) 介绍了如何加载您自己的 TAO 模型，但让我们首先看一下如何使用预训练的模型。


### 人民网


[PeopleNet](https://catalog.ngc.nvidia.com/orgs/nvidia/teams/tao/models/peoplenet) 是一种高分辨率 960x544 模型，检测人、包和人脸的准确率高达约 90%。  它基于具有 ResNet-34 主干的 DetectNet_v2。  使用 `--model=peoplenet` 启动 detectornet/detectnet.py 将在支持它的平台上以 INT8 精度运行 TAO PeopleNet 模型（否则为 FP16）。  还有 `peoplenet-pruned` 模型，速度更快，但准确性稍差。


<a href="https://youtu.be/rWGTxeb3Nrw" target="_blank"><img src=https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-tao-peoplenet-youtube.jpg></a>



`` 重击
# 下载测试视频
wget https://nvidia.box.com/shared/static/veuuimq6pwvd62p9fresqhrrmfqz0e2f.mp4 -O 行人.mp4


# C++
$检测网络--model = peoplenet行人.mp4行人_peoplenet.mp4


# Python
$ detectornet.py --model=peoplenet 行人.mp4 行人_peoplenet.mp4
````


您还可以调整 `--confidence` 和 `--clustering` 阈值 - 这些 TAO 模型由于准确性提高，似乎不会引入太多阈值较低的误报。  [Flask webapp](webrtc-flask-CN.md) 是一个方便的工具，可以交互地使用这些设置。


### 行车记录仪网络


与 PeopleNet 一样，[DashCamNet](https://catalog.ngc.nvidia.com/orgs/nvidia/teams/tao/models/dashcamnet) 是基于 DetectNet_v2 和 ResNet-34 的 960x544 检测器。  它的预期用途是从街道视角和第一人称视角检测人和车辆。  TrafficCamNet 类似，用于从更高的有利位置拍摄图像。


<a href="https://www.youtube.com/watch?v=tsugHIgFrwI" target="_blank"><img src=https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-tao-dashcamnet-youtube.jpg></a>


`` 重击
# C++
$ detectornet --model=dashcamnet 输入.mp4 输出.mp4


# Python
$ detectornet.py --model=dashcamnet 输入.mp4 输出.mp4
````


> **注意**: 您可以使用[相机流和多媒体](aux-streaming-CN.md)页面中的任何输入/输出来运行此程序


### 人脸检测


[FaceDetect](https://catalog.ngc.nvidia.com/orgs/nvidia/teams/tao/models/facenet) 是一个仅用于检测人脸的 TAO 模型。  它在数据集上进行了训练，准确率高达约 85%，该数据集包含从各种摄像机角度采集的超过 180 万个样本。  它的分辨率为 736x416，并使用带有 ResNet-18 主干的 DetectNet_v2。


<img src=https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-tao-facenet.jpg>


`` 重击
# C++
$ detectornet --model=facedetect "images/ humans_*.jpg" images/test/facedetect_ humans_%i.jpg


# Python
$ detectornet.py --model=facedetect "images/ humans_*.jpg" images/test/facedetect_ humans_%i.jpg
````


### 导入您自己的 TAO 检测模型


尽管 jetson-inference 可以自动下载、转换和加载上面预先训练的 TAO 检测模型，但您可能希望使用这些模型的不同版本或您自己使用 TAO 训练或微调的 DetectNet_v2 模型。  为此，请将经过训练的 ETLT 模型以及适当版本的 [`tao-converter`](https://catalog.ngc.nvidia.com/orgs/nvidia/teams/tao/resources/tao-converter) 工具复制到 Jetson。  然后，根据模型的配置（通常可以在模型卡上找到详细信息），您可以运行如下脚本来从 ETLT 生成 TensorRT 引擎: 


`` 重击
# 模型配置
MODEL_DIR="peoplenet_deployable_quantized_v2.6.1"
MODEL_INPUT="$MODEL_DIR/resnet34_peoplenet_int8.etlt"
MODEL_OUTPUT="$MODEL_INPUT.engine"


INPUT_DIMS="3,544,960"
OUTPUT_LAYERS="output_bbox/BiasAdd,output_cov/Sigmoid"
MAX_BATCH_SIZE =“1”


WORKSPACE="4294967296" # 4GB（默认）
PRECISION="int8" # fp32、fp16、int8
校准=“$MODEL_DIR/resnet34_peoplenet_int8.txt”


ENCRYPTION_KEY="tlt_encode"


# 生成TensorRT引擎
tao 转换器 \
	-k $ENCRYPTION_KEY \
	-d $INPUT_DIMS \
	-o $OUTPUT_LAYERS \
	-m $MAX_BATCH_SIZE \
	-w $工作空间\
	-t $精度 \
	-c $校准\
	-e $MODEL_OUTPUT \
	$模型_输入
````


转换后，您可以使用 detectornet/detectnet.py 加载它，如下所示: 


``` bash
$ detectnet \
	--model=$MODEL_DIR/resnet34_peoplenet_int8.etlt.engine \
	--labels=$MODEL_DIR/labels.txt \
	--input-blob=input_1 \
	--output-cvg=output_cov/Sigmoid \
	--output-bbox=output_bbox/BiasAdd \
	input.mp4 output.mp4
```


> **注意**: jetson-inference 目前仅支持 TAO DetectNet_v2 模型，因为它是为该网络的预处理/后处理设置的


在您自己的应用程序中，您还可以使用 detectorNet API 的扩展形式直接从 [C++](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/classdetectNet.html#a9981735c38d2cb97205aa9e255ab4a0e) 或 [Python](https://github.com/dusty-nv/jetson-inference/blob/89a9bbe8812ec8a142910ae55e9a6c25dbdb9841/python/examples/detectnet.py#L57) 加载它们。


<palign="right">下一个| <b><a href="detectnet-tracking-CN.md">视频上的对象跟踪</a></b>
<br/>
返回 | <b><a href="detectnet-example-2-CN.md">编写您自己的目标检测程序</a></p>
</b><palign="center"><sup>© 2016-2023 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>