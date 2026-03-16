<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="pytorch-transfer-learning-CN.md">返回</a> | <a href="pytorch-plants-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>迁移学习 - 分类</sup></s></p>


# 猫/狗数据集的重新训练


我们将重新训练的第一个模型是一个简单的模型，它识别两个类别: 猫或狗。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-cat-dog.jpg" width="700">


下面提供了一个 800MB 的数据集，其中包括 5000 个训练图像、1000 个验证图像和 200 个测试图像，每个图像均匀地分为猫类和狗类。  训练图像集用于迁移学习，验证集用于评估训练期间的分类准确性，测试图像将在训练完成后供我们使用。  网络从来不会直接在验证集和测试集上进行训练，而只会在训练集上进行训练。


数据集中的图像由许多不同品种的狗和猫组成，包括老虎和山狮等大型猫科动物，因为可用的猫图像数量略低于狗。  有些图像还描绘了人类，检测器本质上经过训练，可以将其作为背景忽略，并专注于猫与狗的内容。


首先，请确保您在 Jetson 上安装了 [PyTorch](pytorch-transfer-learning-CN.md#installing-pytorch)，然后下载下面的数据集并启动训练脚本。  之后，我们将在 TensorRT 中使用一些静态图像和实时摄像头测试重新训练的模型。


## 下载数据


在本教程中，我们将把数据集存储在主机设备上的 `jetson-inference/python/training/classification/data` 下，这是自动[安装到容器中](aux-docker-CN.md#mounted-data-volumes) 的目录之一。  这样，当您关闭容器时，数据集就不会丢失。


``` bash
$ cd jetson-inference/python/training/classification/data
$ wget https://nvidia.box.com/shared/static/o577zd8yp3lmxf5zhm38svrbrv45am3y.gz -O cat_dog.tar.gz
$ tar xvzf cat_dog.tar.gz
```


数据集的镜像可在此处获得: 


* <a href="https://drive.google.com/file/d/16E3yFvVS2DouwgIl4TPFJvMlhGpnYWKF/view?usp=sharing">https://drive.google.com/file/d/16E3yFvVS2DouwgIl4TPFJvMlhGpnYWKF/view?usp=sharing</a>

* <a href="https://nvidia.box.com/s/o577zd8yp3lmxf5zhm38svrbrv45am3y">https://nvidia.box.com/s/o577zd8yp3lmxf5zhm38svrbrv45am3y</a>


## 重新训练 ResNet-18 模型


PyTorch 训练脚本位于 <a href="https://github.com/dusty-nv/jetson-inference/tree/master/python/training/classification">`jetson-inference/python/training/classification/`</a>. 下的存储库中。这些脚本并不特定于任何一个数据集，因此我们将对本教程中的每个示例数据集使用相同的 PyTorch 代码。默认情况下，它设置为训练 ResNet-18 模型，但您可以使用 `--arch` 标志更改它。


要启动训练，请运行以下命令: 


``` bash
$ cd jetson-inference/python/training/classification
$ python3 train.py --model-dir=models/cat_dog data/cat_dog
```


> **注意: ** 如果内存不足或进程在训练期间被“终止”，请尝试[安装交换](pytorch-transfer-learning-CN.md#mounting-swap) 和[禁用桌面 GUI](pytorch-transfer-learning-CN.md#disabling-the-desktop-gui)。 <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;为了节省内存，您还可以减少 `--batch-size` （默认 8）和 `--workers` （默认 2）


训练开始时，您应该看到控制台中显示如下文本: 


``` bash
Use GPU: 0 for training
=> dataset classes:  2 ['cat', 'dog']
=> using pre-trained model 'resnet18'
=> reshaped ResNet fully-connected layer with: Linear(in_features=512, out_features=2, bias=True)
Epoch: [0][  0/625]	Time  0.932 ( 0.932)	Data  0.148 ( 0.148)	Loss 6.8126e-01 (6.8126e-01)	Acc@1  50.00 ( 50.00)	Acc@5 100.00 (100.00)
Epoch: [0][ 10/625]	Time  0.085 ( 0.163)	Data  0.000 ( 0.019)	Loss 2.3263e+01 (2.1190e+01)	Acc@1  25.00 ( 55.68)	Acc@5 100.00 (100.00)
Epoch: [0][ 20/625]	Time  0.079 ( 0.126)	Data  0.000 ( 0.013)	Loss 1.5674e+00 (1.8448e+01)	Acc@1  62.50 ( 52.38)	Acc@5 100.00 (100.00)
Epoch: [0][ 30/625]	Time  0.127 ( 0.114)	Data  0.000 ( 0.011)	Loss 1.7583e+00 (1.5975e+01)	Acc@1  25.00 ( 52.02)	Acc@5 100.00 (100.00)
Epoch: [0][ 40/625]	Time  0.118 ( 0.116)	Data  0.000 ( 0.010)	Loss 5.4494e+00 (1.2934e+01)	Acc@1  50.00 ( 50.30)	Acc@5 100.00 (100.00)
Epoch: [0][ 50/625]	Time  0.080 ( 0.111)	Data  0.000 ( 0.010)	Loss 1.8903e+01 (1.1359e+01)	Acc@1  50.00 ( 48.77)	Acc@5 100.00 (100.00)
Epoch: [0][ 60/625]	Time  0.082 ( 0.106)	Data  0.000 ( 0.009)	Loss 1.0540e+01 (1.0473e+01)	Acc@1  25.00 ( 49.39)	Acc@5 100.00 (100.00)
Epoch: [0][ 70/625]	Time  0.080 ( 0.102)	Data  0.000 ( 0.009)	Loss 5.1142e-01 (1.0354e+01)	Acc@1  75.00 ( 49.65)	Acc@5 100.00 (100.00)
Epoch: [0][ 80/625]	Time  0.076 ( 0.100)	Data  0.000 ( 0.009)	Loss 6.7064e-01 (9.2385e+00)	Acc@1  50.00 ( 49.38)	Acc@5 100.00 (100.00)
Epoch: [0][ 90/625]	Time  0.083 ( 0.098)	Data  0.000 ( 0.008)	Loss 7.3421e+00 (8.4755e+00)	Acc@1  37.50 ( 50.00)	Acc@5 100.00 (100.00)
Epoch: [0][100/625]	Time  0.093 ( 0.097)	Data  0.000 ( 0.008)	Loss 7.4379e-01 (7.8715e+00)	Acc@1  50.00 ( 50.12)	Acc@5 100.00 (100.00)
```


要随时停止训练，您可以按 `Ctrl+C`。  您还可以稍后使用 `--resume` 和 `--epoch-start` 标志再次重新启动训练，因此您无需等待训练完成即可测试模型。


运行 `python3 train.py --help` 以获取有关可供您使用的每个选项的更多信息，包括您可以使用 `--arch` 标志尝试的其他网络。


### 训练指标


上述训练过程中输出的统计信息对应如下信息: 


* Epoch: 一个epoch是对数据集的一次完整训练

	* `Epoch: [N]` 表示您当前处于纪元 0、1、2 等。

	* 默认运行 35 个 epoch（您可以使用 `--epochs=N` 标志更改此设置）

* `[N/625]` 是您所在纪元的当前图像批次

	* 训练图像以小批量方式处理以提高性能

	* 默认批量大小为 8 张图像，可以使用 `--batch=N` 标志设置

	* 将括号中的数字乘以批次大小（例如批次 `[100/625]` -> 图像 `[800/5000]`）

* Time: 当前图像批次的处理时间（以秒为单位）

* data: 当前图像批次的磁盘加载时间（以秒为单位）

* 损失: 模型产生的累积误差（预期与预测）

* `Acc@1`: 该批次的 Top-1 分类精度

	* Top-1，意味着模型准确预测了正确的类别

* `Acc@5`: 该批次的 Top-5 分类准确率

	* Top-5，意味着正确的类别是模型预测的前 5 个输出之一

	* 由于此猫/狗示例只有 2 个类别（猫和狗），因此 Top-5 始终为 100%

	* 本教程中的其他数据集具有超过 5 个类别，其中 Top-5 有效


您可以在训练期间密切关注这些统计数据，以衡量模型的训练效果，以及是否要继续或停止并进行测试。  如上所述，如果您愿意，您可以稍后重新开始训练。


### 模型精度


在这个包含 5000 张图像的数据集上，在 Jetson Nano 上每个 epoch 训练 ResNet-18 大约需要 7-8 分钟，或者将模型训练到 35 个 epoch 和 80% 分类准确率大约需要 4 小时。  下面是用于分析 epoch 的训练进度与模型准确性的图表: 


<palign="center"><img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-cat-dog-training.jpg" width="700"></p>


在 epoch 30 左右，ResNet-18 模型达到 80% 的准确率，在 epoch 65 时收敛到 82.5% 的准确率。  通过额外的训练时间，您可以通过增加数据集的大小（请参阅下面的[生成更多数据](#generating-more-data-optional) 部分）或尝试更复杂的模型来进一步提高准确性。


默认情况下，训练脚本设置为运行 35 个 epoch，但如果您不希望等待那么长时间来测试模型，您可以提前退出训练并继续下一步（可选择稍后从上次中断的位置重新开始训练）。  您还可以从此处下载经过完整 100 轮训练的完整模型: 


* <a href="https://nvidia.box.com/s/zlvb4y43djygotpjn6azjhwu0r3j0yxc">https://nvidia.box.com/s/zlvb4y43djygotpjn6azjhwu0r3j0yxc</a>


请注意，模型保存在 `jetson-inference/python/training/classification/models/cat_dog/` 下，包括最新纪元的检查点和具有最高分类精度的最佳性能模型。  此 `classification/models` 目录会自动[安装到容器中](aux-docker-CN.md#mounted-data-volumes)，因此您训练的模型将在容器关闭后保留。


## 将模型转换为 ONNX


要使用 TensorRT 运行重新训练的 ResNet-18 模型进行测试和实时推理，首先我们需要将 PyTorch 模型转换为 <a href="https://onnx.ai/">ONNX 格式</a>，以便 TensorRT 可以加载它。ONNX 是一种开放模型格式，支持许多流行的 ML 框架，包括 PyTorch、TensorFlow、TensorRT 等，因此它简化了工具之间的模型传输。


PyTorch 内置支持将 PyTorch 模型导出到 ONNX，因此请运行以下命令，使用提供的 `onnx_export.py` 脚本转换我们的 Cat/Dog 模型: 


``` bash
python3 onnx_export.py --model-dir=models/cat_dog
```


这将在 `jetson-inference/python/training/classification/models/cat_dog/` 下创建一个名为 `resnet18.onnx` 的模型


## 使用 TensorRT 处理图像


为了对一些静态测试图像进​​行分类，我们将使用 `imagenet` 的扩展命令行参数来加载我们在上面重新训练的自定义 ResNet-18 模型。  要运行这些命令，终端的工作目录仍应位于: `jetson-inference/python/training/classification/`


````bash
NET=模型/cat_dog
数据集=数据/cat_dog


# C++
imagenet --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/labels.txt $DATASET/test/cat/01.jpg cat.jpg


# Python
imagenet.py --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/labels.txt $DATASET/test/cat/01.jpg cat.jpg
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-cat.jpg">


````bash
# C++
imagenet --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/labels.txt $DATASET/test/dog/01.jpg 狗.jpg


# Python
imagenet.py --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/labels.txt $DATASET/test/dog/01.jpg 狗.jpg
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-dog.jpg">


### 处理所有测试图像


数据集中包含猫类和狗类之间的 200 张测试图像，或者您可以下载自己的图片进行尝试。  您可以像这样处理它们: 


`` 重击
mkdir $DATASET/test_output_cat $DATASET/test_output_dog


imagenet --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/../labels.txt \
           $DATASET/test/cat $DATASET/test_output_cat


imagenet --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/../labels.txt \
           $DATASET/测试/狗 $DATASET/test_output_dog
````


在这种情况下，将从数据集的 `test/` 目录中读取所有图像，并将其保存到 `test_output/` 目录中。


有关加载/保存图像序列的详细信息，请参阅[相机流和多媒体](aux-streaming-CN.md#sequences) 页面。


接下来，我们将尝试在实时摄像头输入上运行重新训练的模型。


## 运行实时摄像头程序


如果你家里有毛茸茸的朋友，你可以运行相机程序，看看它是如何工作的！  与上一步一样，`imagenet` 支持扩展命令行参数来加载自定义模型: 


````bash
# C++ (MIPI CSI)
imagenet --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/labels.txt csi://0


# Python (MIPI CSI)
imagenet.py --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/labels.txt csi://0
````

> **注意: ** 有关支持的视频流和协议的信息，请参阅[相机流和多媒体](aux-streaming-CN.md) 页面。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-otto.jpg" width="500">


## 生成更多数据（可选）


Cat/Dog 数据集中的图像是使用 [`cat-dog-dataset.sh`](../tools/cat-dog-dataset.sh) 脚本从更大的 22.5GB <a href="https://drive.google.com/open?id=1LsxHT9HX5gM2wMVqPUfILgrqVlGtqX1o">subset of ILSCRV12</a> 中随机提取的。第一个 Cat/Dog 数据集故意保持较小，以减少训练时间，但通过使用此脚本，您可以使用其他图像重新生成它，以创建更强大的模型。


较大的数据集需要更多时间来训练，因此您可以暂时继续[下一个示例](pytorch-plants-CN.md)，但如果您想要扩展 Cat/Dog 数据集，请首先从此处下载源数据: 


* <a href="https://drive.google.com/open?id=1LsxHT9HX5gM2wMVqPUfILgrqVlGtqX1o">https://drive.google.com/open?id=1LsxHT9HX5gM2wMVqPUfILgrqVlGtqX1o</a>


解压此存档后，编辑 [`tools/cat-dog-dataset.sh`](../tools/cat-dog-dataset.sh) 并进行以下修改: 


* 将提取的 `ilsvrc12_subset` 的位置替换到 `IMAGENET_DIR` 变量中

* 然后在某处为 cat_dog 创建一个空文件夹，并将该位置替换为 `OUTPUT_DIR`

* 通过修改 `NUM_TRAIN`、`NUM_VAL` 和 `NUM_TEST` 变量来更改数据集的大小


该脚本在 `OUTPUT_DIR` 下为 train、val 和 test 创建子目录，然后为每个目录填充指定数量的图像。  然后，您可以按照与上述相同的方式[训练模型](#re-training-resnet-18-model)，可以选择使用 `--resume` 和 `--epoch-start` 标志从您上次中断的地方继续训练（如果您不想从头开始训练）。  请记住在重新训练后将模型重新导出到 ONNX。


在下面的示例中，我们将在支持 20 个对象类的植物和树木数据集上训练另一个模型。


<palign="right">下一个| <b><a href="pytorch-plants-CN.md">在 PlantCLEF 数据集上重新训练</a></b>
<br/>
返回 | <b><a href="pytorch-transfer-learning-CN.md">使用 PyTorch 进行迁移学习</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>