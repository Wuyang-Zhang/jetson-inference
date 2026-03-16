<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="jetpack-setup-2-CN.md">返回</a> | <a href="building-repo-2-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>系统设置</sup></p>


# 运行 Docker 容器


该项目的预构建 Docker 容器映像托管在 [DockerHub](https://hub.docker.com/r/dustynv/jetson-inference/tags) 上。  或者，您可以从源代码[构建项目](building-repo-2-CN.md)。


以下是当前可用的容器标签: 


|集装箱标签| L4T版 |          JetPack 版本 |
|--------------------------------------------------------------------------------------------------------|:---------:|:--------------------------------:|
| [`dustynv/jetson-inference:r36.3.0`](https://hub.docker.com/r/dustynv/jetson-inference/tags) | L4T R36.3.0 | JetPack 6.0 正式版 |
| [`dustynv/jetson-inference:r36.2.0`](https://hub.docker.com/r/dustynv/jetson-inference/tags) | L4T R36.2.0 | JetPack 6.0 DP |
| [`dustynv/jetson-inference:r35.3.1`](https://hub.docker.com/r/dustynv/jetson-inference/tags) | L4T R35.3.1 | JetPack 5.1.1 |
| [`dustynv/jetson-inference:r35.2.1`](https://hub.docker.com/r/dustynv/jetson-inference/tags) | L4T R35.2.1 |喷气背包 5.1 |
| [`dustynv/jetson-inference:r35.1.0`](https://hub.docker.com/r/dustynv/jetson-inference/tags) | L4T R35.1.0 | JetPack 5.0.2 |
| [`dustynv/jetson-inference:r34.1.1`](https://hub.docker.com/r/dustynv/jetson-inference/tags) | L4T R34.1.1 | JetPack 5.0.1 |
| [`dustynv/jetson-inference:r32.7.1`](https://hub.docker.com/r/dustynv/jetson-inference/tags) | L4T R32.7.1 |喷气背包 4.6.1 |
| [`dustynv/jetson-inference:r32.6.1`](https://hub.docker.com/r/dustynv/jetson-inference/tags) | L4T R32.6.1 |喷气背包 4.6 |
| [`dustynv/jetson-inference:r32.5.0`](https://hub.docker.com/r/dustynv/jetson-inference/tags) | L4T R32.5.0 |喷气背包 4.5 |
| [`dustynv/jetson-inference:r32.4.4`](https://hub.docker.com/r/dustynv/jetson-inference/tags) | L4T R32.4.4 |喷气背包 4.4.1 |
| [`dustynv/jetson-inference:r32.4.3`](https://hub.docker.com/r/dustynv/jetson-inference/tags) | L4T R32.4.3 |喷气背包 4.4 |



> **注意: ** 您在 Jetson 上安装的 JetPack-L4T 版本需要与上述标签之一兼容。  如果您安装了不同版本的 JetPack，请升级到最新的 JetPack 或 [从源代码构建项目](docs/building-repo-2.md) 直接编译项目。


这些容器使用 [`l4t-pytorch`](https://ngc.nvidia.com/catalog/containers/nvidia:l4t-pytorch) 基础容器，因此已经包含对训练模型和迁移学习的支持。


## 启动容器


由于运行容器需要各种安装和设备，建议使用 [`docker/run.sh`](../docker/run.sh) 脚本来运行容器: 


```bash
$ git clone --recursive --depth=1 https://github.com/dusty-nv/jetson-inference
$ cd jetson-inference
$ docker/run.sh
```


> **注意: ** 由于使用了 Docker 脚本以及安装到容器中的数据目录结构，您仍然应该在主机设备上克隆项目（即，即使不打算本机构建/安装项目）


[`docker/run.sh`](../docker/run.sh) 将根据您当前安装的 JetPack-L4T 版本自动从 DockerHub 中提取正确的容器标签，并安装适当的数据目录和设备，以便您可以在容器内使用摄像头/显示器等。


### ROS支持


该项目还具有可用于 ROS/ROS2 的 [ros_deep_learning](https://github.com/dusty-nv/ros_deep_learning) 软件包，通过指定 `--ros=ROS_DISTRO` 选项，您可以启动使用 ROS 构建的容器版本。  支持的 ROS 发行版包括 Noetic、Foxy、Galicate、Humble 和 Iron: 


``` bash
$ docker/run.sh --ros=humble   # noetic, foxy, galactic, humble, iron
```


容器在启动时将获取 ROS 环境和包。  有关更多信息，请参阅 [ros_deep_learning](https://github.com/dusty-nv/ros_deep_learning) 文档。


### x86 支持


除了在基于 Jetson ARM 的架构上受支持之外，jetson-inference 容器还可以[构建](#building-the-container) 并在具有 NVIDIA GPU 的 x86_64 系统上运行。  这可用于运行 Hello AI World 教程及其随附的应用程序/库，或者用于在 PC/服务器上进行更快的培训。  为此，请首先安装 [NVIDIA 驱动程序](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/install-guide.html#pre-requisites) 和 [NVIDIA 容器运行时](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/nvidia-docker.html) 以在 Docker 中启用 GPU 支持。


要运行最新的预构建 jetson-inference x86 容器，请使用与上面相同的命令 (`docker/run.sh`)。  如果您想使用较新/较旧版本的 [`nvcr.io/nvidia/pytorch`](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/pytorch) 基本容器，请使用所需标签编辑 [此行](https://github.com/dusty-nv/jetson-inference/blob/master/docker/tag.sh#L40)，然后运行 ​​[`docker/build.sh`](#building-the-container)


虽然 jetson-inference 容器是为 Linux 构建的，但它可以在 WSL 2 下的 Windows 上运行，只需遵循 [WSL 上的 CUDA 用户指南](https://docs.nvidia.com/cuda/wsl-user-guide/index.html#ch02-sub03-installing-wsl2)，然后按上述方式安装 Docker 和 NVIDIA 容器运行时即可。  如果您需要在 WSL 2 下使用 USB 网络摄像头和 V4L2，您还需要使用这些[配置更改](https://github.com/PINTO0309/wsl2_linux_kernel_usbcam_enable_conf) 重新编译 WSL 内核。


### 挂载的数据卷


作为参考，以下路径会自动从主机设备安装到容器中: 


* `jetson-inference/data`（存储网络模型、序列化TensorRT引擎和测试图像）

* `jetson-inference/python/training/classification/data`（存储分类训练数据集）

* `jetson-inference/python/training/classification/models` （存储 PyTorch 训练的分类模型）

* `jetson-inference/python/training/detection/ssd/data`（存储检测训练数据集）

* `jetson-inference/python/training/detection/ssd/models` （存储 PyTorch 训练的检测模型）


这些安装的卷可确保模型和数据集存储在容器外部，并且在容器关闭时不会丢失。


如果您希望将自己的目录挂载到容器中，可以使用 [`docker/run.sh`](../docker/run.sh) 的 `--volume HOST_DIR:MOUNT_DIR` 参数: 


```bash
$ docker/run.sh --volume /my/host/path:/my/container/path    # these should be absolute paths
```


您可以多次指定 `--volume` 来挂载多个目录。  有关详细信息，请运行或查看 [`docker/run.sh --help`](../docker/run.sh)


## 运行应用程序


一旦容器启动并运行，您就可以在容器内像平常一样运行教程中的示例程序: 


```bash
$ cd build/aarch64/bin
$ ./video-viewer /dev/video0
$ ./imagenet images/jellyfish.jpg images/test/jellyfish.jpg
$ ./detectnet images/peds_0.jpg images/test/peds_0.jpg
# (press Ctrl+D to exit the container)
```


> **注意: **当您从示例程序之一（例如 imagenet 或 detectornet）保存图像时，建议将它们保存到 `images/test`。  然后可以从主机设备的 `jetson-inference/data/images/test` 目录中轻松查看这些图像。


## 构建容器


如果您正在学习 Hello AI World 教程，则可以忽略此部分并跳至下一步。  但如果您希望重新构建容器或构建自己的容器，则可以使用 [`docker/build.sh`](../docker/build.sh) 脚本来构建项目的 [`Dockerfile`](../Dockerfile): 


```bash
$ docker/build.sh
```


>  **注意: ** 您应该首先将默认的 `docker-runtime` 设置为 nvidia，有关详细信息，请参阅[此处](https://github.com/dusty-nv/jetson-containers#docker-default-runtime)。


您还可以通过在您自己的 Dockerfile 中使用 `FROM dustynv/jetson-inference:rXX.X.X` 行来以此为基础创建您自己的容器。


## 入门


如果您选择在 Docker 容器内运行项目，则可以继续[使用 ImageNet 对图像进行分类](imagenet-console-2-CN.md)。


但是，如果您希望直接在 Jetson 上（在容器外部）安装项目，请转至 [从源代码构建项目](building-repo-2-CN.md)。


##
<palign="right">下一个| <b><a href="building-repo-2-CN.md">从源代码构建项目</a></b>
<br/>
返回 | <b><a href="jetpack-setup-2-CN.md">使用 JetPack 设置 Jetson</a></p>
<palign="center"><sup>© 2016-2020 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>

