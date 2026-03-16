<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="actionnet-CN.md">返回</a> | <a href="depthnet-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>背景去除</sup></s></p>


# 背景去除
背景去除（又名背景减除或显着目标检测）会生成一个掩模，将图像的前景与背景分开。  您可以使用它来替换或模糊背景（类似于视频会议应用程序），或者它可以帮助其他视觉 DNN 的预处理，例如目标检测/跟踪或运动检测。  使用的模型是全卷积网络 [U²-Net](https://arxiv.org/abs/2005.09007)。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/backgroundnet-dog.jpg">


[`backgroundNet`](../c/backgroundNet.h) 对象获取图像，并输出前景蒙版。  [`backgroundNet`](../c/backgroundNet.h) 可以在 [Python](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#backgroundNet) 和 [C++](../c/backgroundNet.h) 中使用。


作为使用 `backgroundNet` 类的示例，有 C++ 和 Python 的示例程序: 


- [`backgroundnet.cpp`](../examples/backgroundnet/backgroundnet.cpp) (C++)

- [`backgroundnet.py`](../python/examples/backgroundnet.py) (Python)


## 运行示例


以下是删除和替换图像背景的示例: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/backgroundnet-bird.jpg">


`` 重击
# C++
$ ./backgroundnet images/bird_0.jpg images/test/bird_mask.png # 删除背景（带alpha）
$ ./backgroundnet --replace=images/snow.jpg images/bird_0.jpg images/test/bird_replace.jpg # 替换背景


# Python
$ ./backgroundnet.py images/bird_0.jpg images/test/bird_mask.png # 删除背景（带alpha）
$ ./backgroundnet.py --replace=images/snow.jpg images/bird_0.jpg images/test/bird_replace.jpg # 替换背景
````


`--replace` 命令行参数接受要替换背景的图像的文件名。  它将重新缩放到与输入相同的分辨率。


### 直播


要在实时摄像头流上运行后台删除或替换，请从 [摄像头流和多媒体](aux-streaming-CN.md) 页面传入设备: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/backgroundnet-camera.jpg">


`` 重击
# C++
$ ./backgroundnet /dev/video0 # 删除背景
$ ./backgroundnet --replace=images/coral.jpg /dev/video0 # 替换背景


# Python
$ ./backgroundnet /dev/video0 # 删除背景
$ ./backgroundnet --replace=images/coral.jpg /dev/video0 # 替换背景
````


通过指定[输出流](aux-streaming-CN.md#output-streams)，您可以在显示器（默认）上、通过网络（如 WebRTC）查看此内容，或将其保存到视频文件中。


##
<palign="right">下一个| <b><a href="depthnet-CN.md">单目深度估计</a></b>
<br/>
返回 | <b><a href="actionnet-CN.md">动作识别</a></p>
</b><palign="center"><sup>© 2016-2021 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>