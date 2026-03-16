<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="segnet-console-2-CN.md">返回</a> | <a href="posenet-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>语义分割</sup></s></p>


# 运行实时摄像头分割演示
我们之前使用的 [`segnet.cpp`](../examples/segnet/segnet.cpp) / [`segnet.py`](../python/examples/segnet.py) 示例也可用于实时摄像头流。  支持的相机类型包括: 


- MIPI CSI 相机 (`csi://0`)

- V4L2 相机 (`/dev/video0`)

- RTP/RTSP 流 (`rtsp://username:password@ip:port`)


有关视频流和协议的更多信息，请参阅[相机流和多媒体](aux-streaming-CN.md) 页面。


使用 `--help` 运行程序以查看完整的选项列表 - 其中一些特定于 segNet 的选项包括: 


- 可选 `--network` 标志更改正在使用的分段模型（请参阅[可用网络](segnet-console-2-CN.md#pre-trained-segmentation-models-available)）

- 可选 `--visualize` 标志接受 `mask` 和/或 `overlay` 模式（默认为 `overlay`）

- 可选 `--alpha` 标志设置叠加层的 Alpha 混合值（默认为 `120`）

- 可选 `--filter-mode` 标志接受 `point` 或 `linear` 采样（默认为 `linear`）


以下是启动该程序的一些典型场景 - 请参阅[此表](segnet-console-2-CN.md#pre-trained-segmentation-models-available) 了解可用的模型。


#### C++


``` bash
$ ./segnet --network=<model> csi://0                    # MIPI CSI camera
$ ./segnet --network=<model> /dev/video0                # V4L2 camera
$ ./segnet --network=<model> /dev/video0 output.mp4     # save to video file
```


#### Python


``` bash
$ ./segnet.py --network=<model> csi://0                 # MIPI CSI camera
$ ./segnet.py --network=<model> /dev/video0             # V4L2 camera
$ ./segnet.py --network=<model> /dev/video0 output.mp4  # save to video file
```


> **注意**: 例如要使用的相机，请参阅 Jetson Wiki 的以下部分: <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Nano:&nbsp;&nbsp;[`https://eLinux.org/Jetson_Nano#Cameras`](https://elinux.org/Jetson_Nano#Cameras) <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Xavier: [`https://eLinux.org/Jetson_AGX_Xavier#Ecosystem_Products_.26_Cameras`](https://elinux.org/Jetson_AGX_Xavier#Ecosystem_Products_.26_Cameras) <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- TX1/TX2: 开发套件包括板载 MIPI CSI 传感器模块 (0V5693)<br/>


#### 可视化


OpenGL 窗口中显示的是覆盖有分割输出的实时摄像机流，以及清晰的实体分割蒙版。  以下是一些与[不同型号](segnet-console-2-CN.md#pre-trained-segmentation-models-available)一起使用的示例，可供尝试: 


`` 重击
# C++
$ ./segnet --network=fcn-resnet18-mhp csi://0


# Python
$ ./segnet.py --network=fcn-resnet18-mhp csi://0
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-mhp-camera.jpg" width="900">


`` 重击
# C++
$ ./segnet --network=fcn-resnet18-sun csi://0


# Python
$ ./segnet.py --network=fcn-resnet18-sun csi://0
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-sun-camera.jpg" width="900">


`` 重击
# C++
$ ./segnet --network=fcn-resnet18-deepscene csi://0


# Python
$ ./segnet.py --network=fcn-resnet18-deepscene csi://0
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-deepscene-camera.jpg" width="900">


请随意尝试针对室内和室外环境的不同模型和分辨率。



##
<palign="right">下一个| <b><a href="posenet-CN.md">使用 PoseNet 进行姿势估计</a></b>
<br/>
返回 | <b><a href="segnet-console-2-CN.md">从命令行分割图像</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>