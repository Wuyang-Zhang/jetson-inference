<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="segnet-camera-2-CN.md">返回</a> | <a href="actionnet-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>姿势估计</sup></s></p>


# 姿势估计
姿势估计包括定位形成骨骼拓扑（也称为链接）的各个身体部位（也称为关键点）。姿势估计有多种应用，包括手势、AR/VR、HMI（人机界面）和姿势/步态校正。 [预训练模型](#pre-trained-pose-estimation-models) 用于人体和手部姿势估计，能够每帧检测多个人。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/posenet-0.jpg">


[`poseNet`](../c/poseNet.h) 对象接受图像作为输入，并输出对象姿势列表。  每个对象姿势都包含检测到的关键点列表，以及它们的位置和关键点之间的链接。  您可以查询这些以查找特定功能。  [`poseNet`](../c/poseNet.h) 可以在 [Python](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#poseNet) 和 [C++](../c/poseNet.h) 中使用。


作为使用 `poseNet` 类的示例，有 C++ 和 Python 的示例程序: 


- [`posenet.cpp`](../examples/posenet/posenet.cpp) (C++)

- [`posenet.py`](../python/examples/posenet.py) (Python)


这些样本能够检测图像、视频和摄像头中多个人的姿势。  有关支持的各种类型的输入/输出流的详细信息，请参阅[相机流和多媒体](aux-streaming-CN.md) 页面。


## 图像上的姿势估计


首先，让我们尝试在一些示例图像上运行 `posenet` 示例。  除了输入/输出路径之外，还有一些可选的附加命令行选项: 


- 可选 `--network` 标志，用于更改正在使用的[姿势模型](#pre-trained-pose-estimation-models)（默认为 `resnet18-body`）。

- 可选的 `--overlay` 标志，可以是 `box`、`links`、`keypoints` 和 `none` 的逗号分隔组合

	- 默认值为 `--overlay=links,keypoints`，在关键点上显示圆圈，在链接上显示线条

- 可选的 `--keypoint-scale` 值，控制叠加中关键点圆的半径（默认为 `0.0052`）

- 可选 `--link-scale` 值，控制叠加中链接线的线宽（默认为 `0.0013`）

- 可选 `--threshold` 值，设置检测的最小阈值（默认为 `0.15`）。


如果您使用 [Docker 容器](aux-docker-CN.md)，建议将输出图像保存到 `images/test` 安装目录。  然后，您可以在主机设备的 `jetson-inference/data/images/test` 下轻松查看这些图像（有关详细信息，请参阅[已安装的数据卷](aux-docker-CN.md#mounted-data-volumes)）。


以下是使用默认 Pose-ResNet18-Body 模型进行人体姿势估计的一些示例: 


`` 重击
# C++
$ ./posenet "images/ humans_*.jpg" 图片/test/pose_ humans_%i.jpg


# Python
$ ./posenet.py "images/ humans_*.jpg" 图片/test/pose_ humans_%i.jpg
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/posenet-1.jpg">


> **注意**: 第一次运行每个模型时，TensorRT 将花费几分钟来优化网络。 <br/>

> 然后，此优化的网络文件会缓存到磁盘，因此将来使用该模型的运行将加载得更快。


还有 `"images/peds_*.jpg"` 下的人的测试图像，您也可以尝试。


## 根据视频进行姿势估计


要对实时摄像机流或视频运行姿势估计，请从 [摄像机流和多媒体](aux-streaming-CN.md) 页面传入设备或文件路径。


`` 重击
# C++
$ ./posenet /dev/video0 # csi://0 如果使用 MIPI CSI 摄像头


# Python
$ ./posenet.py /dev/video0 # csi://0 如果使用 MIPI CSI 摄像头
````


<a href="https://www.youtube.com/watch?v=hwFtWYR986Q" target="_blank"><img src=https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/posenet-video-body.jpg width="750"></a>


`` 重击
# C++
$ ./posenet --network=resnet18-hand /dev/video0


# Python
$ ./posenet.py --network=resnet18-hand /dev/video0
````


<a href="https://www.youtube.com/watch?v=6NL_IE44vRE" target="_blank"><img src=https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/posenet-video-hands.jpg width="750"></a>


## 预训练的姿势估计模型


以下是可供使用的预训练姿态估计网络，以及用于加载预训练模型的 `posenet` 的相关 `--network` 参数: 


|型号| CLI 参数 |网络类型枚举 |要点 |
| ------------------------|--------------------|--------------------|------------------------|
| Pose-ResNet18-Body | 姿势-ResNet18-Body `resnet18-body` | `RESNET18_BODY` | 18 | 18
| Pose-ResNet18-手 | `resnet18-hand` | `RESNET18_HAND` | 21 | 21
| Pose-DenseNet121-身体 | `densenet121-body` | `DENSENET121_BODY` | 18 | 18


您可以通过将命令行上的 `--network` 标志设置为上表中相应的 CLI 参数之一来指定要加载的模型。  默认情况下，如果未指定可选的 `--network` 标志，则使用 Pose-ResNet18-Body。



## 使用对象姿势


如果要访问位姿关键点位置，`poseNet.Process()` 函数将返回 `poseNet.ObjectPose` 结构的列表。  每个对象姿势代表一个对象（即一个人），并包含检测到的关键点和链接的列表 - 有关详细信息，请参阅 [Python](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#poseNet) 和 [C++](../c/poseNet.h) 文档。


下面是通过在 `left_shoulder` 和 `left_wrist` 关键点之间形成向量来查找人所指向的 2D 方向（在图像空间中）的 Python 伪代码: 


```` 蟒蛇
姿势 = net.Process(img)


对于姿势中的姿势: 
    # 从检测到的关键点列表中找到关键点索引
    # 您可以在模型的 JSON 文件中找到这些关键点名称， 
    # 或使用 net.GetKeypointName() / net.GetNumKeypoints()
    left_wrist_idx =pose.FindKeypoint('left_wrist')
    left_shoulder_idx =pose.FindKeypoint('left_shoulder')


# 如果关键点索引<0，则表示在图像中没有找到它
    如果 left_wrist_idx < 0 或 left_shoulder_idx < 0: 
        继续


left_wrist = 姿势.Keypoints[left_wrist_idx]
    left_shoulder =pose.Keypoints[left_shoulder_idx]


point_x = left_shoulder.x - left_wrist.x
    point_y = left_shoulder.y - left_wrist.y


print(f"人 {pose.ID} 指向 ({point_x}, {point_y})")
````


这是一个简单的示例，但您可以通过进一步操作向量并查找更多关键点来使其更加有用。  还有更先进的技术使用机器学习对姿势结果进行手势分类，例如 [`trt_hand_pose`](https://github.com/NVIDIA-AI-IOT/trt_pose_hand) 项目。



##
<palign="right">下一个| <b><a href="actionnet-CN.md">动作识别</a></b>
<br/>
返回 | <b><a href="segnet-camera-2-CN.md">运行实时摄像头分割演示</a></p>
</b><palign="center"><sup>© 2016-2021 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>