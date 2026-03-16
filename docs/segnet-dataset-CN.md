<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="detectnet-camera-CN.md">返回</a> | <a href="segnet-pretrained-CN.md">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>语义分割</sup></s></p>


# 使用 SegNet 进行语义分割


我们在本教程中强调的第三个深度学习功能是语义分割。  语义分割基于图像识别，只不过分类发生在像素级别，而不是像图像识别那样对整个图像进行分类。  这是通过对预训练的图像识别模型（如 Alexnet）进行“卷积化”来实现的，将其转变为能够进行每像素标记的全卷积分割模型。  分割对于环境传感和避免碰撞很有用，可以对每个场景的许多不同潜在对象（包括场景前景和背景）进行密集的每像素分类。


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-cityscapes.jpg)


[`segNet`](../c/segNet.h) 对象接受 2D 图像作为输入，并输出带有每像素分类掩码叠加的第二个图像。  掩模的每个像素对应于被分类的对象的类别。


> **注意**: 有关分割的更多背景信息，请参阅 DIGITS [语义分割](https://github.com/NVIDIA/DIGITS/tree/master/examples/semantic-segmentation) 示例。


### 下载空中无人机数据集


作为图像分割的示例，我们将使用将地面地形与天空分开的空中无人机数据集。  该数据集采用第一人称视角 (FPV)，用于模拟飞行中无人机的有利位置，并训练一个网络，该网络充当由其感知的地形引导的自动驾驶仪。


要下载并提取数据集，请从运行 DIGITS 服务器的主机 PC 运行以下命令: 


`` 重击
$ wget --no-check-certificate https://nvidia.box.com/shared/static/ft9cc5yjvrbhkh07wcivu5ji9zola6i1.gz -O NVIDIA-Aerial-Drone-Dataset.tar.gz


HTTP 请求已发送，正在等待响应... 200 OK
长度: 7140413391 (6.6G) [应用程序/八位字节流]
保存到: “NVIDIA-Aerial-Drone-Dataset.tar.gz”


NVIDIA-Aerial-Drone-Datase 100%[========================================>] 6.65G 3.33MB/s，44m 44s


2017-04-17 14:11:54 (2.54 MB/s) - ‘NVIDIA-Aerial-Drone-Dataset.tar.gz’已保存 [7140413391/7140413391]


$ tar -xzvf NVIDIA-Aerial-Drone-Dataset.tar.gz 
````


该数据集包括从无人机平台飞行中捕获的各种剪辑，但我们在本教程中将重点关注的剪辑位于 `FPV/SFWA` 下。  接下来，我们将在训练模型之前在 DIGITS 中创建训练数据库。


### 将航空数据集导入 DIGITS


首先，将浏览器导航到 DIGITS 服务器实例，然后从“数据集”选项卡的下拉列表中选择创建一个新的 `Segmentation Dataset`: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-digits-create-dataset.png" width="250">


在数据集创建表单中，指定以下选项以及提取航空数据集位置下的图像和标签文件夹的路径: 


* 特征图像文件夹: `NVIDIA-Aerial-Drone-Dataset/FPV/SFWA/720p/images`

* 标签图像文件夹: `NVIDIA-Aerial-Drone-Dataset/FPV/SFWA/720p/labels`

* 将 `% for validation` 设置为 1%

* 类标签: `NVIDIA-Aerial-Drone-Dataset/FPV/SFWA/fpv-labels.txt`

* 颜色图: 来自文本文件

* 特征编码: `None`

* 标签编码: `None`


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-digits-aerial-dataset-options.png)


将数据集命名为您选择的名称，然后单击页面底部的 `Create` 按钮以启动导入作业。  接下来我们将创建新的分割模型并开始训练。


##
<palign="right">下一个| <b><a href="segnet-pretrained-CN.md">生成预训练的 FCN-Alexnet</a></b>
<br/>
返回 | <b><a href="detectnet-camera-CN.md">运行实时摄像头检测演示</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>