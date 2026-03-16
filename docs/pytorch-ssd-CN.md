<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="pytorch-collect-CN.md">返回</a> | <a href="pytorch-collect-detection-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>迁移学习 - 目标检测</sup></s></p>


# 重新训练SSD-Mobilenet


接下来，我们将使用 PyTorch 和 [Open Images](https://storage.googleapis.com/openimages/web/visualizer/index.html?set=train&type=detection&c=%2Fm%2F06l9r) 数据集训练我们自己的 SSD-Mobilenet 目标检测模型。  SSD-Mobilenet 是一种流行的网络架构，用于移动和嵌入式设备上的实时目标检测，它将 [SSD-300](https://arxiv.org/abs/1512.02325) Single-Shot MultiBox 检测器与 [Mobilenet](https://arxiv.org/abs/1704.04861) 主干网相结合。


<a href="https://arxiv.org/abs/1512.02325"><img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-ssd-mobilenet.jpg"></a>


在下面的示例中，我们将训练一个自定义检测模型来定位 8 种不同的水果品种，但欢迎您从 Open Images 数据集中的 [600 个类别](https://github.com/dusty-nv/pytorch-ssd/blob/master/open_images_classes.txt) 中进行选择来训练您的模型。  您可以在[此处](https://storage.googleapis.com/openimages/web/visualizer/index.html?set=train&type=detection)直观地浏览数据集。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-fruit.jpg">


首先，请确保您的 Jetson 上安装了适用于 **Python 3** 的 [JetPack 4.4](https://developer.nvidia.com/embedded/jetpack)（或更高版本）和 [PyTorch displayed](pytorch-transfer-learning-CN.md#installing-pytorch)。  JetPack 4.4 包含 TensorRT 7.1，这是支持通过 ONNX 加载 SSD-Mobilenet 的最低 TensorRT 版本。  新版本的 TensorRT 也很好。


## 设置


用于训练 SSD-Mobilenet 的 PyTorch 代码可在 [`jetson-inference/python/training/detection/ssd`](https://github.com/dusty-nv/pytorch-ssd) 下的存储库中找到。  如果您没有[运行 Docker 容器](aux-docker-CN.md)，则在使用它之前需要执行几个步骤: 


```bash
# you only need to run these if you aren't using the container
$ cd jetson-inference/python/training/detection/ssd
$ wget https://nvidia.box.com/shared/static/djf5w54rjvpqocsiztzaandq1m3avr7c.pth -O models/mobilenet-v1-ssd-mp-0_675.pth
$ pip3 install -v -r requirements.txt
```


> **注意: ** 首先确保您的 Jetson 上安装了 [JetPack 4.4](https://developer.nvidia.com/embedded/jetpack) 或更高版本，并且 [安装了 PyTorch](pytorch-transfer-learning-CN.md#installing-pytorch) 适用于 **Python 3**


这会将 [基本模型](https://nvidia.box.com/shared/static/djf5w54rjvpqocsiztzaandq1m3avr7c.pth) 下载到 `ssd/models` 并安装一些必需的 Python 包（这些包已安装到容器中）。  基本模型已经在不同的数据集（PASCAL VOC）上进行了预训练，因此我们不需要从头开始训练 SSD-Mobilenet，这会花费更长的时间。  相反，我们将使用迁移学习对其进行微调以检测我们选择的新对象类。


## 下载数据


[打开图像](https://storage.googleapis.com/openimages/web/visualizer/index.html?set=train&type=detection&c=%2Fm%2F0fp6w) 数据集包含超过 [600 个对象类](https://github.com/dusty-nv/pytorch-ssd/blob/master/open_images_classes.txt)，您可以从中挑选。  提供了一个名为 `open_images_downloader.py` 的脚本，它将自动为您下载所需的对象类。


> **注意: ** 使用的类越少，模型在推理过程中运行得越快。  打开图像还可以包含数百 GB 的数据，具体取决于您选择的类 - 因此在下载您自己的类之前，请参阅下面的[限制数据量](#limiting-the-amount-of-data) 部分。


我们将使用的类是 `"Apple,Orange,Banana,Strawberry,Grape,Pear,Pineapple,Watermelon"`，例如水果采摘机器人 - 尽管欢迎您从[类列表](https://github.com/dusty-nv/pytorch-ssd/blob/master/open_images_classes.txt) 中替换您自己的选择。水果类有大约 6500 张图像，这是一个快乐的媒介。


```bash
$ python3 open_images_downloader.py --class-names "Apple,Orange,Banana,Strawberry,Grape,Pear,Pineapple,Watermelon" --data=data/fruit
...
2020-07-09 16:20:42 - Starting to download 6360 images.
2020-07-09 16:20:42 - Downloaded 100 images.
2020-07-09 16:20:42 - Downloaded 200 images.
2020-07-09 16:20:42 - Downloaded 300 images.
2020-07-09 16:20:42 - Downloaded 400 images.
2020-07-09 16:20:42 - Downloaded 500 images.
2020-07-09 16:20:46 - Downloaded 600 images.
...
2020-07-09 16:32:12 - Task Done.
```


默认情况下，数据集将下载到 `jetson-inference/python/training/detection/ssd` 下的 `data/` 目录（自动[安装到容器中](aux-docker-CN.md#mounted-data-volumes)），但您可以通过指定 `--data=<PATH>` 选项来更改它。  根据数据集的大小，可能需要使用外部存储。  如果您下载多个数据集，则应将每个数据集存储在各自的子目录中。


### 限制数据量


根据您选择的类，Open Images 可能包含大量数据 - 在某些情况下，数据太多，无法在合理的时间内完成我们的训练。  特别是包含人和车辆的类具有非常大量的图像（>250GB）。


因此，在选择您自己的类时，在下载数据之前，建议首先使用 `--stats-only` 选项运行下载器脚本。  这将显示您的班级有多少图像，而无需实际下载任何图像。


`` 重击
$ python3 open_images_downloader.py --stats-only --class-names "苹果、橙子、香蕉、草莓、葡萄、梨、菠萝、西瓜" --data=data/fruit
...
2020-07-09 16:18:06 - 可用图像总数: 6360
2020-07-09 16:18:06 - 可用盒子总数: 27188


------------------------------------------------
 “训练”集统计
------------------------------------------------
  图片数量: 5145
  边界框数量: 23539
  边界框分布: 
    草莓: 7553/23539 = 0.32
    橙色: 6186/23539 = 0.26
    苹果: 3622/23539 = 0.15
    葡萄: 2560/23539 = 0.11
    香蕉: 1574/23539 = 0.07
    梨: 757/23539 = 0.03
    西瓜: 753/23539 = 0.03
    菠萝: 534/23539 = 0.02


...


------------------------------------------------
 总体统计
------------------------------------------------
  图片数量: 6360
  边界框数量: 27188
````


> **注意: ** `--stats-only` 确实下载了注释数据（大约 1GB），但还没有下载图像。


在实践中，为了减少训练时间（和磁盘空间），您可能希望将图像总数保持在 <10K 以内。  尽管您使用的图像越多，您的模型就越准确。  您可以使用 `--max-images` 选项或 `--max-annotations-per-class` 选项限制下载的数据量: 


* `--max-images` 将总数据集限制为指定的图像数量，同时保持每个类的图像分布与原始数据集大致相同。  如果一个类比另一类拥有更多图像，则比率将保持大致相同。

* `--max-annotations-per-class` 将每个类限制为指定数量的边界框，如果某个类的可用边界框数量少于该数量，则将使用其所有数据 - 如果类之间的数据分布不平衡，这非常有用。


例如，如果您只想对水果数据集使用 2500 个图像，您可以像这样启动下载器: 


``` bash
$ python3 open_images_downloader.py --max-images=2500 --class-names "Apple,Orange,Banana,Strawberry,Grape,Pear,Pineapple,Watermelon" --data=data/fruit
```


如果未设置 `--max-boxes` 选项或 `--max-annotations-per-class` ，默认情况下将下载所有可用的数据 - 因此，请务必先使用 `--stats-only` 检查数据量。  遗憾的是，无法提前确定图像的实际磁盘大小要求，但此数据集的一般经验法则是每个图像预算约 350KB（水果约 2GB）。


### 训练表现


以下是 SSD-Mobilenet 的近似训练性能，可帮助估计训练所需的时间: 


|           |图片/秒 |每个时期的时间* |
|------------|:----------:|:----------------:|
|纳米|    4.77 | 4.77  17 分 55 秒 |
|泽维尔 NX |    14.65 | 14.65   5 分 50 秒 |


* 在水果数据集上测量（5145 个训练图像，批量大小 4）


## 训练 SSD-Mobilenet 模型


数据下载完成后，运行 `train_ssd.py` 脚本来启动训练: 


```bash
python3 train_ssd.py --data=data/fruit --model-dir=models/fruit --batch-size=4 --epochs=30
```


> **注意: ** 如果内存不足或进程在训练期间被“终止”，请尝试[安装交换](pytorch-transfer-learning-CN.md#mounting-swap) 和[禁用桌面 GUI](pytorch-transfer-learning-CN.md#disabling-the-desktop-gui)。 <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;为了节省内存，您还可以减少 `--batch-size` （默认 4）和 `--workers` （默认 2）


以下是您可以用来运行训练脚本的一些常见选项: 


|论证|  默认 |描述 |
|----------------|:---------:|------------------------------------------------------------------------|
| `--data` |  `data/` |数据集的位置 |
| `--model-dir` | `models/` |输出训练后的模型检查点的目录 |
| `--resume` |    无 |到现有检查点的路径以从 | 恢复训练
| `--batch-size` |     4 |尝试根据可用内存增加 |
| `--epochs` |     30|最多 100 是理想的，但会增加训练时间 |
| `--workers` |     2 |数据加载器线程数（0 = 禁用多线程）|


随着时间的推移，您应该会看到损失减少: 


```bash
2020-07-10 13:14:12 - Epoch: 0, Step: 10/1287, Avg Loss: 12.4240, Avg Regression Loss 3.5747, Avg Classification Loss: 8.8493
2020-07-10 13:14:12 - Epoch: 0, Step: 20/1287, Avg Loss: 9.6947, Avg Regression Loss 4.1911, Avg Classification Loss: 5.5036
2020-07-10 13:14:13 - Epoch: 0, Step: 30/1287, Avg Loss: 8.7409, Avg Regression Loss 3.4078, Avg Classification Loss: 5.3332
2020-07-10 13:14:13 - Epoch: 0, Step: 40/1287, Avg Loss: 7.3736, Avg Regression Loss 2.5356, Avg Classification Loss: 4.8379
2020-07-10 13:14:14 - Epoch: 0, Step: 50/1287, Avg Loss: 6.3461, Avg Regression Loss 2.2286, Avg Classification Loss: 4.1175
...
2020-07-10 13:19:26 - Epoch: 0, Validation Loss: 5.6730, Validation Regression Loss 1.7096, Validation Classification Loss: 3.9634
2020-07-10 13:19:26 - Saved model models/fruit/mb1-ssd-Epoch-0-Loss-5.672993580500285.pth
```


要在全部 epoch 完成训练之前测试您的模型，您可以按 `Ctrl+C` 终止训练脚本，并稍后使用 `--resume=<CHECKPOINT>` 参数再次恢复它。  您可以在[此处](https://nvidia.box.com/shared/static/gq0zlf0g2r258g3ldabl9o7vch18cxmi.gz)下载已经训练了 100 个 epoch 的水果模型。


## 将模型转换为 ONNX


接下来，我们需要将训练好的模型从 PyTorch 转换为 ONNX，以便我们可以使用 TensorRT 加载它: 


``` bash
python3 onnx_export.py --model-dir=models/fruit
```


这将在 `jetson-inference/python/training/detection/ssd/models/fruit/` 下保存一个名为 `ssd-mobilenet.onnx` 的模型


## 使用 TensorRT 处理图像


为了对一些静态测试图像进​​行分类，我们将使用 `detectnet` （或 `detectnet.py`）的扩展命令行参数来加载我们的自定义 SSD-Mobilenet ONNX 模型。  要运行这些命令，终端的工作目录仍应位于: `jetson-inference/python/training/detection/ssd/`


````bash
IMAGES=<path-to-your-jetson-inference>/data/images # 在此替换您的 jetson-inference 路径


detectornet --model=models/fruit/ssd-mobilenet.onnx --labels=models/fruit/labels.txt \
          --input-blob=input_0 --output-cvg=分数 --output-bbox=盒子 \
            “$IMAGES/fruit_*.jpg”$IMAGES/test/fruit_%i.jpg
````


> **注意: ** 可以替换上面的 `detectnet.py` 来运行 Python 版本的程序


以下是输出到 `$IMAGES/test` 目录的一些图像: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-fruit-2.jpg">


## 运行实时摄像头程序


您还可以尝试在摄像机或视频流上运行重新训练的植物模型，如下所示: 


```bash
detectnet --model=models/fruit/ssd-mobilenet.onnx --labels=models/fruit/labels.txt \
          --input-blob=input_0 --output-cvg=scores --output-bbox=boxes \
            csi://0
```


有关其他摄像头/视频源的更多详细信息，请参阅[摄像头流媒体和多媒体](aux-streaming-CN.md)。


<palign="right">下一个| <b><a href="pytorch-collect-detection-CN.md">收集您自己的检测数据集</a></b>
<br/>
返回 | <b><a href="pytorch-collect-CN.md">收集您自己的分类数据集</a></p>
</b><palign="center"><sup>© 2016-2020 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>