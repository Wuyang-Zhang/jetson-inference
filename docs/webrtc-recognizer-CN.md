<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="webrtc-dash-CN.md">返回</a> | <a href="aux-streaming-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>Web应用程序框架</sup></s></p>


# 识别器（交互式培训）


Recognizer 是一款基于 Flask 的视频标记/分类 Web 应用程序，具有交互式数据收集和训练功能。  当视频被标记和记录时，更新的模型会在后台使用 PyTorch 增量地重新训练，然后用于 TensorRT 的推理。  推理和训练可以同时运行，重新训练的模型在运行时动态加载以进行推理。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/webrtc-recognizer.jpg" width="600">


它还支持多标签标记，除了通过 WebRTC 录制客户端视频之外，还可以从客户端上传现有图像。本示例的主要源文件（在 [`python/www/recognizer`](../python/www/recognizer) 下找到）如下: 


  * [`app.py`](../python/www/recognizer/app.py)（网络服务器）

  * [`stream.py`](../python/www/recognizer/stream.py)（WebRTC 流线程）

  * [`model.py`](../python/www/recognizer/model.py)（DNN 推理 + 训练）

  * [`dataset.py`](../python/www/recognizer/dataset.py)（数据标记+记录）

  * [`index.html`](../python/www/recognizer/templates/index.html)（前端演示）


## 运行示例


启动 app.py 将启动 Flask Web 服务器、运行 WebRTC 和推理的流线程以及 PyTorch 的训练线程: 


``` bash
$ cd jetson-inference/python/www/recognizer
$ pip3 install -r requirements.txt
$ python3 app.py --data=data/my_dataset
```


> **注意**: 接收浏览器网络摄像头需要启用 [HTTPS/SSL](webrtc-server-CN.md#enabling-https--ssl)


`--data` 参数设置数据集和模型的存储路径。  如果您从源代码构建 jetson-inference，则应选择 [安装 PyTorch](building-repo-2-CN.md#installing-pytorch)（或再次运行 `install-pytorch.sh` 脚本）。如果您使用的是 Docker 容器，则已经为您安装了 PyTorch。


运行 app.py 后，您应该能够将浏览器导航到 `https://<JETSON-IP>:8050` 并启动流。  默认端口为 8050，但您可以使用 `--port=N` 命令行参数更改该端口。  它默认配置为 WebRTC 输入和输出，但如果您想使用不同的[视频输入设备](aux-streaming-CN.md#input-streams)，您可以使用 `--input` 参数进行设置（例如，V4L2 摄像头为 `--input=/dev/video0`）


### 收集数据


如果需要，首先从网页上的流源下拉列表中选择客户端摄像头，然后按 `Send` 按钮。  准备好后，在“标签”选择框中输入相机正在查看的内容的类标签。  输入标签后，您将能够将图像记录或上传到数据集中。  您可以按住“录制”按钮来捕获视频序列。  下面是数据流的高级图表: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/webrtc-recognizer-diagram.jpg">


建议保持类别之间标签的分布相对平衡 - 否则模型将更有可能偏向某些类别。  您可以通过展开 `Training` 下拉列表来查看数据集中的标签分布和图像数量。


### 训练


添加和标记新数据时，可以在 `Training` 下拉列表下启用训练。  训练进度和准确性将在页面上更新。  在每个 epoch 结束时，如果模型具有最高的准确度，它将导出到 ONNX 并加载到 TensorRT 中进行推理。


启动 app.py 时可以设置各种用于训练的命令行选项: 


| CLI 参数 |描述 |默认|
|--------------------------------|----------------------------------------------------------------------------------------------------------------------------------------|------------|
| `--data` |数据和模型的存储路径 | `data/` |
| `--net` | DNN 架构（请参阅[此处](https://pytorch.org/vision/stable/models.html#classification) 了解选项）| `resnet18` |
| `--net-width` |模型的宽度（增加以获得更高的精度）| 224 | 224
| `--net-height` |模型的高度（增加以获得更高的精度）| 224 | 224
| `--batch-size` |训练批量大小 | 1 |
| `--workers` |数据加载器线程数 | 1 |
| `--optimizer` |求解器（`adam` 或 `sgd`）| `adam` |
| `--learning-rate` |初始优化器学习率 | 0.001 | 0.001
| `--no-augmentation` |禁用训练数据上的颜色抖动/随机翻转 |已启用 |



### 推理


可以在 `Classification` 下拉菜单下启用推理。  当使用多标签分类时（即数据集包含具有多个标签的图像），将显示置信度分数高于可从页面控制的阈值的所有分类结果。


通过将您自己的代码添加到 [`Model.Classify()`](https://github.com/dusty-nv/jetson-inference/blob/3476b4896051929f764f6b806378271dc82f23f1/python/www/recognizer/model.py#L83) 函数，可以扩展应用程序以在检测到某些对象时触发操作: 


`` 重击
def 分类（自身，img）: 
   ”“”
   运行分类推理并返回结果。
   ”“”
   如果没有 self.inference_enabled: 
      返回


# 返回（classID，confidence）元组列表
   self.results = self.model_infer.Classify(img, topK=0 if self.dataset.multi_label else 1)


# 要触发自定义操作/处理，请在此处添加它们: 
   对于 classID，对 self.results 的信心: 
      if self.model_infer.GetClassLabel(classID) == 'person': # 更新你的类
         print(f"检测到一个具有 {confidence * 100}% 置信度的人") # 做一些回应


返回自己的结果
````


修改后端服务器端Python代码时，请记住重新启动app.py以使更改生效。  与前面的 Flask 示例一样，各种 [REST 查询](https://github.com/dusty-nv/jetson-inference/blob/master/docs/webrtc-flask.md#rest-queries) 用于在客户端和服务器之间传递动态设置和状态更改，您也可以添加到其中。


<palign="right">下一个| <b><a href="aux-streaming-CN.md">相机流媒体和多媒体</a></b>
<br/>
返回 | <b><a href="webrtc-dash-CN.md">Plotly 仪表板</a></p>
</b><palign="center"><sup>© 2016-2023 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>