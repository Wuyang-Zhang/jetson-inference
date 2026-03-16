<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="depthnet-CN.md">返回</a> | <a href="pytorch-cat-dog-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>迁移学习</sup></s></p>


# 使用 PyTorch 进行迁移学习


迁移学习是一种在新数据集上重新训练 DNN 模型的技术，比从头开始训练网络所需的时间更少。  通过迁移学习，可以对预训练模型的权重进行微调，以对定制数据集进行分类。  在这些示例中，我们将使用 <a href="https://arxiv.org/abs/1512.03385">ResNet-18</a> 和 [SSD-Mobilenet](pytorch-ssd-CN.md) 网络，但您也可以尝试使用其他网络。


<palign="center"><a href="https://arxiv.org/abs/1512.03385"><imgsrc="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/pytorch-resnet-18.png"width="600"></a></p>


尽管由于通常使用大型数据集和相关的计算需求，训练通常在具有离散 GPU 的 PC、服务器或云实例上执行，但通过使用迁移学习，我们能够在 Jetson 上重新训练各种网络，以开始训练和部署我们自己的 DNN 模型。


<a href=https://pytorch.org/>PyTorch</a> 是我们将使用的机器学习框架，除了用于收集和标记您自己的训练数据集的基于相机的工具之外，下面还提供了示例数据集和训练脚本。


## 安装 PyTorch


如果您正在[运行 Docker 容器](aux-docker-CN.md) 或在[构建项目](building-repo-2-CN.md#installing-pytorch) 时选择安装 PyTorch，则它应该已安装在您的 Jetson 上以供使用。  否则，如果您不使用容器并希望继续进行迁移学习，则可以立即安装它: 


``` bash
$ cd jetson-inference/build
$ ./install-pytorch.sh
```


<img src="https://raw.githubusercontent.com/dusty-nv/jetson-inference/master/docs/images/pytorch-installer.jpg" width="650">


> **注意**: 自动化 PyTorch 安装工具需要 JetPack 4.2 或更高版本。<br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;如果您想要进行[目标检测训练](pytorch-ssd-CN.md)，您应该使用 JetPack 4.4 或更高版本并安装 PyTorch for **Python 3.6**。


### 验证 PyTorch


您可以通过从交互式 Python shell 执行这些命令来测试 PyTorch 是否已正确安装并检测您的 GPU - 从终端运行 `python` 或 `python3`: 


`` 重击

>>> 进口火炬

>>> 打印（火炬.__版本__）

>>> print('CUDA 可用: ' + str(torch.cuda.is_available()))

>>> a = torch.cuda.FloatTensor(2).zero_()

>>> print('张量 a = ' + str(a))

>>> b = torch.randn(2).cuda()

>>> print('张量 b = ' + str(b))

>>> c = a + b

>>> print('张量 c = ' + str(c))

````


`` 重击

>>> 导入火炬视觉

>>> 打印（火炬视觉.__版本__）

````


请注意，torch 版本应报告为 `1.6.0`，torchvision 版本应报告为 `0.7.0`。


## 安装交换


除非您使用 Jetson AGX Xavier，否则您应该安装 4GB 交换空间，因为训练会占用大量额外内存。


在 Jetson 上（容器外部）运行以下命令以禁用 ZRAM 并创建交换文件: 


``` bash
sudo systemctl disable nvzramconfig
sudo fallocate -l 4G /mnt/4GB.swap
sudo mkswap /mnt/4GB.swap
sudo swapon /mnt/4GB.swap
```


然后将以下行添加到 `/etc/fstab` 的末尾以使更改持久化: 


``` bash
/mnt/4GB.swap  none  swap  sw 0  0
```


现在，您的交换文件将在重新启动后自动安装。  要检查使用情况，请运行 `swapon -s` 或 `tegrastats`。  禁用 ZRAM（内存压缩交换）也会释放物理内存，并且需要重新启动才能生效。


## 禁用桌面 GUI


如果您在训练时内存不足，您可能需要尝试在训练时禁用 Ubuntu 桌面 GUI。  这将释放窗口管理器和桌面使用的额外内存（对于 Unity/GNOME 约为 800MB，对于 LXDE 约为 250MB）


您可以暂时禁用桌面，在控制台中运行命令，然后在完成训练后重新启动桌面: 


``` bash
$ sudo init 3     # stop the desktop
# log your user back into the console
# run the PyTorch training scripts
$ sudo init 5     # restart the desktop
```


如果您希望在重新启动后保持此状态，可以使用以下命令来更改启动行为: 


``` bash
$ sudo systemctl set-default multi-user.target     # disable desktop on boot
$ sudo systemctl set-default graphical.target      # enable desktop on boot
```


然后，重新启动后，桌面将保持禁用或启用状态（以您设置的默认值为准）。


## 训练数据集


除了收集您自己的数据以创建您自己的定制模型之外，以下是通过迁移学习在一些示例数据集上重新训练模型的分步说明: 


* 分类/识别 (ResNet-18)

	* [猫/狗数据集的重新训练](pytorch-cat-dog-CN.md)

	* [PlantCLEF 数据集的重新训练](pytorch-plants-CN.md)

	* [收集您自己的分类数据集](pytorch-collect-CN.md)

* 物体检测（SSD-Mobilenet）

	* [重新训练SSD-Mobilenet](pytorch-ssd-CN.md)

	* [收集您自己的检测数据集](pytorch-collect-detection-CN.md)


此表包含数据集及其相关训练时间的摘要: 


|类型 |数据集 |尺寸|  课程 |培训图像|每个纪元的时间* |培训时间** |
|:------------:|:------------:|:--------:|:---------:|:-----------------:|:-----------------:|:-----------------:|
|分类| [`Cat/Dog`](pytorch-cat-dog-CN.md) | 800MB |    2 |      5,000 |  〜7-8 分钟 |    约 4 小时 |
|分类| [`PlantCLEF`](pytorch-plants-CN.md) | 1.5GB | 1.5GB   20 |     10,475 | 10,475约 15 分钟 |    ~8 小时 |
|检测| [`Fruit`](pytorch-ssd-CN.md) | 2GB |   8 |     6,375 | 6,375约 15 分钟 |    ~8 小时 |


*&nbsp;&nbsp;使用 Jetson Nano 对数据集进行一次完整训练的大约时间  
** 使用 Jetson Nano 训练模型 35 个 epoch 的大约时间


<palign="right">下一个| <b><a href="pytorch-cat-dog-CN.md">猫/狗数据集的重新训练</a></b>
<br/>
返回 | <b><a href="depthnet-CN.md">使用 DepthNet 的单目深度</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>