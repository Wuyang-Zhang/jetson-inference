<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="pytorch-plants-CN.md">返回</a> | <a href="../README-CN.md#webapp-frameworks">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>迁移学习 - 目标检测</sup></s></p>


# 收集您自己的检测数据集


之前使用的 `camera-capture` 工具还可以标记实时视频中的目标检测数据集: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-collection-detect.jpg" >


当 `Dataset Type` 下拉菜单处于检测模式时，该工具会创建 [Pascal VOC](http://host.robots.ox.ac.uk/pascal/VOC/) 格式的数据集（训练期间支持）。


> **注意: **如果您想标记一组已有的图像（而不是从相机捕获它们），请尝试使用 [`CVAT`](https://github.com/openvinotoolkit/cvat) 等工具并以 Pascal VOC 格式导出数据集。  然后在数据集中创建一个 labels.txt，其中包含每个对象类的名称。


## 创建标签文件


在 `jetson-inference/python/training/detection/ssd/data` 下，创建一个空目录用于存储数据集和一个用于定义类标签的文本文件（通常称为 `labels.txt`）。  标签文件每行包含一个类标签，例如: 


``` bash
Water
Nalgene
Coke
Diet Coke
Ginger ale
```


如果您正在使用容器，则需要将数据集存储在如上所述的 [已安装目录](aux-docker-CN.md#mounted-data-volumes) 中，以便在容器关闭后保存它。


## 启动该工具


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


下面是 `Dataset Type` 下拉菜单设置为检测模式后的 `Data Capture Control` 窗口（首先执行此操作）。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-collection-detection-widget.jpg" >


然后，打开您创建的数据集路径和类标签。  然后 `Freeze/Edit` 和 `Save` 按钮将变为活动状态。


将相机定位在场景中的对象上，然后单击 `Freeze/Edit` 按钮（或按空格键）。  然后，实时摄像机视图将被“冻结”，您将能够在对象上绘制边界框。  然后，您可以为控制窗口网格表中的每个边界框选择适当的对象类。  完成图像标记后，再次单击按下的 `Freeze/Edit` 按钮以保存数据并解冻下一张图像的相机视图。


控制窗口中的其他小部件包括: 


* `Save on Unfreeze` - 当 `Freeze/Edit` 解冻时自动保存数据

* `Clear on Unfreeze` - 解冻时自动删除先前的边界框

* `Merge Sets` - 在训练集、验证集和测试集中保存相同的数据

* `Current Set` - 从训练/验证/测试集中选择

* 对于物体检测，至少需要训练集和测试集
    * 尽管如果您检查 `Merge Sets`，数据将被复制为 train、val 和 test

* `JPEG Quality` - 控制保存图像的编码质量和磁盘大小


重要的是，您的数据是从不同的物体方向、相机视角、照明条件收集的，最好是在不同的背景下收集，以创建一个对噪声和环境变化具有鲁棒性的模型。  如果您发现模型的性能没有达到您想要的效果，请尝试添加更多训练数据并调整条件。


## 训练你的模型


当您收集了一堆数据后，您可以尝试使用相同的 `train_ssd.py` 脚本在其上训练模型。  训练过程与前面的示例相同，但应设置 `--dataset-type=voc` 和 `--data=<PATH>` 参数: 


```bash
$ cd jetson-inference/python/training/detection/ssd
$ python3 train_ssd.py --dataset-type=voc --data=data/<YOUR-DATASET> --model-dir=models/<YOUR-MODEL>
```


> **注意: ** 如果内存不足或进程在训练期间被“终止”，请尝试[安装交换](pytorch-transfer-learning-CN.md#mounting-swap) 和[禁用桌面 GUI](pytorch-transfer-learning-CN.md#disabling-the-desktop-gui)。 <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;为了节省内存，您还可以减少 `--batch-size` （默认 4）和 `--workers` （默认 2）


与之前一样，训练后您需要将 PyTorch 模型转换为 ONNX: 


```bash
$ python3 onnx_export.py --model-dir=models/<YOUR-MODEL>
```


然后，转换后的模型将保存在 `<YOUR-MODEL>/ssd-mobilenet.onnx` 下，然后您可以使用 `detectnet` 程序加载该模型，就像我们在前面的示例中所做的那样: 


````bash
NET=模型/<您的模型>


detectornet --model=$NET/ssd-mobilenet.onnx --labels=$NET/labels.txt \
          --input-blob=input_0 --output-cvg=分数 --output-bbox=盒子 \
            CSI://0
````


> **注意: ** 使用生成到模型目录的标签文件（而不是最初为数据集创建的标签文件）运行推理非常重要。  这是因为 `BACKGROUND` 类被 `train_ssd.py` 添加到类标签中，并保存到模型目录（经过训练的模型期望使用的目录）。


如果需要，请返回并收集更多训练数据并再次重新训练您的模型。  您可以再次重新启动并使用 `--resume` 参数从上次中断的位置继续（运行 `python3 train_ssd.py --help` 了解更多信息）。  请记住在重新训练后将模型重新导出到 ONNX。


<palign="right">下一个| <b><a href="../README-CN.md#webapp-frameworks">Web应用程序框架</a></b>
<br/>
返回 | <b><a href="pytorch-ssd-CN.md">重新训练SSD-Mobilenet</a></p>
</b><palign="center"><sup>© 2016-2020 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>