<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="digits-workflow-CN.md">返回</a> | <a href="jetpack-setup-CN.md">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>系统设置</sup></p>


# 在主机上本地设置 DIGITS


> **注意**: 建议初学者使用 [NVIDIA GPU Cloud (NGC)](digits-setup-CN.md) 设置 DIGITS


如果您选择不对 DIGITS 使用 NGC 容器，则需要在 PC 上本地设置 CUDA 开发环境并构建 DIGITS。


#### 在主机上安装 NVIDIA 驱动程序


此时，JetPack 将使用最新的 L4T BSP 刷新 Jetson，并将 CUDA 工具包安装到 Jetson 和主机 PC。  但是，仍然需要在主机 PC 上安装 NVIDIA PCIe 驱动程序才能启用 GPU 加速训练。  从主机 PC 运行以下命令，从 Ubuntu 存储库安装 NVIDIA 驱动程序: 


``` bash
$ sudo apt-get install nvidia-384	# use nvidia-375 for alternate version
$ sudo reboot
```


重新启动后，NVIDIA 驱动程序应列在 `lsmod` 下: 


``` bash
$ lsmod | grep nvidia
nvidia_uvm            647168  0
nvidia_drm             49152  1
nvidia_modeset        790528  4 nvidia_drm
nvidia              12144640  60 nvidia_modeset,nvidia_uvm
drm_kms_helper        167936  1 nvidia_drm
drm                   368640  4 nvidia_drm,drm_kms_helper
```


要验证 CUDA 工具包和 NVIDIA 驱动程序是否正常工作，请运行 CUDA 示例附带的一些测试: 


``` bash
$ cd /usr/local/cuda/samples
$ sudo make
$ cd bin/x86_64/linux/release/
$ ./deviceQuery
$ ./bandwidthTest --memory=pinned
```


#### 在主机上安装 cuDNN


下一步是在主机 PC 上安装 NVIDIA **[cuDNN](https://developer.nvidia.com/cudnn)** 库。  从 NVIDIA cuDNN 网页下载 libcudnn 和 libcudnn 软件包: 


[`https://developer.nvidia.com/cudnn`](https://developer.nvidia.com/cudnn)


然后使用以下命令安装软件包: 


``` bash
$ sudo dpkg -i libcudnn<version>_amd64.deb
$ sudo dpkg -i libcudnn-dev_<version>_amd64.deb
```


#### 在主机上安装 NVcaffe


[NVcaffe](https://github.com/nvidia/caffe/tree/caffe-0.15) 是 Caffe 的 NVIDIA 分支，针对 GPU 进行了优化。  NVcaffe 需要 cuDNN，并由 DIGITS 用于训练 DNN。  要安装它，请从 GitHub 克隆 NVcaffe 存储库，并使用 caffe-0.15 分支从源代码进行编译。


> **注意**: 对于本教程，仅主机上需要 NVcaffe（用于训练）。  在推理阶段，TensorRT 在 Jetson 上使用，不需要 caffe。


首先从 https://github.com/NVIDIA/caffe 克隆 caffe-0.15 分支


``` bash
$ git clone -b caffe-0.15 https://github.com/NVIDIA/caffe
```


使用此处的[说明](http://caffe.berkeleyvision.org/installation.html#compilation) 构建caffe: 


[`http://caffe.berkeleyvision.org/installation.html#compilation`](http://caffe.berkeleyvision.org/installation.html#compilation)


现在应该配置并构建 Caffe。  现在编辑用户的 ~/.bashrc 以包含 Caffe 树的路径（替换下面的路径以反映您自己的路径）: 


``` bash
export CAFFE_ROOT=/home/dusty/workspace/caffe
export PYTHONPATH=/home/dusty/workspace/caffe/python:$PYTHONPATH
```


关闭并重新打开终端以使更改生效。



#### 在主机上安装 DIGITS


NVIDIA **[DIGITS](https://developer.nvidia.com/digits)** 是一种基于 Python 的 Web 服务，可交互式训练 DNN 并管理数据集。  正如 DIGITS 工作流程中所强调的那样，它在主机 PC 上运行，以在训练阶段创建网络模型。  然后将经过训练的模型从主机 PC 复制到 Jetson，以使用 TensorRT 进行运行时推理阶段。


对于自动化安装，建议通过 [NVIDIA GPU Cloud](https://www.nvidia.com/en-us/gpu-cloud/) 使用 DIGITS，它附带了 DIGITS Docker 映像，可以在连接到本地 PC 或云实例的 GPU 上运行。或者，要从源安装 DIGITS，请首先从 GitHub 克隆 DIGITS 存储库: 


``` bash
$ git clone https://github.com/nvidia/DIGITS
```


然后完成 **[Building DIGITS](https://github.com/NVIDIA/DIGITS/blob/digits-6.0/docs/BuildDigits.md)** 文档下的步骤。


[`https://github.com/NVIDIA/DIGITS/blob/digits-6.0/docs/BuildDigits.md`](https://github.com/NVIDIA/DIGITS/blob/digits-6.0/docs/BuildDigits.md)


#### 启动 DIGITS 服务器


假设您的终端仍在 DIGITS 目录中，则可以通过运行 `digits-devserver` Python 脚本来启动网络服务器: 


`` 重击
$ ./digits-devserver 
  ___ ___ ___ ___ _____ ___
 |   \_ _/ __|_ _|_ _/ __|
 | |)| | (_ || | | | \__ \
 |___/___\___|___| |_| |___/ 5.1-开发


2017-04-17 13:19:02 [信息] 已加载 0 个职位。`
````


DIGITS 将在 `digits/jobs` 目录下存储用户作业（训练数据集和模型快照）。


要访问交互式 DIGITS 会话，请打开 Web 浏览器并导航至 `0.0.0.0:5000`。


> **注意**: 默认情况下，DIGITS 服务器将在端口 5000 上启动，但可以通过将 `--port` 参数传递给 `digits-devserver` 脚本来指定该端口。


##
<palign="right">下一个| <b><a href="jetpack-setup-CN.md">使用 JetPack 设置 Jetson</a></b>
<br/>
返回 | <b><a href="digits-workflow-CN.md">DIGITS 工作流程</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>