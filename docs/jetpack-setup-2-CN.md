<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="../README-CN.md#hello-ai-world">返回</a> | <a href="aux-docker-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>系统设置</sup></p>


# 使用 JetPack 设置 Jetson


> **注意**: 如果您的 Jetson 已经刷写了 JetPack [SD 卡映像](https://developer.nvidia.com/embedded/learn/get-started-jetson-nano-devkit#write) 或 [SDK Manager](https://developer.nvidia.com/embedded/dlc/nv-sdk-manager)，您可以跳过此步骤并继续 [`Running the Docker Container`](aux-docker-CN.md) 或 [`Building the Project`](building-repo-2-CN.md)


NVIDIA **[JetPack](https://developer.nvidia.com/embedded/jetpack)** 是一款适用于 Jetson 的综合 SDK，用于开发和部署 AI 和计算机视觉应用程序。  JetPack 简化了操作系统和驱动程序的安装，并包含 L4T Linux 内核、CUDA Toolkit、cuDNN、TensorRT 等。


在尝试使用 Docker 容器或构建存储库之前，请确保您的 Jetson 已安装最新版本的 JetPack。


### Jetson Nano、Orin Nano 和 Xavier NX


对于具有可移动 microSD 存储的 Jetson 开发者套件，建议的安装方法是刷新最新的 **[SD 卡映像](https://developer.nvidia.com/embedded/downloads)**。


它预先填充了已安装的 JetPack 组件，并且可以从 Windows、Mac 或 Linux PC 进行刷新。  如果您还没有这样做，请按照您各自的 Jetson 的入门指南来刷新 SD 卡映像并设置您的设备: 


* [Jetson Nano 开发者套件入门](https://developer.nvidia.com/embedded/learn/get-started-jetson-nano-devkit)

* [Jetson Nano 2GB 开发者套件入门](https://developer.nvidia.com/embedded/learn/get-started-jetson-nano-2gb-devkit)

* [Jetson Xavier NX 开发者套件用户指南](https://developer.nvidia.com/embedded/downloads#?search=Jetson%20Xavier%20NX%20Developer%20Kit%20User%20Guide)

* [Jetson Orin Nano 开发者套件入门指南](https://developer.nvidia.com/embedded/learn/get-started-jetson-orin-nano-devkit)


### Jetson TX1/TX2、AGX Xavier 和 AGX Orin


其他 Jetson 应该通过将 [NVIDIA SDK Manager](https://developer.nvidia.com/embedded/dlc/nv-sdk-manager) 下载到运行 Ubuntu x86_64 的主机 PC 来刷新。  将 Micro-USB 或 USB-C 端口连接到主机 PC 并使设备进入恢复模式，然后再继续: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/nvsdkm.png" width="800">


有关更多详细信息，请参阅 **[NVIDIA SDK Manager 文档](https://docs.nvidia.com/sdk-manager/index.html)** 和 **[安装 Jetson 软件](https://docs.nvidia.com/sdk-manager/install-with-sdkm-jetson/index.html)** 页面。


### 获取项目


jetson-inference项目有两种使用方式: 


* 运行预构建的[Docker容器](aux-docker-CN.md)

* [从源代码构建项目](building-repo-2-CN.md)


建议最初使用容器来尽可能快地启动和运行（并且容器已经安装了 PyTorch），但是如果您更熟悉本机开发，那么自己编译项目也不复杂。


##
<palign="right">下一个| <b><a href="building-repo-2-CN.md">从源代码构建项目</a></b>
<br/>
返回 | <b><a href="../README-CN.md#hello-ai-world">概述</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>