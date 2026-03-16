<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="digits-setup-CN.md">返回</a> | <a href="building-repo-CN.md">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>系统设置</sup></p>


# 使用 JetPack 设置 Jetson


> **注意**: 如果您的 Jetson Nano 已经设置了 [SD 卡映像](https://developer.nvidia.com/embedded/learn/get-started-jetson-nano-devkit#write)（其中包括 JetPack 组件），或者您的 Jetson 已经设置了 JetPack，您可以跳过此步骤并继续 [`Building the Repo`](building-repo-CN.md)


将最新的 **[JetPack](https://developer.nvidia.com/embedded/jetpack)** 下载到您的主机 PC。  除了使用最新的主板支持包 (BSP) 刷新 Jetson 之外，JetPack 还会自动为主机安装 CUDA Toolkit 等工具。  有关功能和已安装软件包的完整列表，请参阅 JetPack [发行说明](https://developer.nvidia.com/embedded/jetpack-notes)。


从上面的链接下载 JetPack 后，使用以下命令从主机 PC 运行它: 


``` bash 
$ cd <directory where you downloaded JetPack>
$ chmod +x JetPack-L4T-<version>-linux-x64.run 
$ ./JetPack-L4T-<version>-linux-x64.run 
```


JetPack GUI 将启动。  按照分步 **[安装指南](http://docs.nvidia.com/jetpack-l4t/index.html#developertools/mobile/jetpack/l4t/3.0/jetpack_l4t_install.htm)** 完成设置。  刚开始时，JetPack 将确认您正在为哪一代 Jetson 开发。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/jetpack-platform.png" width="450">


如果您使用的是 TX1，请选择 Jetson TX1；如果您使用的是 TX2，请选择 Jetson TX2，然后按 `Next` 继续。


下一个屏幕将列出可安装的软件包。  安装到主机的软件包列在顶部的 `Host - Ubuntu` 下拉列表中，而用于 Jetson 的软件包则显示在底部附近。  您可以通过单击其 `Action` 列来选择或取消选择要安装的单个软件包。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/jetpack-downloads.png" width="500">


由于 CUDA 将在主机上用于训练 DNN，因此建议通过单击右上角的单选按钮选择完整安装。  然后按 `Next` 开始设置。  JetPack 将下载并安装一系列软件包。  请注意，如果您稍后需要，所有 .deb 包都存储在 `jetpack_downloads` 子目录下。


下载完成安装后，JetPack 将进入安装后阶段，在此阶段将使用 L4T BSP 刷新 JetPack。  您需要通过开发套件中包含的微型 USB 端口和电缆将 Jetson 连接到主机 PC。  然后按住“恢复”按钮，同时按下并松开“重置”，让 Jetson 进入恢复模式。  如果在连接 micro-USB 电缆并使 Jetson 进入恢复模式后从主机 PC 键入 `lsusb`，您应该会看到 NVIDIA 设备出现在 USB 设备列表下。  JetPack 使用主机的 micro-USB 连接将 L4T BSP 闪存到 Jetson。


刷新后，Jetson 将重新启动，如果连接到 HDMI 显示器，将启动至 Ubuntu 桌面。  此后，JetPack 通过 SSH 从主机连接到 Jetson，以将其他软件包安装到 Jetson，例如 CUDA Toolkit、cuDNN 和 TensorRT 的 ARM aarch64 版本。  为了使 JetPack 能够通过 SSH 访问 Jetson，主机 PC 应通过以太网连接到 Jetson。  这可以通过直接从主机到 Jetson 运行以太网电缆，或者将两个设备连接到路由器或交换机来完成 - JetPack GUI 会要求您确认正在使用哪种网络场景。


请参阅 **[JetPack 安装指南](http://docs.nvidia.com/jetpack-l4t/index.html#developertools/mobile/jetpack/l4t/3.0/jetpack_l4t_install.htm)** 了解安装 JetPack 和刷新 Jetson 的完整说明。


##
<palign="right">下一个| <b><a href="building-repo-CN.md">从源代码构建存储库</a></b>
<br/>
返回 | <b><a href="digits-setup-CN.md">DIGITS 系统设置</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>