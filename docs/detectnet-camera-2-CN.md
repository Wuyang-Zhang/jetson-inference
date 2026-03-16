<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="detectnet-console-2-CN.md">返回</a> | <a href="detectnet-example-2-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>物体检测</sup></p>


# 运行实时摄像头检测演示


我们之前使用的 [`detectnet.cpp`](../examples/detectnet/detectnet.cpp) / [`detectnet.py`](../python/examples/detectnet.py) 示例也可用于实时摄像头流。  支持的相机类型包括: 


- MIPI CSI 相机 (`csi://0`)

- V4L2 相机 (`/dev/video0`)

- RTP/RTSP 流 (`rtsp://username:password@ip:port`)

- WebRTC 流 (`webrtc://@:port/stream_name`)


有关视频流和协议的更多信息，请参阅[相机流和多媒体](aux-streaming-CN.md) 页面。


使用 `--help` 运行程序以查看完整的选项列表 - 其中一些特定于 detectorNet 包括: 


- `--network` 标志，用于更改正在使用的[检测模型](detectnet-console-2-CN.md#pre-trained-detection-models-available)（默认为 SSD-Mobilenet-v2）。

- `--overlay` 标志，可以是 `box`、`labels`、`conf`、`track` 和 `none` 的逗号分隔组合

	- 默认值为 `--overlay=box,labels,conf`，显示框、标签和置信度值

- `--alpha` 值，设置叠加期间使用的 alpha 混合值（默认为 `120`）。

- `--threshold` 值，设置检测的最小阈值（默认为 `0.5`）。


以下是在摄像头源上启动程序的一些典型场景: 


#### C++


``` bash
$ ./detectnet csi://0                    # MIPI CSI camera
$ ./detectnet /dev/video0                # V4L2 camera
$ ./detectnet /dev/video0 output.mp4     # save to video file
```


#### Python


``` bash
$ ./detectnet.py csi://0                 # MIPI CSI camera
$ ./detectnet.py /dev/video0             # V4L2 camera
$ ./detectnet.py /dev/video0 output.mp4  # save to video file
```


> **注意**: 例如要使用的相机，请参阅 Jetson Wiki 的以下部分: <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Nano:&nbsp;&nbsp;[`https://eLinux.org/Jetson_Nano#Cameras`](https://elinux.org/Jetson_Nano#Cameras) <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Xavier: [`https://eLinux.org/Jetson_AGX_Xavier#Ecosystem_Products_.26_Cameras`](https://elinux.org/Jetson_AGX_Xavier#Ecosystem_Products_.26_Cameras) <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- TX1/TX2: 开发套件包括板载 MIPI CSI 传感器模块 (0V5693)<br/>


#### 可视化


OpenGL 窗口中显示的是实时摄像机流，上面覆盖着检测到的对象的边界框。  请注意，基于 SSD 的型号目前具有最高的性能。  这是使用 `coco-dog` 模型的一个: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-ssd-animals.jpg" width="800">


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-ssd-kitchen.jpg" width="800">


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-ssd-laptops.jpg" width="800">


如果在视频源中未检测到所需的对象或者您收到虚假检测，请尝试使用 `--threshold` 参数（默认值为 `0.5`）减小或增大检测阈值。


接下来，我们将介绍如何使用 Python 创建相机检测应用程序的代码。


##
<palign="right">下一个| <b><a href="detectnet-example-2-CN.md">编写您自己的目标检测程序</a></b>
<br/>
返回 | <b><a href="detectnet-console-2-CN.md">从图像中检测对象</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>