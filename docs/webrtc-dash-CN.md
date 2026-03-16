<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="webrtc-flask-CN.md">返回</a> | <a href="webrtc-recognizer-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>Web应用程序框架</sup></s></p>


# 情节仪表板


[Plotly Dash](https://plotly.com/dash/) 是一个基于 Python 的 Web 框架，用于构建数据驱动的仪表板和交互式 UI。  在前端，它使用 [React.js](https://reactjs.org/) 客户端，它将状态更改连接到服务器上运行的 Python 回调。  有了它，您可以快速开发与后端处理管道和数据分析集成的丰富可视化效果。  在此示例中（位于 [`python/www/dash`](../python/www/dash) 下），用户可以动态创建流、加载 DNN 模型、可视化事件以及设置由事件触发的可扩展操作: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/webrtc-dash.jpg" width="1000">


和以前一样，它使用 WebRTC 传输实时视频，使用 TensorRT 进行推理。  请注意，该示例仍在开发中作为概念验证。  作为参考，该项目的结构如下: 


  * [`app.py`](../python/www/dash/app.py)（网络服务器）

  * [`actions/`](../python/www/dash/actions)（操作插件）

  * [`assets/`](../python/www/dash/assets)（CSS/JavaScript/图像）

  * [`layout/`](../python/www/dash/layout)（UI 组件）

  * [`server/`](../python/www/dash/server)（后端流/处理）


## 运行示例


启动 app.py 将启动仪表板，以及运行 WebRTC 捕获/传输、推理和事件/操作触发器的后端进程。  虽然前面的示例在与 Web 服务器相同的进程内的线程中运行流式传输，但在其自己的独立进程中运行流式传输后端允许多个 Web 服务器工作人员在部署中实现负载平衡（即使用 [Gunicorn](https://gunicorn.org/) 或其他生产 WSGI Web 服务器）。  这些进程通过 REST JSON 消息共享元数据（视频数据驻留在流处理进程中）。


``` bash
$ cd jetson-inference/python/www/dash
$ pip3 install -r requirements.txt
$ python3 app.py --detection=ssd-mobilenet-v2 --pose=resnet18-hand --action=resnet18-kinetics
```


> **注意**: 建议运行服务器时启用[HTTPS/SSL](webrtc-server-CN.md#enabling-https--ssl)


然后，您应该能够将浏览器导航到 `https://<JETSON-IP>:8050` 并开始配置系统。  8050 是使用的默认端口，但您可以使用 `--port=N` 命令行参数更改它。  您还可以通过 `data/config.json` 更改各种设置（首次运行应用程序时使用 [`config.py`](../python/www/dash/config.py) 中的默认值写入）。


### 加载 DNN 模型


首先要做的是通过进入 `Models -> Load Model` 菜单加载一些 DNN 模型。  目前，除了导入自定义训练的 ONNX 模型之外，它还支持加载预训练的分类和检测模型: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/webrtc-dash-model-load.jpg" width="400">


在对话框中选择模型后，加载后您应该会在主页底部看到一条状态消息。  通常这需要 5-10 秒，但如果这是您第一次加载该特定模型，TensorRT 可能需要几分钟来生成网络引擎（为避免这种延迟，建议在运行 web 应用程序之前使用 imagenet.py/detectnet.py 程序之一加载模型一次）


#### 导入模型


要加载您使用 Hello AI World 教程中的 PyTorch 训练的自定义分类或检测 ONNX 模型（即使用 [`train.py`](pytorch-cat-dog-CN.md#re-training-resnet-18-model) 或 [`train_ssd.py`](https://github.com/dusty-nv/jetson-inference/blob/dev/docs/pytorch-collect-detection.md#training-your-model)），请切换到 `Import` 选项卡: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/webrtc-dash-model-import.jpg" width="400">


预计您的模型已经存在于服务器上的某个位置，并且您可以使用与 imagenet.py/detectnet.py 相同的名称填写输入/输出层名称（例如使用[此处](pytorch-cat-dog-CN.md#processing-images-with-tensorrt)进行分类和[此处](pytorch-ssd-CN.md#processing-images-with-tensorrt)进行检测）


### 创建流


通过打开 `Streams -> Add Stream` 菜单，您可以指定要流式传输的视频源以及要应用的 DNN 模型: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/webrtc-dash-add-stream.jpg" width="400">


连接到各种类型的相机和视频设备的语法可以在[相机流和多媒体](aux-streaming-CN.md)页面上找到。  请注意，此示例尚不支持来自浏览器网络摄像头的 WebRTC 输入，但将会添加。  添加流后，您可以通过从 `Streams` 菜单中选择它来打开其视频播放器。  面板小部件可拖动、可调整大小和可折叠。


## 活动


当 DNN 的输出发生变化（即分类结果发生变化或检测到新对象）时，它会在系统中记录一个事件。  可以通过从 `Events` 菜单下打开事件表来实时监控这些事件: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/webrtc-dash-event-table.jpg" width="750">


您可以按表中的列进行过滤和排序，并在事件时间线中可视化结果: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/webrtc-dash-event-timeline.jpg" width="750">


该图的 y 轴显示置信度得分，x 轴显示时间，每个对象类在图表中获得不同的轨迹。  快速创建不同类型的动态[图表](https://plotly.com/python/)和[表格](https://dash.plotly.com/datatable)是Plotly Dash的强大功能，您可以在创建自己的应用程序时扩展这些功能。


## 行动


操作是过滤事件并在发生此类事件时触发用户定义的代码（例如警报/通知、播放声音或生成物理响应）的插件。  操作的属性通过 Web UI 公开，以便可以在运行时进行配置: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/webrtc-dash-actions.jpg" width="400">


您可以在项目的 [`actions/`](../python/www/dash/actions) 目录下添加自己的操作类型，应用程序将在启动时自动加载它们，并可从 UI 中进行选择。  用户可以创建一种操作的多个实例，每个实例都具有可以控制的独立设置。


例如，以下是仅将消息记录到服务器终端的操作模板: 


```` 蟒蛇
从服务器导入操作


类我的动作（动作）: 
    def __init__(自身):
        超级().__init__()


def on_event(自身，事件): 
        如果 event.label == 'person' 且 event.score > 0.5: 
            print("检测到一个人！") # 做点什么        
````



操作插件应实现 `on_event()` 回调，该回调从系统接收所有新的和更新的事件。  然后，插件在触发某种响应之前按特定于域的标准过滤事件。  有关可访问的事件属性，请参阅 [`Event`](../python/www/dash/server/event.py) 类。  该代码全部在后端流处理中运行，并且可以访问低级数据流而不影响性能。


### 过滤器


操作可以实现自定义事件过滤逻辑，和/或从 [`EventFilter`](../python/www/dash/server/filter.py) 混合继承，该混合实现一些默认过滤（如类标签、最小置信度得分、最小帧数等），可以使用 `filter()` 函数调用。  [`BrowserAlert`](../python/www/dash/actions/alert.py) 插件使用以下内容。  然后，它检查/设置一个属性 (`alert_triggered`)，以防止同一事件触发多个警报: 


```` 蟒蛇
从服务器导入服务器、操作、事件过滤器


类 BrowserAlert(Action, EventFilter):
    ”“”
    触发浏览器警报并支持事件过滤的操作。
    ”“”
    def __init__(自身):
        超级(BrowserAlert, self).__init__()


def on_event(自身，事件): 
        如果 self.filter(event) 而不是 hasattr(event, 'alert_triggered'): 
            Server.alert(f"检测到'{event.label}'({event.maxScore * 100:.1f}%)")
            event.alert_triggered = True
````


在一些更高级的场景中，您可能希望在事件的其他方面发生变化时重新触发操作（例如，置信度分数出现显着偏差，或者超过检测到的时间量）


### 特性


具有 `@property` 装饰器的插件将自动将这些属性公开给 UI，以便用户可以在运行时动态修改它们。  客户端/服务器通信使用 REST JSON 查询透明地进行。  例如，来自 [`EventFilter`](../python/www/dash/server/filter.py)


```` 蟒蛇
@属性
def 标签(self) -> str:
   返回 ';'.join(self._labels)


@labels.setter
def 标签（自身，标签）: 
   self._labels = [label.strip() for label in labels.split(';')]


@属性
def min_frames(self) -> int:
   返回 self._min_frames


@min_frames.setter
def min_frames(self, min_frames):
   self._min_frames = int(min_frames)


@属性
def min_score(self) -> 浮动: 
   返回 self._min_score


@min_frames.setter
def min_score(self, min_score):
   self._min_score = float(min_score)
````


请注意 getter 函数上指定的 Python 类型提示 - 这些提示告知前端要使用哪种 UI 控件（例如文本框、滑块、复选框等）。  支持的类型有 `str`、`int`、`float` 和 `bool`。  如果省略类型提示，则将假定它是带有文本框输入的字符串，并且用户的插件将负责将其解析/转换为所需的类型。


<palign="right">下一个| <b><a href="webrtc-recognizer-CN.md">识别器（交互式训练）</a></b>
<br/>
返回 | <b><a href="webrtc-flask-CN.md">Flask + REST</a></p>
</b><palign="center"><sup>© 2016-2023 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>