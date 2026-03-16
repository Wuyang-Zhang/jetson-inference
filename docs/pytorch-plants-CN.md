<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="pytorch-cat-dog-CN.md">返回</a> | <a href="pytorch-collect-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>迁移学习 - 分类</sup></s></p>


# PlantCLEF 数据集的重新训练


接下来，我们将训练一个能够对 <a href="https://www.imageclef.org/lifeclef/2017/plant">PlantCLEF</a> 数据集中的 20 种不同植物和树木进行分类的模型。


<a href="https://www.imageclef.org/lifeclef/2017/plant"><img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-plants.jpg"></a>


下面提供了一个 1.5GB 的子集，其中包括 20 个类别的植物和树木的 10,475 个训练图像、1,155 个验证图像和 30 个测试图像。  这些类是从 PlantCLEF 2017 中从原始数据集中至少有 500 个训练图像的类别中选择的: 


```
• ash
• beech
• cat-tail
• cedar
• clover
• cyprus
• daisy
• dandelion
• dogwood
• elm
• fern
• fig
• fir
• juniper
• maple
• poison ivy
• sweetgum
• sycamore
• trout lily
• tulip tree
```


首先，请确保您在 Jetson 上安装了 [PyTorch](pytorch-transfer-learning-CN.md#installing-pytorch)，然后下载下面的数据集并启动训练脚本。  之后，我们将在 TensorRT 中使用一些静态图像和实时摄像头测试重新训练的模型。


## 下载数据


运行以下命令来下载并提取准备好的 PlantCLEF 数据集: 


``` bash
$ cd jetson-inference/python/training/classification/data
$ wget https://nvidia.box.com/shared/static/vbsywpw5iqy7r38j78xs0ctalg7jrg79.gz -O PlantCLEF_Subset.tar.gz
$ tar xvzf PlantCLEF_Subset.tar.gz
```


数据集的镜像可在此处获得: 


* <a href="https://drive.google.com/file/d/14pUv-ZLHtRR-zCYjznr78mytFcnuR_1D/view?usp=sharing">https://drive.google.com/file/d/14pUv-ZLHtRR-zCYjznr78mytFcnuR_1D/view?usp=sharing</a>

* <a href="https://nvidia.box.com/s/vbsywpw5iqy7r38j78xs0ctalg7jrg79">https://nvidia.box.com/s/vbsywpw5iqy7r38j78xs0ctalg7jrg79</a>


## 重新训练 ResNet-18 模型


我们将使用与上一个示例相同的训练脚本，位于 <a href="https://github.com/dusty-nv/jetson-inference/tree/master/python/training/classification">`python/training/classification/`</a>. 下。默认情况下，它设置为训练 ResNet-18 模型，但您可以使用 `--arch` 标志更改它。


要启动训练，请运行以下命令: 


``` bash
$ cd jetson-inference/python/training/classification
$ python3 train.py --model-dir=models/plants data/PlantCLEF_Subset
```


> **注意: ** 如果内存不足或进程在训练期间被“终止”，请尝试[安装交换](pytorch-transfer-learning-CN.md#mounting-swap) 和[禁用桌面 GUI](pytorch-transfer-learning-CN.md#disabling-the-desktop-gui)。 <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;为了节省内存，您还可以减少 `--batch-size` （默认 8）和 `--workers` （默认 2）


训练开始时，您应该在控制台中看到如下所示的文本: 


``` bash
Use GPU: 0 for training
=> dataset classes:  20 ['ash', 'beech', 'cattail', 'cedar', 'clover', 'cyprus', 'daisy', 'dandelion', 'dogwood', 'elm', 'fern', 'fig', 'fir', 'juniper', 'maple', 'poison_ivy', 'sweetgum', 'sycamore', 'trout_lily', 'tulip_tree']
=> using pre-trained model 'resnet18'
=> reshaped ResNet fully-connected layer with: Linear(in_features=512, out_features=20, bias=True)
Epoch: [0][   0/1307]	Time 49.345 (49.345)	Data  0.561 ( 0.561)	Loss 3.2172e+00 (3.2172e+00)	Acc@1   0.00 (  0.00)	Acc@5  25.00 ( 25.00)
Epoch: [0][  10/1307]	Time  0.779 ( 5.211)	Data  0.000 ( 0.060)	Loss 2.3915e+01 (1.5221e+01)	Acc@1   0.00 (  5.68)	Acc@5  12.50 ( 27.27)
Epoch: [0][  20/1307]	Time  0.765 ( 3.096)	Data  0.000 ( 0.053)	Loss 3.6293e+01 (2.1256e+01)	Acc@1   0.00 (  5.95)	Acc@5  37.50 ( 27.38)
Epoch: [0][  30/1307]	Time  0.773 ( 2.346)	Data  0.000 ( 0.051)	Loss 2.8803e+00 (1.9256e+01)	Acc@1  37.50 (  6.85)	Acc@5  62.50 ( 27.42)
Epoch: [0][  40/1307]	Time  0.774 ( 1.962)	Data  0.000 ( 0.050)	Loss 3.7734e+00 (1.5865e+01)	Acc@1  12.50 (  8.84)	Acc@5  37.50 ( 29.88)
Epoch: [0][  50/1307]	Time  0.772 ( 1.731)	Data  0.000 ( 0.049)	Loss 3.0311e+00 (1.3756e+01)	Acc@1  25.00 ( 10.29)	Acc@5  37.50 ( 32.35)
Epoch: [0][  60/1307]	Time  0.773 ( 1.574)	Data  0.000 ( 0.048)	Loss 3.2433e+00 (1.2093e+01)	Acc@1   0.00 (  9.84)	Acc@5  25.00 ( 32.79)
Epoch: [0][  70/1307]	Time  0.806 ( 1.462)	Data  0.000 ( 0.048)	Loss 2.9213e+00 (1.0843e+01)	Acc@1  12.50 (  8.98)	Acc@5  37.50 ( 33.27)
Epoch: [0][  80/1307]	Time  0.792 ( 1.379)	Data  0.000 ( 0.048)	Loss 3.2370e+00 (9.8715e+00)	Acc@1   0.00 (  9.26)	Acc@5  25.00 ( 34.41)
Epoch: [0][  90/1307]	Time  0.770 ( 1.314)	Data  0.000 ( 0.048)	Loss 2.4494e+00 (9.0905e+00)	Acc@1  25.00 (  9.75)	Acc@5  75.00 ( 36.26)
Epoch: [0][ 100/1307]	Time  0.801 ( 1.261)	Data  0.001 ( 0.048)	Loss 2.6449e+00 (8.4769e+00)	Acc@1  25.00 ( 10.40)	Acc@5  62.50 ( 37.00)
```


请参阅上一页中的[训练指标](pytorch-cat-dog-CN.md#training-metrics)，了解上面输出的统计信息的说明。


### 模型精度


在包含 10,475 张图像的 PlantCLEF 数据集上，在 Jetson Nano 上训练 ResNet-18 每个周期大约需要 15 分钟，或者训练模型 35 个周期大约需要 8 小时。  下面是用于分析 epoch 的训练进度与模型准确性的图表: 


<palign="center"><img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-plants-training.jpg" width="700"></p>


在 epoch 30 左右，ResNet-18 模型达到 75% Top-5 准确率，在 epoch 65 时收敛到 85% Top-5 准确率。  有趣的是，模型的这些稳定性和收敛点在 ResNet-18 中出现的时间与之前的 Cat/Dog 模型的出现时间相似。  该模型的 Top-1 准确度为 55%，考虑到 PlantCLEF 数据集内容的多样性和挑战性（即每个图像有多个重叠的植物品种以及许多彼此几乎无法区分的叶子和树干图片），我们会发现这在实践中非常有效。


默认情况下，训练脚本设置为运行 35 个 epoch，但如果您不希望等待那么长时间来测试模型，您可以提前退出训练并继续下一步（可选择稍后从上次中断的位置重新开始训练）。  您还可以从此处下载经过完整 100 轮训练的完整模型: 


* <a href="https://nvidia.box.com/s/dslt9b0hqq7u71o6mzvy07w0onn0tw66">https://nvidia.box.com/s/dslt9b0hqq7u71o6mzvy07w0onn0tw66</a>


请注意，模型保存在 `jetson-inference/python/training/classification/data/plants/` 下，包括最新纪元的检查点和具有最高分类精度的最佳性能模型。  您可以通过更改 `--model-dir` 标志来更改保存模型的目录。


## 将模型转换为 ONNX


就像猫/狗的例子一样，接下来我们需要将训练好的模型从 PyTorch 转换为 ONNX，以便我们可以使用 TensorRT 加载它: 


``` bash
python3 onnx_export.py --model-dir=models/plants
```


这将在 `jetson-inference/python/training/classification/models/plants/` 下创建一个名为 `resnet18.onnx` 的模型


## 使用 TensorRT 处理图像


为了对一些静态测试图像进​​行分类，像之前一样，我们将使用 `imagenet` 的扩展命令行参数来加载我们上面重新训练的自定义 ResNet-18 模型。  要运行这些命令，终端的工作目录仍应位于: `jetson-inference/python/training/classification/`


````bash
NET=模型/植物
数据集=数据/PlantCLEF_Subset


# C++
imagenet --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/labels.txt $DATASET/test/cattail.jpg cattail.jpg


# Python
imagenet.py --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/labels.txt $DATASET/test/cattail.jpg cattail.jpg
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-plants-cattail.jpg" width="500">


````bash
# C++
imagenet --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/labels.txt $DATASET/test/elm.jpg elm.jpg


# Python
imagenet.py --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/labels.txt $DATASET/test/elm.jpg elm.jpg
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-plants-elm.jpg" width="500">


````bash
# C++
imagenet --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/labels.txt $DATASET/test/juniper.jpg juniper.jpg


# Python
imagenet.py --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/labels.txt $DATASET/test/juniper.jpg juniper.jpg
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-plants-juniper.jpg" width="500">


数据集包含大量测试图像，或者您可以下载自己的图片来尝试。


### 处理所有测试图像


如果您想一次对所有测试图像进​​行分类，您可以在整个目录上运行该程序: 


`` 重击
mkdir $DATASET/test_output


imagenet --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/../labels.txt \
           $DATASET/测试 $DATASET/test_output
````


在这种情况下，将从数据集的 `test/` 目录中读取所有图像，并将其保存到 `test_output/` 目录中。


有关加载/保存图像序列的详细信息，请参阅[相机流和多媒体](aux-streaming-CN.md#sequences) 页面。


## 运行实时摄像头程序


您还可以尝试在实时摄像机流上运行重新训练的植物模型，如下所示: 


````bash
# C++ (MIPI CSI)
imagenet --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/labels.txt csi://0


# Python (MIPI CSI)
imagenet.py --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/labels.txt csi://0
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-plants-fern.jpg" width="500">


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-plants-poison-ivy.jpg" width="500">


看来我应该提防毒葛了！


接下来，我们将介绍一个基于摄像头的工具，用于收集和标记从实时视频捕获的您自己的数据集。


<palign="right">下一个| <b><a href="pytorch-collect-CN.md">收集您自己的分类数据集</a></b>
<br/>
返回 | <b><a href="pytorch-cat-dog-CN.md">猫/狗数据集的重新训练</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>