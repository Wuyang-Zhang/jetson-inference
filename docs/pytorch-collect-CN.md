<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="pytorch-plants-CN.md">返回</a> | <a href="pytorch-ssd-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>迁移学习 - 分类</sup></s></p>


# 收集您自己的分类数据集


为了收集您自己的数据集来训练自定义模型以对您选择的对象或场景进行分类，我们创建了一个名为 `camera-capture` 的易于使用的工具，用于在 Jetson 上从实时视频中捕获和标记图像: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-collection.jpg" >


该工具将在磁盘上创建具有以下目录结构的数据集: 


```
‣ train/
	• class-A/
	• class-B/
	• ...
‣ val/
	• class-A/
	• class-B/
	• ...
‣ test/
	• class-A/
	• class-B/
	• ...
```


其中 `class-A`、`class-B` 等。将是包含您在类标签文件中定义的每个对象类的数据的子目录。  这些类子目录的名称将与我们将在下面创建的类标签名称相匹配。  这些子目录将由工具自动填充标签文件中列出的类中的 `train`、`val` 和 `test` 集，并且每个子目录下将保存一系列 JPEG 图像。


请注意，上面是我们一直在使用的 PyTorch 训练脚本所期望的组织结构。  如果您检查 Cat/Dog 和 PlantCLEF 数据集，它们也以相同的方式组织。


## 创建标签文件


在 `jetson-inference/python/training/classification/data` 下，创建一个空目录用于存储数据集和一个用于定义类标签的文本文件（通常称为 `labels.txt`）。  标签文件每行包含一个类标签，并且按字母顺序排列（这一点很重要，因此标签文件中类的顺序与磁盘上相应子目录的顺序相匹配）。  如上所述，`camera-capture` 工具将自动从此标签文件中填充每个类所需的子目录。


下面是一个包含 5 个类的 `labels.txt` 文件示例: 


``` bash
background
brontosaurus
tree
triceratops
velociraptor
```


这是该工具将创建的相应目录结构: 


``` bash
‣ train/
	• background/
	• brontosaurus/
	• tree/
	• triceratops/
	• velociraptor/
‣ val/
	• background/
	• brontosaurus/
	• tree/
	• triceratops/
	• velociraptor/
‣ test/
	• background/
	• brontosaurus/
	• tree/
	• triceratops/
	• velociraptor/
```


如果您正在使用容器，则需要将数据集存储在如上所述的 [已安装目录](aux-docker-CN.md#mounted-data-volumes) 中，以便在容器关闭后保存它。


## 启动该工具


`camera-capture` 工具的源代码可以在 [`jetson-inference/tools/camera-capture/`](https://github.com/dusty-nv/camera-capture) 下找到，与存储库中的其他程序一样，它构建到 `aarch64/bin` 目录并安装在 `/usr/local/bin/` 下


`camera-capture` 工具在命令行上接受与[相机流和多媒体](aux-streaming-CN.md#sequences) 页面上找到的相同输入 URI。


以下是启动该工具的一些示例命令: 


``` bash
$ camera-capture csi://0       # using default MIPI CSI camera
$ camera-capture /dev/video0   # using V4L2 camera /dev/video0
```


> **注意**: 例如要使用的相机，请参阅 Jetson Wiki 的以下部分: <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Nano:&nbsp;&nbsp;[`https://eLinux.org/Jetson_Nano#Cameras`](https://elinux.org/Jetson_Nano#Cameras) <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Xavier: [`https://eLinux.org/Jetson_AGX_Xavier#Ecosystem_Products_.26_Cameras`](https://elinux.org/Jetson_AGX_Xavier#Ecosystem_Products_.26_Cameras) <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- TX1/TX2: 开发套件包括板载 MIPI CSI 传感器模块 (0V5693)<br/>


## 收集数据


下面是 `Data Capture Control` 窗口，它允许您选择所需的数据集路径并加载您在上面创建的类标签文件，然后提供用于选择当前对象类和您当前正在收集数据的训练/验证/测试集的选项: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-collection-widget.jpg" >


首先，打开数据集路径和类标签。  然后，该工具将创建上面讨论的数据集结构（除非这些子目录已经存在），并且您将看到对象标签填充在 `Current Class` 下拉列表中。  将 `Dataset Type` 保留为分类。


然后将相机定位在您当前在下拉列表中选择的对象或场景，并在准备好拍摄图像时单击 `Capture` 按钮（或按空格键）。  图像将保存在训练集、验证集或测试集的该类子目录下。  状态栏显示该类别下已保存的图像数量。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-capture-brontosaurus.gif" >


建议在尝试训练之前为每个类别收集至少 100 个训练图像。  验证集的经验法则是，它应该约为训练集大小的 10-20%，而测试集的大小仅取决于要测试的静态图像数量。  如果您愿意，您也可以仅运行相机来测试您的模型。


重要的是，您的数据是从不同的物体方向、相机视角、照明条件收集的，最好是在不同的背景下收集，以创建一个对噪声和环境变化具有鲁棒性的模型。  如果您发现模型的性能没有达到您想要的效果，请尝试添加更多训练数据并调整条件。



## 训练你的模型


当您收集了一堆数据后，您可以尝试在其上训练模型，就像我们之前所做的那样。  训练过程与前面的示例相同，并使用相同的 PyTorch 脚本: 


```bash
$ cd jetson-inference/python/training/classification
$ python3 train.py --model-dir=models/<YOUR-MODEL> data/<YOUR-DATASET>
```


> **注意: ** 如果内存不足或进程在训练期间被“终止”，请尝试[安装交换](pytorch-transfer-learning-CN.md#mounting-swap) 和[禁用桌面 GUI](pytorch-transfer-learning-CN.md#disabling-the-desktop-gui)。 <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;为了节省内存，您还可以减少 `--batch-size` （默认 8）和 `--workers` （默认 2）


与之前一样，训练后您需要将 PyTorch 模型转换为 ONNX: 


```bash
$ python3 onnx_export.py --model-dir=models/<YOUR-MODEL>
```


转换后的模型将保存在 `models/<YOUR-MODEL>/resnet18.onnx` 下，然后您可以使用 `imagenet` 程序加载该模型，就像我们在前面的示例中所做的那样: 


````bash
NET=模型/<您的模型>
数据集=数据/<您的数据集>


# C++ (MIPI CSI)
imagenet --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/labels.txt csi://0


# Python (MIPI CSI)
imagenet.py --model=$NET/resnet18.onnx --input_blob=input_0 --output_blob=output_0 --labels=$DATASET/labels.txt csi://0
````


如果需要，请返回并收集更多数据并再次重新训练您的模型。  您可以使用 `--resume` 和 `--epoch-start` 标志从上次停止的位置重新开始训练（运行 `python3 train.py --help` 了解更多信息）。  然后记得重新导出模型。


接下来，我们将使用 PyTorch 训练我们自己的目标检测模型。


<palign="right">下一个| <b><a href="pytorch-ssd-CN.md">重新训练SSD-Mobilenet</a></b>
<br/>
返回 | <b><a href="pytorch-plants-CN.md">在 PlantCLEF 数据集上重新训练</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>