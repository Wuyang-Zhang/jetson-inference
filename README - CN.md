

# 深度学习部署（Deploying Deep Learning）

欢迎阅读本教程指南。本项目提供一个用于 **实时推理和视觉深度神经网络（DNN）库** 的教学指南，适用于 **NVIDIA Jetson** 设备。

该项目使用 **TensorRT** 在 GPU 上运行优化后的神经网络，并支持通过 **C++ 或 Python** 进行调用，同时使用 **PyTorch** 进行模型训练。

支持的视觉 DNN 基础模块包括：

- **imageNet**：图像分类
- **detectNet**：目标检测
- **segNet**：语义分割
- **poseNet**：姿态估计
- **actionNet**：动作识别

项目还提供了示例，包括：

- 从实时摄像头视频流进行推理
- 使用 WebRTC 构建 Web 应用
- 支持 ROS / ROS2 系统

------

## Hello AI World

Hello AI World 教程可以 **完全在 Jetson 设备上运行**，包括：

- 使用 TensorRT 进行实时推理
- 使用 PyTorch 进行迁移学习（Transfer Learning）

如果需要安装说明，请查看 **系统设置（System Setup）**。

建议先阅读 **推理（Inference）** 部分以熟悉概念，然后再继续学习 **训练（Training）** 部分来训练自己的模型。

------

# 系统设置（System Setup）

- 设置 Jetson 和 JetPack
- 运行 Docker 容器
- 从源码构建项目

------

# 推理（Inference）

## 图像分类（Image Classification）

- 在 Jetson 上运行 ImageNet 程序
- 编写自己的图像识别程序（Python）
- 编写自己的图像识别程序（C++）
- 运行实时摄像头识别 Demo
- 多标签分类（图像标签）

------

## 目标检测（Object Detection）

- 从图像中检测目标
- 运行实时摄像头检测 Demo
- 编写自己的目标检测程序
- 使用 TAO 训练的检测模型
- 视频目标跟踪

------

## 语义分割（Semantic Segmentation）

- 在命令行中进行图像分割
- 运行实时摄像头分割 Demo

------

## 其他视觉任务

- 姿态估计（Pose Estimation）
- 动作识别（Action Recognition）
- 背景移除（Background Removal）
- 单目深度估计（Monocular Depth）

------

# 模型训练（Training）

## 使用 PyTorch 进行迁移学习

### 图像分类（ResNet-18）

- 使用 Cat/Dog 数据集重新训练
- 使用 PlantCLEF 数据集重新训练
- 采集自己的分类数据集

------

### 目标检测（SSD-Mobilenet）

- 重新训练 SSD-Mobilenet
- 采集自己的检测数据集

------

# Web 应用框架

支持使用以下框架构建 AI Web 应用：

- WebRTC Server
- HTML / JavaScript
- Flask + REST API
- Plotly Dashboard
- Recognizer（交互式训练）

------

# 附录

- 摄像头视频流与多媒体处理
- 使用 CUDA 进行图像处理
- ROS / ROS2 深度学习推理节点

------

# Jetson AI Lab

Jetson AI Lab 提供额外的教程，包括：

- LLM（大语言模型）
- Vision Transformer（ViT）
- Vision Language Model（VLM）

这些教程可以在 **Jetson Orin**（部分也支持 Xavier）上运行。

示例包括：

- NanoOWL（开放词汇目标检测）
- Jetson 上运行 LLaVA
- 多模态数据库 NanoDB

------

# 视频教程（Video Walkthroughs）

以下是 Hello AI World 的视频教程：

| 内容                           | 描述                            |
| ------------------------------ | ------------------------------- |
| Hello AI World Setup           | 在 Jetson Nano 上下载并运行容器 |
| Image Classification Inference | 编写 Python 图像分类程序        |
| Training Image Classification  | 使用 PyTorch 训练分类模型       |
| Object Detection Inference     | 编写 Python 目标检测程序        |
| Training Object Detection      | 使用 PyTorch 训练检测模型       |
| Semantic Segmentation          | 实时语义分割                    |

------

# API 参考（API Reference）

该仓库提供 **C++ 和 Python API 文档**。

## jetson-inference

| 功能     | C++           | Python        |
| -------- | ------------- | ------------- |
| 图像识别 | imageNet      | imageNet      |
| 目标检测 | detectNet     | detectNet     |
| 语义分割 | segNet        | segNet        |
| 姿态估计 | poseNet       | poseNet       |
| 动作识别 | actionNet     | actionNet     |
| 背景移除 | backgroundNet | backgroundNet |
| 单目深度 | depthNet      | depthNet      |

------

## jetson-utils

提供底层工具库：

- C++ API
- Python API

这些库可以在外部项目中使用：

```
libjetson-inference
libjetson-utils
```

------

# 代码示例（Code Examples）

Hello AI World 教程中包含以下示例：

- 编写自己的图像识别程序（Python）
- 编写自己的图像识别程序（C++）

此外还提供以下示例代码：

| 功能     | C++               | Python           |
| -------- | ----------------- | ---------------- |
| 图像分类 | imagenet.cpp      | imagenet.py      |
| 目标检测 | detectnet.cpp     | detectnet.py     |
| 语义分割 | segnet.cpp        | segnet.py        |
| 姿态估计 | posenet.cpp       | posenet.py       |
| 动作识别 | actionnet.cpp     | actionnet.py     |
| 背景移除 | backgroundnet.cpp | backgroundnet.py |
| 单目深度 | depthnet.cpp      | depthnet.py      |

这些示例在 **从源码构建项目时会自动编译**。

运行方法：

```
--help
```

可以查看使用说明。

------

# 预训练模型（Pre-Trained Models）

项目提供多种 **自动下载的预训练模型**。

------

## 图像分类模型

| 网络         | CLI 参数     |
| ------------ | ------------ |
| AlexNet      | alexnet      |
| GoogleNet    | googlenet    |
| ResNet-18    | resnet-18    |
| ResNet-50    | resnet-50    |
| ResNet-101   | resnet-101   |
| ResNet-152   | resnet-152   |
| VGG-16       | vgg-16       |
| VGG-19       | vgg-19       |
| Inception-v4 | inception-v4 |

------

## 目标检测模型

| 模型             | 参数             | 类别                |
| ---------------- | ---------------- | ------------------- |
| SSD-Mobilenet v1 | ssd-mobilenet-v1 | COCO 91类           |
| SSD-Mobilenet v2 | ssd-mobilenet-v2 | COCO 91类           |
| SSD-Inception v2 | ssd-inception-v2 | COCO 91类           |
| PeopleNet        | peoplenet        | person / bag / face |
| DashCamNet       | dashcamnet       | 行人 / 车辆         |
| TrafficCamNet    | trafficcamnet    | 行人 / 车辆         |
| FaceDetect       | facedetect       | 人脸                |

------

## 语义分割模型

支持数据集：

- Cityscapes
- DeepScene
- Pascal VOC
- SUN RGB-D
- Multi-Human

模型使用 **FCN-ResNet18** 架构，并提供不同分辨率版本。

------

## 姿态估计模型

| 模型             | 参数             |
| ---------------- | ---------------- |
| ResNet18 Body    | resnet18-body    |
| ResNet18 Hand    | resnet18-hand    |
| DenseNet121 Body | densenet121-body |

------

## 动作识别模型

| 模型              | 类别数量 |
| ----------------- | -------- |
| ResNet18 Kinetics | 1040     |
| ResNet34 Kinetics | 1040     |

------

# 推荐系统配置（Recommended System Requirements）

支持以下 Jetson 设备：

- Jetson Nano
- Jetson Nano 2GB
- Jetson Orin Nano
- Jetson Xavier NX
- Jetson AGX Xavier
- Jetson AGX Orin
- Jetson TX2
- Jetson TX1

------

PyTorch 训练部分可以：

- 在 Jetson 上运行
- 或在 PC / 服务器 / 云 GPU 上运行以加快训练速度

------

# 额外资源（Extra Resources）

- ros_deep_learning：TensorRT ROS 节点
- NVIDIA AI IoT GitHub
- Jetson eLinux Wiki

------

# DIGITS 教程（已弃用）

旧版教程使用：

```
DIGITS + Caffe
```

现在推荐使用：

```
PyTorch Transfer Learning
```

