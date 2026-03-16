<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="digits-workflow-CN.md">返回</a> | <a href="jetpack-setup-CN.md">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>系统设置</sup></p>


# 数字系统设置


在本教程中，我们将使用主机 PC（或云实例）来训练 DNN，并使用 Jetson 进行推理。


由于训练所需的依赖项数量较多，建议初学者使用 **[NVIDIA GPU Cloud (NGC)](https://www.nvidia.com/en-us/gpu-cloud/)** 或 [nvidia-docker](https://github.com/NVIDIA/nvidia-docker) 设置其主机训练 PC。  这些方法可自动在主机上安装驱动程序和机器学习框架。  NGC 可用于在本地部署 Docker 映像，或远程部署到 AWS 或 Azure N 系列等云提供商。


主机 PC 还将用于使用最新的 JetPack 来刷新 Jetson。  首先，我们将使用所需的操作系统和工具设置和配置主机培训 PC。


### 在主机上安装 Ubuntu


如果您的主机 PC 上尚未安装 Ubuntu，请从以下位置之一下载并安装 Ubuntu 16.04 x86_64: 


```
http://releases.ubuntu.com/16.04/ubuntu-16.04.2-desktop-amd64.iso
http://releases.ubuntu.com/16.04/ubuntu-16.04.2-desktop-amd64.iso.torrent
```


Ubuntu 14.04 x86_64 或 Ubuntu 18.04 x86_64 也可以接受，稍后在使用 apt-get 安装一些软件包时稍加修改即可。


### 使用 NGC 容器设置主机训练 PC


> **注意**: 要在主机上本地设置 DIGITS，您应该转到 [`Natively setting up DIGITS on the Host`](digits-native-CN.md)（高级）


NVIDIA 为全球 AI 开发人员托管 NVIDIA® GPU Cloud (NGC) 容器注册表。
您可以下载适用于各种深度学习框架的容器化软件堆栈，并使用 NVIDIA 库和 CUDA 运行时版本进行优化和验证。


<img src="./images/NGC-Registry_DIGITS.png">


如果您的 PC 上有最新一代 GPU（Pascal 或更新版本），则使用 NGC 注册表容器可能是设置 DIGITS 的最简单方法。
要在本地主机（而不是云）上使用 NGC 注册表容器，您可以遵循此详细的[设置指南](https://docs.nvidia.com/ngc/ngc-titan-setup-guide/index.html)。


#### 安装 NVIDIA 驱动程序


添加 NVIDIA 开发人员存储库并安装 NVIDIA 驱动程序。


``` bash
$ sudo apt-get install -y apt-transport-https curl
$ cat <<EOF | sudo tee /etc/apt/sources.list.d/cuda.list > /dev/null
deb https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1604/x86_64 /
EOF
$ curl -s \
 https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1604/x86_64/7fa2af80.pub \
 | sudo apt-key add -
$ cat <<EOF | sudo tee /etc/apt/preferences.d/cuda > /dev/null
Package: *
Pin: origin developer.download.nvidia.com
Pin-Priority: 600
EOF
$ sudo apt-get update && sudo apt-get install -y --no-install-recommends cuda-drivers
$ sudo reboot
```


重新启动后，检查是否可以运行 `nvidia-smi` 并查看您的 GPU 是否显示。


`` 重击
$ Nvidia-SMI
2018 年 5 月 31 日星期四 11:56:44
+----------------------------------------------------------------------------------------+
| NVIDIA-SMI 390.30 驱动程序版本: 390.30 |
|----------------------------------------+----------------------+--------------------------------+
| GPU 名称持久性-M|总线 ID Disp.A |挥发性未校正。 ECC |
|风扇温度性能功率: 使用/上限|         内存使用情况 | GPU-Util 计算 M。
|===============================+======================+========================|
|   0 Quadro GV100 关闭 | 00000000:01:00.0 开启 |                  关闭 |
| 29% 41C P2 27W / 250W | 29%   1968MiB / 32506MiB |     22% 默认 |
+----------------------------------------++----------------------+------------------------+


````


#### 安装 Docker


安装先决条件，安装 GPG 密钥，然后添加 Docker 存储库。


``` bash
$ sudo apt-get install -y ca-certificates curl software-properties-common
$ curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
$ sudo add-apt-repository \
 "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"
```


添加 Docker Engine Utility (nvidia-docker2) 存储库，安装 nvidia-docker2，设置每次无需 sudo 使用 Docker 的权限，然后重新启动系统。


``` bash
$ curl -s -L https://nvidia.github.io/nvidia-docker/gpgkey | \
  sudo apt-key add -
$ ccurl -s -L https://nvidia.github.io/nvidia-docker/ubuntu16.04/amd64/nvidia-docker.list | \
  sudo tee /etc/apt/sources.list.d/nvidia-docker.list
$ csudo apt-get update
$ csudo apt-get install -y nvidia-docker2
$ csudo usermod -aG docker $USER
$ sudo reboot
```


#### NGC 注册


如果您还没有注册 NGC，请注册。


https://ngc.nvidia.com/signup/register


生成您的 API 密钥，并将其保存在安全的地方。稍后您将使用它。


<img src="./images/NGC-Registry_API-Key-generated.png" width="500">


#### 为 DIGITS 设置数据和作业目录


返回您的 PC（重新启动后），登录 NGC 容器注册表


``` bash
$ docker login nvcr.io
```


系统将提示您输入用户名和密码


``` bash
Username: $oauthtoken
Password: <Your NGC API Key>
```


对于测试，请使用 CUDA 容器查看 nvidia-smi 是否显示您的 GPU。


``` bash
docker run --runtime=nvidia --rm nvcr.io/nvidia/cuda:9.0-cudnn7-devel-ubuntu16.04 nvidia-smi
```


#### 设置数据和作业目录


在主机上创建数据和作业目录，由 DIGITS 容器挂载。


``` bash
$ mkdir /home/username/data
$ mkdir /home/username/digits-jobs
```


#### 启动 DIGITS 容器


``` bash
$ nvidia-docker run --name digits -d -p 8888:5000 \
 -v /home/username/data:/data:ro
 -v /home/username/digits-jobs:/workspace/jobs nvcr.io/nvidia/digits:18.05
```


打开网络浏览器并访问 http://localhost:8888


##
<palign="right">下一个| <b><a href="jetpack-setup-CN.md">使用 JetPack 设置 Jetson</a></b>
<br/>
返回 | <b><a href="digits-workflow-CN.md">DIGITS 工作流程</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>

