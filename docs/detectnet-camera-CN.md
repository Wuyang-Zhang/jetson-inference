<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="detectnet-console-CN.md">返回</a> | <a href="segnet-dataset-CN.md">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>物体检测</sup></p>


# 运行实时摄像头检测演示


接下来我们有一个适用于 C++ 和 Python 的实时目标检测相机演示: 


- [`detectnet-camera.cpp`](../examples/detectnet-camera/detectnet-camera.cpp) (C++)

- [`detectnet-camera.py`](../python/examples/detectnet-camera.py) (Python)


与前面的 [`detectnet-console`](detectnet-console-CN.md) 示例类似，这些摄像头应用程序使用检测网络，只不过它们处理来自摄像头的实时视频源。  `detectnet-camera` 接受 4 个可选命令行参数: 


- `--network` 标志设置分类模型（默认为 PedNet）

	- 有关可供使用的网络，请参阅[可用的预训练检测模型](detectnet-console-CN.md#pre-trained-detection-models-available)。

- `--camera` 标志设置要使用的相机设备

	- MIPI CSI 相机通过指定传感器索引（`0` 或 `1` 等）来使用

	- V4L2 USB 摄像头通过指定其 `/dev/video` 节点（`/dev/video0`、`/dev/video1` 等）来使用

	- 默认使用 MIPI CSI 传感器 0 (`--camera=0`)

- `--width` 和 `--height` 标志设置相机分辨率（默认为 `1280x720`）

	- 分辨率应设置为相机支持的格式。

- 使用以下命令查询可用的格式:   
          ``` bash
          $ sudo apt-get install v4l-utils
          $ v4l2-ctl --list-formats-ext
          ```


您可以根据需要组合使用这些标志，并且还有其他命令行参数可用于加载自定义模型。  使用 `--help` 标志启动应用程序以接收更多信息，或参阅 [`Examples`](../README-CN.md#code-examples) 自述文件。


以下是启动该程序的一些典型场景: 


#### C++


``` bash
$ ./detectnet-camera                          # using PedNet,  default MIPI CSI camera (1280x720)
$ ./detectnet-camera --network=facenet        # using FaceNet, default MIPI CSI camera (1280x720)
$ ./detectnet-camera --camera=/dev/video0     # using PedNet,  V4L2 camera /dev/video0 (1280x720)
$ ./detectnet-camera --width=640 --height=480 # using PedNet,  default MIPI CSI camera (640x480)
```


#### Python


``` bash
$ ./detectnet-camera.py                          # using PedNet,  default MIPI CSI camera (1280x720)
$ ./detectnet-camera.py --network=facenet        # using FaceNet, default MIPI CSI camera (1280x720)
$ ./detectnet-camera.py --camera=/dev/video0     # using PedNet,  V4L2 camera /dev/video0 (1280x720)
$ ./detectnet-camera.py --width=640 --height=480 # using PedNet,  default MIPI CSI camera (640x480)
```


> **注意**: 例如要使用的相机，请参阅 Jetson Wiki 的以下部分: <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Nano:&nbsp;&nbsp;[`https://eLinux.org/Jetson_Nano#Cameras`](https://elinux.org/Jetson_Nano#Cameras) <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Xavier: [`https://eLinux.org/Jetson_AGX_Xavier#Ecosystem_Products_.26_Cameras`](https://elinux.org/Jetson_AGX_Xavier#Ecosystem_Products_.26_Cameras) <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- TX1/TX2: 开发套件包括板载 MIPI CSI 传感器模块 (0V5693)<br/>


#### 可视化


OpenGL 窗口中显示的是实时摄像机流，上面覆盖着检测到的对象的边界框。  请注意，基于 SSD 的型号目前具有最高的性能。  这是使用 `coco-dog` 模型的一个: 


`` 重击
# C++
$ ./detectnet-camera --network=coco-dog


# Python
$ ./detectnet-camera.py --network=coco-dog
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet_camera_dog.jpg" width="800">


<br/>


##
<palign="right">下一个| <b><a href="segnet-dataset-CN.md">使用 SegNet 进行语义分割</a></b>
<br/>
返回 | <b><a href="detectnet-console-CN.md">从命令行检测对象</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>