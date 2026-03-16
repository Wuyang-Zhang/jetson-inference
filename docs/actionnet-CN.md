<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="posenet-CN.md">返回</a> | <a href="backgroundnet-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>动作识别</sup></s></p>


# 动作识别
动作识别对一系列视频帧上发生的活动、行为或手势进行分类。  DNN 通常使用带有附加时间维度的图像分类主干网。  例如，基于ResNet18的预训练模型使用16帧的窗口。  您还可以跳过帧以延长模型对动作进行分类的时间窗口。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/actionnet-windsurfing.gif">


[`actionNet`](../c/actionNet.h) 对象一次接收一个视频帧，将它们缓冲作为模型的输入，并输出具有最高置信度的类。  [`actionNet`](../c/actionNet.h) 可以在 [Python](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#actionNet) 和 [C++](../c/actionNet.h) 中使用。


作为使用 `actionNet` 类的示例，有 C++ 和 Python 的示例程序: 


- [`actionnet.cpp`](../examples/actionnet/actionnet.cpp) (C++)

- [`actionnet.py`](../python/examples/actionnet.py) (Python)


## 运行示例


要对实时摄像机流或视频运行动作识别，请从 [摄像机流和多媒体](aux-streaming-CN.md) 页面传入设备或文件路径。


`` 重击
# C++
$ ./actionnet /dev/video0 # V4L2摄像头输入，显示输出（默认） 
$ ./actionnet input.mp4 output.mp4 # 视频文件输入/输出（mp4、mkv、avi、flv）


# Python
$ ./actionnet.py /dev/video0 # V4L2摄像头输入，显示输出（默认） 
$ ./actionnet.py input.mp4 output.mp4 # 视频文件输入/输出（mp4、mkv、avi、flv）
````


### 命令行参数


这些可选的命令行参数可以与 actionnet/actionnet.py 一起使用: 


```
  --network=NETWORK    pre-trained model to load, one of the following:
                           * resnet-18 (default)
                           * resnet-34
  --model=MODEL        path to custom model to load (.onnx)
  --labels=LABELS      path to text file containing the labels for each class
  --input-blob=INPUT   name of the input layer (default is 'input')
  --output-blob=OUTPUT name of the output layer (default is 'output')
  --threshold=CONF     minimum confidence threshold for classification (default is 0.01)
  --skip-frames=SKIP   how many frames to skip between classifications (default is 1)
```


默认情况下，模型将每隔一帧处理一次，以延长对动作进行分类的时间窗口。  您可以使用 `--skip-frames` 参数更改此设置（使用 `--skip-frames=0` 将处理每一帧）。


### 预训练的动作识别模型


以下是可用的预训练动作识别模型，以及用于加载它们的 `actionnet` 的关联 `--network` 参数: 


|型号| CLI 参数 |课程 |
| ------------------------|--------------|---------|
| Action-ResNet18-动力学 | `resnet18` |  1040 | 1040
| Action-ResNet34-动力学 | `resnet34` |  1040 | 1040


默认值为 `resnet18`。  这些模型在 [Kinetics 700](https://www.deepmind.com/open-source/kinetics) 和 [Moments in Time](http://moments.csail.mit.edu/) 数据集上进行训练（有关类标签列表，请参阅[此处](https://gist.github.com/dusty-nv/3aaa2494f7be212391cca1927ef7c74e)）。


##
<palign="right">下一个| <b><a href="backgroundnet-CN.md">背景去除</a></b>
<br/>
返回 | <b><a href="posenet-CN.md">使用 PoseNet 进行姿势估计</a></p>
</b><palign="center"><sup>© 2016-2021 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>