<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="imagenet-custom-CN.md">返回</a> | <a href="detectnet-snapshot-CN.md">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>物体检测</sup></p>


# 使用 DetectNet 定位对象
前面的图像识别示例输出代表整个输入图像的类概率。   我们在本教程中强调的第二个深度学习功能是检测对象，并查找这些对象在视频中的位置（即提取它们的边界框）。  这是使用“DetectNet”或目标检测/定位网络来执行的。


[`detectNet`](../c/detectNet.h) 对象接受 2D 图像作为输入，并输出检测到的边界框的坐标列表。  为了训练目标检测模型，首先使用预训练的 ImageNet 识别模型（如 Googlenet），除了源图像之外，还使用训练数据集中包含的边界坐标标签。


本教程包含以下预训练的 DetectNet 模型: 


1. **ped-100**（单级行人探测器）

2. **multiped-500**（多级行人+行李探测器）

3. **facenet-120**（单类面部识别检测器）

4. **coco-airplane**（MS COCO 飞机级别）

5. **coco-bottle**（MS COCO 瓶类）

6. **coco-chair**（MS COCO 椅类）

7. **coco-dog**（MS COCO 狗类）


与前面的示例一样，提供了用于使用 detectorNet 的控制台程序和摄像头流程序。



### 检测数据的数字格式


示例目标检测数据集包括 [KITTI](http://www.cvlibs.net/datasets/kitti/eval_object.php)、[MS-COCO](http://mscoco.org/) 等。  要使用 KITTI 数据集，请遵循此 [KITTI DIGITS 目标检测教程](https://github.com/NVIDIA/DIGITS/blob/digits-4.0/digits/extensions/data/objectDetection/README.md)。


无论数据集如何，DIGITS 都使用 KITTI 元数据格式来摄取检测边界标签。  这些由文本文件组成，帧号与图像文件名相对应，包括以下内容: 


```
dog 0 0 0 528.63 315.22 569.09 354.18 0 0 0 0 0 0 0
sheep 0 0 0 235.28 300.59 270.52 346.55 0 0 0 0 0 0 0
```


[阅读更多](https://github.com/NVIDIA/DIGITS/blob/digits-4.0/digits/extensions/data/objectDetection/README.md) 有关 DIGITS 使用的文件夹结构和 KITTI 标签格式的信息。


### 下载检测数据集


让我们探索如何使用 [MS-COCO](http://mscoco.org/) 数据集来训练和部署网络，以检测摄像机源中日常物体的位置。  请参阅 [`coco2kitti.py`](../tools/coco2kitti.py) 脚本将 MS-COCO 对象类转换为 KITTI 格式。  一旦进入 DIGITS 文件夹结构，它们就可以作为数据集导入到 DIGITS 中。  为了方便起见，提供了一些来自 MS-COCO 的示例类，这些类已经以 DIGITS/KITTI 格式进行了预处理。


从 DIGITS 服务器上的终端下载并提取 **[示例 MS-COCO 类](https://nvidia.box.com/shared/static/tdrvaw3fd2cwst2zu2jsi0u43vzk8ecu.gz)** 已采用 DIGITS/KITTI 格式: 


````bash
$ wget --no-check-certificate https://nvidia.box.com/shared/static/tdrvaw3fd2cwst2zu2jsi0u43vzk8ecu.gz -O coco.tar.gz


HTTP 请求已发送，正在等待响应... 200 OK
长度: 5140413391 (4.5G) [应用程序/八位字节流]
保存到: ‘coco.tar.gz’


可可 100%[========================================>] 4.5G 3.33MB/s 28m 22s


2017-04-17 10:41:19 (2.5 MB/s) - ‘coco.tar.gz’已保存 [5140413391/5140413391]


$ tar -xzvf coco.tar.gz 
````


其中包括飞机、瓶子、椅子和狗类的数字格式的训练数据。  [`coco2kitti.py`](../tools/coco2kitti.py) 可用于转换其他类。


### 将检测数据集导入DIGITS


将浏览器导航到 DIGITS 服务器实例，然后从“数据集”选项卡的下拉列表中选择创建新的 `Detection Dataset`: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-digits-new-dataset-menu.png" width="250">


在表单字段中，指定以下选项以及提取航空数据集位置下的图像和标签文件夹的路径: 


* 训练图像文件夹: `coco/train/images/dog`

* 培训标签文件夹: `coco/train/labels/dog`

* 验证图像文件夹: `coco/val/images/dog`

* 验证标签文件夹: `coco/val/labels/dog`

* 焊盘图像（宽度 x 高度）: `640 x 640`

* 自定义类: `dontcare, dog`

* 群组名称: `MS-COCO`

* 数据集名称: `coco-dog`


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-digits-new-dataset-dog.png)


将数据集命名为您选择的名称，然后单击页面底部的 `Create` 按钮以启动导入作业。  接下来我们将创建新的检测模型并开始训练它。


### 使用 DIGITS 创建 DetectNet 模型


上一个数据导入作业完成后，返回 DIGITS 主屏幕。  选择 `Models` 选项卡，然后从下拉列表中选择创建新的 `Detection Model`: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-digits-new-model-menu.png" width="250">


在表单中进行如下设置: 


* 选择数据集: `coco-dog`

* 训练时期: `100`

* 减去平均值: `none`

* 解算器类型: `Adam`

* 基础学习率: `2.5e-05`

* 选择`Show advanced learning options`

  * 政策: `Exponential Decay`

  * 伽玛: `0.99`


#### 选择DetectNet批量大小


DetectNet 的网络默认批量大小为 10，在训练期间消耗高达 12GB GPU 内存。  但是，通过使用 `Batch Accumulation` 字段，您还可以在内存小于 12GB 的 GPU 上训练 DetectNet。  根据 DIGITS 服务器中可用的 GPU 内存量，请参阅下表: 


| GPU内存|批量大小 |批量积累|
| -------------- |:-------------------------------------:|:--------------------:|
| 4GB | 2 | 5 |
| 8GB | 5 | 2 |
| 12GB 或更大 | `[network defaults]` (10) |留空 (1) |


如果您在内存为 12GB 或更多的卡上进行训练，请将 `Batch Size` 保留为默认值，并将 `Batch Accumulation` 留空。  对于内存较少的 GPU，请使用上面的设置。


#### 指定 DetectNet Prototxt


在网络区域中选择 `Custom Network` 选项卡，然后复制/粘贴 [`detectnet.prototxt`](../data/networks/detectnet.prototxt) 的内容


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-digits-custom-network.jpg)


DetectNet 原型文本位于存储库中的 [`data/networks/detectnet.prototxt`](https://github.com/dusty-nv/jetson-inference/blob/master/data/networks/detectnet.prototxt)。


#### 使用预训练的 Googlenet 训练模型


由于 DetectNet 源自 Googlenet，因此强烈建议使用 Googlenet 中的预训练权重，因为这将有助于显着加快和稳定训练。  从[此处](http://dl.caffe.berkeleyvision.org/bvlc_googlenet.caffemodel) 下载 Googlenet 模型，或者从您的 DIGITS 服务器运行以下命令: 


```bash
wget http://dl.caffe.berkeleyvision.org/bvlc_googlenet.caffemodel
```


然后在 `Pretrained Model` 字段下指定 Googlenet 的路径。


选择要训练的 GPU 并设置模型的名称和组: 


* 群组名称 `MS-COCO`

* 型号名称 `DetectNet-COCO-Dog`


最后，单击底部的 `Create` 按钮开始训练。


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-digits-new-model-dog.png)


### 在 DIGITS 中测试 DetectNet 模型推理


让训练作业运行一段时间，比如 50 个 epoch，直到 mAP (`Mean Average Precision`) 图开始增加。  请注意，由于 DetectNet 损失函数计算 mAP 的方式，mAP 的范围不一定是 0-100，甚至 mAP 介于 5 和 10 之间也可能表明模型是有效的。  根据我们使用的示例 COCO 数据集的大小，在所有 100 个 epoch 完成之前，在最新的 GPU 上应该需要几个小时的训练时间。


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-digits-model-dog.png)


此时，我们可以尝试在 DIGITS 中的一些示例图像上测试新模型的推理。  在与上图相同的页面上，向下滚动到 `Trained Models` 部分。  将 `Visualization Model` 设置为*边界框*，并在 `Test a Single Image` 下选择要尝试的图像（例如 `/coco/val/images/dog/000074.png`）: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-digits-visualization-options-dog.png" width="350">


按 `Test One` 按钮，您应该看到类似以下内容的页面: 


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-digits-infer-dog.png)


接下来，我们将把 DetectNet 模型部署到 Jetson 并在那里运行推理。


##
<palign="right">下一个| <b><a href="detectnet-snapshot-CN.md">将检测模型下载到 Jetson</a></b>
<br/>
返回 | <b><a href="imagenet-custom-CN.md">在 Jetson 上加载自定义模型</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>