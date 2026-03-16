<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="webrtc-server-CN.md">返回</a> | <a href="webrtc-flask-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>Web应用程序框架</sup></s></p>


# HTML / JavaScript


此存储库中包含使用 WebRTC 的各种示例 Web 应用程序，可在 [`jetson-inference/python/www`](../python/www) 下找到，例如: 


````

+ Python/

  + 万维网/

- dash # Plotly 仪表板
    - html # 核心 HTML/JavaScript
    - 烧瓶 # 烧瓶 + 休息
    - 识别器#互动训练
````


其中每一个都演示了 WebRTC 与不同的基于 Python 的 Web 服务器框架的集成，用于构建您自己的 AI 驱动的交互式 Web 应用程序。  它们通常具有类似的组件，如下所示: 


````

- app.py # 用于运行网络服务器的服务器端Python代码

- Stream.py # 用于 WebRTC 流/推理的服务器端 Python 代码

- webrtc.js # 客户端 WebRTC JavaScript 代码

- index.html # 客户端 HTML 代码

````


第一个示例是最简单的，重点介绍了播放/发送 WebRTC 流和应用 DNN 推理所需的核心 HTML/JavaScript 代码。  如果您已经有一个首选的前端可以集成，您可以将其应用到任何选择的 Web 框架。


## 运行示例


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/webrtc-html.jpg" width="600">


启动 app.py 将启动一个内置的 Python Web 服务器（易于使用，但不适用于生产，并且可以轻松更改）以及运行 WebRTC 捕获/传输和推理代码的独立流线程: 


``` bash
$ cd jetson-inference/python/www/html
$ python3 app.py --classification  # see below for other DNN options
```


> **注意**: 接收浏览器网络摄像头需要启用 [HTTPS/SSL](webrtc-server-CN.md#enabling-https--ssl)


然后，您应该能够将浏览器导航到 `https://<JETSON-IP>:8050` 并启动流。  8050 是这些 web 应用程序示例使用的默认端口，但您可以使用 `--port=N` 命令行参数更改它。  默认情况下，它也配置为 WebRTC 输入和输出，但如果您想使用不同的[视频输入设备](aux-streaming-CN.md#input-streams)，您可以使用 `--input` 参数进行设置（例如，`--input=/dev/video0` 表示直接连接到 Jetson 的 V4L2 摄像头）。


### 加载 DNN 模型


此示例支持一次运行一个 DNN 模型 - 分类、检测、分割、姿势估计、动作识别或背景去除（后续示例支持模拟运行多个模型）。  您可以在启动应用程序时更改 DNN，如下所示: 


``` bash
$ python3 app.py --classification --model=resnet18
$ python3 app.py --detection --model=ssd-mobilenet-v2
$ python3 app.py --segmentation --model=fcn-resnet18-mhp
$ python3 app.py --pose --model=resnet18-body
$ python3 app.py --action --model=resnet18-kinetics
$ python3 app.py --background --model=u2net
```


省略可选的 `--model` 参数将加载该网络的默认模型，或者如果您在 PyTorch 中训练并导出到 ONNX 的教程中拥有自己的自定义分类或检测模型，则可以使用扩展命令行参数来加载它（例如使用[此处](pytorch-cat-dog-CN.md#processing-images-with-tensorrt)进行分类和[此处](pytorch-ssd-CN.md#processing-images-with-tensorrt)进行检测）。


## HTML 元素


查阅 [`index.html`](../python/www/html/index.html) 的来源，让我们演练一下使用 WebRTC 构建自己的网页的最重要步骤: 


1.  JavaScript 导入


``` html
<script type='text/javascript' src='https://webrtc.github.io/adapter/adapter-latest.js'></script>
<script type='text/javascript' src='/webrtc.js'></script>
```


2.  HTML视频播放器


这应该进入页面 `<body>` 来创建视频播放器元素: 


``` html
<video id="video-player" autoplay controls playsinline muted></video>
```


3.  开始播放


``` javascript
// playStream() is a helper function from webrtc.js that connects the specified WebRTC stream to the video player
// getWebSocketURL() is a helper function that makes a URL path of the form:  wss://<SERVER-IP>:8554/output
playStream(getWebsocketURL('output'), document.getElementById('video-player'));
```


通常，此 JavaScript 函数将在 `window.onload()` 中调用，或者从事件处理程序（如按钮的 `onclick()` 事件）中调用（如本例所示）。  尽管上面没有提到，但还包含用于枚举浏览器网络摄像头并通过 WebRTC 将其作为视频输入发送到 Jetson 的代码。  您基本上可以将此代码（以及 `webrtc.js`）复制并粘贴到任何项目中以启用 WebRTC。



<palign="right">下一个| <b><a href="webrtc-flask-CN.md">Flask + REST</a></b>
<br/>
返回 | <b><a href="webrtc-server-CN.md">WebRTC 服务器</a></p>
</b><palign="center"><sup>© 2016-2023 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>