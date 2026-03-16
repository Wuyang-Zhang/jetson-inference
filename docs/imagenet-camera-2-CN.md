<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="imagenet-example-2-CN.md">返回</a> | <a href="imagenet-tagging-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>图像分类</sup></p>


# 运行实时摄像头识别演示


我们之前使用的 [`imagenet.cpp`](../examples/imagenet/imagenet.cpp) / [`imagenet.py`](../python/examples/imagenet.py) 示例也可用于实时摄像头流。  支持的相机类型包括: 


- MIPI CSI 相机 (`csi://0`)

- V4L2 相机 (`/dev/video0`)

- RTP/RTSP 流 (`rtsp://username:password@ip:port`)


有关视频流和协议的更多信息，请参阅[相机流和多媒体](aux-streaming-CN.md) 页面。


以下是在摄像头源上启动程序的一些典型场景（运行 `--help` 以获得更多选项）: 


#### C++


``` bash
$ ./imagenet csi://0                    # MIPI CSI camera
$ ./imagenet /dev/video0                # V4L2 camera
$ ./imagenet /dev/video0 output.mp4     # save to video file
```


#### Python


``` bash
$ ./imagenet.py csi://0                 # MIPI CSI camera
$ ./imagenet.py /dev/video0             # V4L2 camera
$ ./imagenet.py /dev/video0 output.mp4  # save to video file
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


Hello AI World 图像分类教程的这一部分到此结束。  接下来，我们将开始使用目标检测网络，它为我们提供每帧多个对象的边界框坐标。


##
<palign="right">下一个| <b><a href="imagenet-tagging-CN.md">图像标记的多标签分类</a></b>
<br/>
返回 | <b><a href="imagenet-example-2-CN.md">编写自己的图像识别程序</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>