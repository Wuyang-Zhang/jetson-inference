<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="detectnet-camera-2-CN.md">返回</a> | <a href="detectnet-tao-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>物体检测</sup></p>


# 编写您自己的目标检测程序


在本教程的这一步中，我们将逐步创建您自己的 Python 脚本，以便仅用 10-15 行代码即可在实时摄像头输入上进行实时目标检测。  该程序将捕获视频帧并使用 [`detectNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#detectNet) 对象通过检测 DNN 对其进行处理。


完整的源代码可在存储库的 [`python/examples/my-detection.py`](../python/examples/my-detection.py) 文件中找到，但下面的指南将就像它们驻留在用户的主目录或您选择的任意目录中一样。  以下是我们将要演示的 Python 代码的快速预览: 


```` 蟒蛇
从 jetson_inference 导入 detectorNet
从 jetson_utils 导入 videoSource、videoOutput


net = detectorNet("ssd-mobilenet-v2"，阈值=0.5)
相机 = videoSource("csi://0") # '/dev/video0' 对于 V4L2
display = videoOutput("display://0") # 文件的“my_video.mp4”


while display.IsStreaming():
    img = 相机.Capture()


if img is None: # 捕获超时
        继续


检测= net.Detect(img)


显示.渲染(img)
    display.SetStatus("物体检测|网络{:.0f} FPS".format(net.GetNetworkFPS()))
````


YouTube 上还提供了此编码教程的视频截屏: 


<a href="https://www.youtube.com/watch?v=obt60r8ZeB0&list=PL5B692fm6--uQRRDTPsJDp4o0xbzkoyf8&index=12" target="_blank"><img src=https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/thumbnail_detectnet.jpg width="750"></a>
## 源代码


首先，打开您选择的文本编辑器并创建一个新文件。  下面我们假设您将其保存在主机设备上的用户主目录下，名称为 `~/my-detection.py`，但您可以将其命名并存储在您希望的位置。  如果您使用 Docker 容器，则需要将代码存储在 [已安装目录](aux-docker-CN.md#mounted-data-volumes) 中，类似于我们在 [图像识别 Python 示例](imagenet-example-python-2-CN.md#setting-up-the-project) 中所做的操作。


#### 导入模块


在源文件的顶部，我们将导入将在脚本中使用的 Python 模块。  添加 `import` 语句以加载用于目标检测和相机捕获的 [`jetson_inference`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson_inference.html) 和 [`jetson_utils`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson_utils.html) 模块。


``` python
from jetson_inference import detectNet
from jetson_utils import videoSource, videoOutput
```


> **注意**: 这些 Jetson 模块是在[构建存储库](building-repo-2-CN.md#compiling-the-project) 的 `sudo make install` 步骤中安装的。

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;如果您没有运行 `sudo make install`，那么在运行示例时将找不到这些包。


#### 加载检测模型


接下来使用以下行创建加载 [91-class](../data/networks/ssd_coco_labels.txt) SSD-Mobilenet-v2 模型的 [`detectNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#detectNet) 对象实例: 


``` python
# load the object detection model
net = detectNet("ssd-mobilenet-v2", threshold=0.5)
```


请注意，您可以将模型字符串更改为[此表](detectnet-console-2-CN.md#pre-trained-detection-models-available) 中的值之一以加载不同的检测模型。  出于说明目的，我们还将此处的检测阈值设置为默认值 `0.5` - 您可以稍后根据需要进行调整。


#### 打开相机流


要连接到相机设备进行流式传输，我们将创建 [`videoSource`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.utils.html#videoSource) 对象的实例: 


``` python
camera = videoSource("csi://0")      # '/dev/video0' for V4L2
```


传递给 `videoSource()` 的字符串实际上可以是任何有效的资源 URI，无论是相机、视频文件还是网络流。  有关视频流和协议的更多信息，请参阅[相机流和多媒体](aux-streaming-CN.md) 页面。


> **注意**: 要使用兼容的相机，请参阅 Jetson Wiki 的以下部分: <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Nano:&nbsp;&nbsp;[`https://eLinux.org/Jetson_Nano#Cameras`](https://elinux.org/Jetson_Nano#Cameras) <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Xavier: [`https://eLinux.org/Jetson_AGX_Xavier#Ecosystem_Products_.26_Cameras`](https://elinux.org/Jetson_AGX_Xavier#Ecosystem_Products_.26_Cameras) <br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- TX1/TX2: 开发套件包括板载 MIPI CSI 传感器模块 (0V5693)<br/>


#### 显示循环


接下来，我们将使用 [`videoOutput`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.utils.html#videoOutput) 对象创建一个视频输出接口，并创建一个将运行直到用户退出的主循环: 


```` 蟒蛇
display = videoOutput("display://0") # 文件的“my_video.mp4”


while display.IsStreaming():
	# 主循环将转到这里
````


请注意，下面的代码的其余部分应缩进在此 `while` 循环下方。  与上面类似，您可以将 URI 字符串替换为 [此页面](aux-streaming-CN.md) 上找到的其他类型的输出（例如视频文件等）。


#### 相机捕捉


主循环中发生的第一件事是从相机捕获下一个视频帧。  `camera.Capture()` 将等待，直到下一帧从相机发送并加载到 GPU 内存中。


```` 蟒蛇
	img = 相机.Capture()


if img is None: # 捕获超时
		继续
````


返回的图像将是一个 [`jetson_utils.cudaImage`](aux-image-CN.md#image-capsules-in-python) 对象，其中包含宽度、高度和像素格式等属性: 


```python
<jetson.utils.cudaImage>
  .ptr      # memory address (not typically used)
  .size     # size in bytes
  .shape    # (height,width,channels) tuple
  .width    # width in pixels
  .height   # height in pixels
  .channels # number of color channels
  .format   # format string
  .mapped   # true if ZeroCopy
```


有关从 Python 访问图像的更多信息，请参阅[使用 CUDA 进行图像操作](aux-image-CN.md) 页面。


#### 检测物体


接下来，检测网络使用 `net.Detect()` 函数处理图像。  它从 `camera.Capture()` 获取图像并返回检测列表: 


``` python
	detections = net.Detect(img)
```


该功能还将自动将检测结果覆盖在输入图像的顶部。


如果需要，您可以在此处添加 `print(detections)` 语句，每个检测结果的坐标、置信度和类别信息将打印到终端。  另请参阅 [`detectNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#detectNet) 文档，了解有关在自定义应用程序中直接访问所返回的 `Detection` 结构的不同成员的信息。


#### 渲染


最后，我们将使用 OpenGL 可视化结果并更新窗口标题以显示当前性能: 


``` python
	display.Render(img)
	display.SetStatus("Object Detection | Network {:.0f} FPS".format(net.GetNetworkFPS()))
```


`Render()` 函数将自动翻转后备缓冲区并将图像呈现在屏幕上。


#### 来源清单


就是这样！  为了完整起见，以下是我们刚刚创建的 Python 脚本的完整源代码: 


```` 蟒蛇
从 jetson_inference 导入 detectorNet
从 jetson_utils 导入 videoSource、videoOutput


net = detectorNet("ssd-mobilenet-v2"，阈值=0.5)
相机 = videoSource("csi://0") # '/dev/video0' 对于 V4L2
display = videoOutput("display://0") # 文件的“my_video.mp4”


while display.IsStreaming():
    img = 相机.Capture()


if img is None: # 捕获超时
        继续


检测= net.Detect(img)


显示.渲染(img)
    display.SetStatus("物体检测|网络{:.0f} FPS".format(net.GetNetworkFPS()))
````


请注意，此版本假设您使用的是 MIPI CSI 摄像头。  有关更改它以使用不同类型的输入的信息，请参阅上面的 [`Opening the Camera Stream`](#opening-the-camera-stream) 部分。


## 运行程序


要运行我们刚刚编写的应用程序，只需使用 Python 解释器从终端启动它即可: 


``` bash
$ python3 my-detection.py
```


要调整结果，您可以尝试更改与检测阈值一起加载的模型。  玩得开心！


<palign="right">下一个| <b><a href="detectnet-tao-CN.md">使用 TAO 检测模型</a></b>
<br/>
返回 | <b><a href="detectnet-camera-2-CN.md">运行实时摄像头检测演示</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>