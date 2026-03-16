<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="imagenet-example-CN.md">返回</a> | <a href="imagenet-training-CN.md">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>图像识别</sup></p>


# 运行实时摄像头识别演示


接下来我们有一个适用于 C++ 和 Python 的实时图像识别相机演示: 


- [`imagenet-camera.cpp`](../examples/imagenet-camera/imagenet-camera.cpp) (C++)

- [`imagenet-camera.py`](../python/examples/imagenet-camera.py) (Python)


与前面的 [`imagenet-console`](imagenet-console-CN.md) 示例类似，相机应用程序构建到 `/aarch64/bin` 目录中。它们在带有 OpenGL 渲染的实时摄像头流上运行，并接受 4 个可选的命令行参数: 


- `--network` 标志设置分类模型（默认为 GoogleNet）

	- 有关可用的网络，请参阅[下载其他分类模型](imagenet-console-CN.md#downloading-other-classification-models)。

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
$ ./imagenet-camera                          # using GoogleNet, default MIPI CSI camera (1280x720)
$ ./imagenet-camera --network=resnet-18      # using ResNet-18, default MIPI CSI camera (1280x720)
$ ./imagenet-camera --camera=/dev/video0     # using GoogleNet, V4L2 camera /dev/video0 (1280x720)
$ ./imagenet-camera --width=640 --height=480 # using GoogleNet, default MIPI CSI camera (640x480)
```


#### Python


``` bash
$ ./imagenet-camera.py                          # using GoogleNet, default MIPI CSI camera (1280x720)
$ ./imagenet-camera.py --network=resnet-18      # using ResNet-18, default MIPI CSI camera (1280x720)
$ ./imagenet-camera.py --camera=/dev/video0     # using GoogleNet, V4L2 camera /dev/video0 (1280x720)
$ ./imagenet-camera.py --width=640 --height=480 # using GoogleNet, default MIPI CSI camera (640x480)
```


> **注意**: 例如要使用的相机，请参阅 Jetson Wiki 的以下部分: <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Nano:&nbsp;&nbsp;[`https://eLinux.org/Jetson_Nano#Cameras`](https://elinux.org/Jetson_Nano#Cameras) <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Xavier: [`https://eLinux.org/Jetson_AGX_Xavier#Ecosystem_Products_.26_Cameras`](https://elinux.org/Jetson_AGX_Xavier#Ecosystem_Products_.26_Cameras) <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- TX1/TX2: 开发套件包括板载 MIPI CSI 传感器模块 (0V5693)<br/>


OpenGL 窗口中显示的是实时摄像机流、分类对象名称、分类对象的置信度以及网络的帧速率。  在 Jetson Nano 上，GoogleNet 和 ResNet-18 的帧率最高可达约 75 FPS（在其他 Jetson 上更快）。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet_camera_bear.jpg" width="800">
<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet_camera_camel.jpg" width="800">
<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet_camera_triceratops.jpg" width="800">


该应用程序可以识别多达 1000 种不同类型的对象，因为分类模型是在包含 1000 个对象类别的 ILSVRC ImageNet 数据集上进行训练的。  1000 种对象的名称映射，您可以在 [`data/networks/ilsvrc12_synset_words.txt`](http://github.com/dusty-nv/jetson-inference/blob/master/data/networks/ilsvrc12_synset_words.txt) 下的存储库中找到


接下来，我们将在自定义数据集上重新训练图像识别网络。


##
<palign="right">下一个| <b><a href="imagenet-training-CN.md">重新训练识别网络</a></b>
<br/>
返回 | <b><a href="imagenet-example-CN.md">编写自己的图像识别程序</a></b></p>
<palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>