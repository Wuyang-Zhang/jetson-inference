<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="webrtc-html-CN.md">返回</a> | <a href="webrtc-dash-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>Web应用程序框架</sup></s></p>


# 烧瓶+休息


[Flask](https://flask.palletsprojects.com/en/2.2.x/) 是一种流行的 Python Web 微框架，它将 HTTP/HTTPS 请求路由到用户实现的 Python 函数。  您还可以使用它轻松处理后端 REST 请求，客户端可以使用它来动态控制属性并根据用户输入触发来自前端的内容。  这个交互式 DNN 游乐场（位于 [`python/www/flask`](../python/www/flask) 下）具有多个视觉模型，您可以从 Web 应用程序同时切换这些模型，并使用 UI 实时控制其各种设置: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/webrtc-flask.jpg" width="600">


它还使用 [Bootstrap CSS](https://getbootstrap.com/) 来设置 UI 组件的样式。  本例的主要源文件如下: 


  * [`app.py`](../python/www/flask/app.py)（网络服务器）

  * [`stream.py`](../python/www/flask/stream.py)（WebRTC 流线程）

  * [`model.py`](../python/www/flask/model.py)（DNN 推理）

  * [`index.html`](../python/www/flask/templates/index.html)（前端演示）


## 运行示例


启动 app.py 将启动 Flask Web 服务器，以及运行 WebRTC 捕获/传输和推理代码的流线程: 


``` bash
$ cd jetson-inference/python/www/flask
$ pip3 install -r requirements.txt
$ python3 app.py --detection=ssd-mobilenet-v2 --pose=resnet18-hand --action=resnet18-kinetics
```


> **注意**: 接收浏览器网络摄像头需要启用 [HTTPS/SSL](webrtc-server-CN.md#enabling-https--ssl)


然后，您应该能够将浏览器导航到 `https://<JETSON-IP>:8050` 并启动流。  8050 是使用的默认端口，但您可以使用 `--port=N` 命令行参数更改它。  默认情况下，它也配置为 WebRTC 输入和输出，但如果您想使用不同的[视频输入设备](aux-streaming-CN.md#input-streams)，您可以使用 `--input` 参数进行设置（例如，`--input=/dev/video0` 表示直接连接到 Jetson 的 V4L2 摄像头）。


### 加载 DNN 模型


此示例支持加载多个可同时运行的 DNN 模型（分类、检测、分割、姿势估计、动作识别和背景去除）。  当您启动应用程序时，您可以选择加载哪些模型，如下所示: 


``` bash
$ python3 app.py \
    --classification=resnet18 \
    --detection=ssd-mobilenet-v2 \
    --segmentation=fcn-resnet18-mhp \
    --pose=resnet18-body \
    --action=resnet18-kinetics \
    --background=u2net
```


> **注意**: 根据您的 Jetson 和后台进程，您可能没有足够的内存来一次加载所有这些模型，或者没有足够的计算能力来实时运行它们。  请参阅[安装交换](pytorch-transfer-learning-CN.md#mounting-swap) 和[禁用桌面 GUI](pytorch-transfer-learning-CN.md#disabling-the-desktop-gui) 以节省内存。


要列出可用的内置模型，您可以运行 `app.py --help` 或查看 [`data/networks/models.json`](../data/networks/models.json)。  每个型号都有一个可扩展的下拉菜单用于打开/关闭它，以及用于更改其设置的交互式控件。  用于实现这些的客户端/服务器通信是使用 REST JSON 查询完成的，这将在下面讨论。


## 其余查询


此应用程序采用[上一个示例](webrtc-html-CN.md) 中用于流式传输 WebRTC 的核心 HTML/JavaScript 代码，并使用 REST JSON 查询构建该代码，以动态更新各种组件和参数。  您可以在 [app.py](../python/www/flask/app.py) 中查看这些后端存根，JavaScript 在 [index.html](../python/www/flask/templates/index.html) 中从客户端查询这些存根。  模板和宏用于减少添加新设置的样板代码量: 


```` 蟒蛇
# 后端 - app.py (Python)
@app.route('/classification/enabled',methods=['GET', 'PUT'])
defclassification_enabled():
   返回rest_property(stream.models['classification'].IsEnabled,stream.models['classification'].SetEnabled, bool)


@app.route('/classification/confidence_threshold',methods=['GET', 'PUT'])
defclassification_confidence_threshold():
   返回rest_property(stream.models['classification'].net.GetThreshold,stream.models['classification'].net.SetThreshold,float)


# 前端 - index.html (Jinja/HTML/JavaScript)
{{ checkbox('classification_enabled', '/classification/enabled', '分类启用') }}
{{ slider('classification_confidence_threshold', '/classification/confidence_threshold', '置信阈值') }}
````


上面的这些代码片段实现了分类模型的控制，还有其他代码片段用于不同类型的 DNN。


[`rest_property()`](../python/www/flask/utils.py) 是 Python 中的后端实用程序函数，用于处理用于获取/设置用户定义属性的 `GET` 和 `PUT` REST 请求。  [`checkbox()`](../python/www/flask/templates/macros.html) 和 [`slider()`](../python/www/flask/templates/macros.html) 是 Jinja 宏，它们呈现控件的 HTML 组件和用于执行 REST 查询的 JavaScript。  如果您想知道 index.html 中的 `{{ ... }}` 代码是什么，这些是动态 [Jinja](https://jinja.palletsprojects.com/en/3.1.x/templates/) 模板表达式，当 Flask 处理来自客户端的请求时，它们在服务器端进行评估以生成页面内容。


<palign="right">下一个| <b><a href="webrtc-dash-CN.md">Plotly 仪表板</a></b>
<br/>
返回 | <b><a href="webrtc-html-CN.md">HTML / JavaScript</a></p>
</b><palign="center"><sup>© 2016-2023 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>