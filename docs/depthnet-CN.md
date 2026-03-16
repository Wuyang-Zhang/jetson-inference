<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="backgroundnet-CN.md">返回</a> | <a href="pytorch-transfer-learning-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>单色深度</sup></s></p>


# 使用 DepthNet 的单目深度
深度感测对于地图绘制、导航和障碍物检测等任务非常有用，但它历来需要立体相机或 RGB-D 相机。  现在的 DNN 能够从单个单目图像（又称单目深度）推断相对深度。  请参阅 [MIT FastDepth](https://arxiv.org/abs/1903.03273) 论文，了解使用全卷积网络 (FCN) 实现此目的的一种方法。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/depthnet-0.jpg">


[`depthNet`](../c/depthNet.h) 对象接受单色图像作为输入，并输出深度图。  深度图被着色以便可视化，但原始的[深度场](#getting-the-raw-depth-field)也可以直接访问深度。 [`depthNet`](../c/depthNet.h) 可在 [Python](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#depthNet) 和 [C++](../c/depthNet.h) 中使用。


作为使用 `depthNet` 类的示例，我们提供了 C++ 和 Python 的示例程序: 


- [`depthnet.cpp`](../examples/depthnet/depthnet.cpp) (C++)

- [`depthnet.py`](../python/examples/depthnet.py) (Python)


这些样本能够从图像、视频和相机馈送中推断深度。  有关支持的各种类型的输入/输出流的详细信息，请参阅[相机流和多媒体](aux-streaming-CN.md) 页面。


## 图像上的单色深度


首先，让我们尝试在一些示例图像上运行 `depthnet` 示例。  除了输入/输出路径之外，还有一些可选的附加命令行选项: 


- 可选 `--network` 标志，用于更改正在使用的深度模型（建议默认值为 `fcn-mobilenet`）。

- 可选的 `--visualize` 标志，可以是 `input`、`depth` 的逗号分隔组合

	- 默认值为 `--visualize=input,depth` 并排显示输入图像和深度图像

	- 要仅查看深度图像，请使用 `--visualize=depth`

- 可选 `--depth-size` 值，用于相对于输入缩放深度图的大小（默认为 `1.0`）

- 可选 `--filter-mode` 标志，选择用于上采样的 `point` 或 `linear` 过滤（默认为 `linear`）

- 可选 `--colormap` 标志，用于设置可视化过程中使用的颜色映射（默认为 `viridis_inverted`）


如果您使用 [Docker 容器](aux-docker-CN.md)，建议将输出图像保存到 `images/test` 安装目录。  然后，您可以在主机设备的 `jetson-inference/data/images/test` 下轻松查看这些图像（有关详细信息，请参阅[已安装的数据卷](aux-docker-CN.md#mounted-data-volumes)）。


以下是室内场景的单色深度估计的一些示例: 


`` 重击
# C++
$ ./depthnet "images/room_*.jpg" images/test/depth_room_%i.jpg


# Python
$ ./depthnet.py "images/room_*.jpg" images/test/depth_room_%i.jpg
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/depthnet-room-0.jpg">


> **注意**: 第一次运行每个模型时，TensorRT 将花费几分钟来优化网络。 <br/>

> 然后，此优化的网络文件会缓存到磁盘，因此将来使用该模型的运行将加载得更快。


以下是一些从室外拍摄的场景: 


`` 重击
# C++
$ ./depthnet "images/trail_*.jpg" 图片/test/depth_trail_%i.jpg


# Python
$ ./depthnet.py "images/trail_*.jpg" 图片/test/depth_trail_%i.jpg
````


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/depthnet-trail-0.jpg">


## 视频的单声道深度


要对实时摄像机流或视频运行单色深度估计，请从 [摄像机流和多媒体](aux-streaming-CN.md) 页面传入设备或文件路径。


`` 重击
# C++
$ ./depthnet /dev/video0 # csi://0 如果使用 MIPI CSI 摄像头


# Python
$ ./depthnet.py /dev/video0 # csi://0 如果使用 MIPI CSI 摄像头
````


<a href="https://www.youtube.com/watch?v=3_bU6Eqb4hE" target="_blank"><img src=https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/depthnet-video-0.jpg width="750"></a>


> **注意**: 如果屏幕太小无法容纳输出，可以使用`--depth-scale=0.5`来缩小尺寸<br/>

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;使用 `--input-width=X --input-height=Y` 缩小相机尺寸



## 获取原始深度场


如果您想访问原始深度图，可以使用 `depthNet.GetDepthField()` 来实现。  这将返回一个单通道浮点图像，该图像通常小于原始输入 (224x224) - 这表示模型的原始输出。  另一方面，用于可视化的彩色深度图像被上采样以匹配原始输入的分辨率（或 `--depth-size` 比例设置为的任何值）。


下面是用于访问原始深度场的 Python 和 C++ 伪代码: 


#### Python


```` 蟒蛇
导入 jetson.inference
导入jetson.utils


将 numpy 导入为 np


# 加载单深度网络
net = jetson.inference.深度Net()


# heightNet 为深度场重新使用相同的内存，
# 所以你只需要执行一次（不是每一帧）
深度场 = net.GetDepthField()


# cudaToNumpy() 将深度场 cudaImage 映射到 numpy
# 这个映射是持久的，所以你只需要做一次
深度_numpy = jetson.utils.cudaToNumpy(深度_场)


print(f"深度场分辨率为{深度场.宽度}x{深度场.高度},格式={深度场.格式})


而真实: 
    img = input.Capture() # 假设您已经创建了输入 videoSource 流
    网络.进程(img)
    jetson.utils.cudaDeviceSynchronize() # 等待GPU完成处理，这样我们就可以在CPU上使用结果


# 使用 numpy 找到最小/最大值
    min_深度 = np.amin(深度_numpy)
    最大深度 = np.amax(深度_numpy)
````


#### C++


````.cpp
#include <jetson-inference/深度Net.h>


// 加载单深度网络
深度网络* net = 深度网络::Create();


// heightNet 为深度场重新使用相同的内存，
// 所以你只需要执行一次（不是每一帧）
float* 深度字段 = net->GetDepthField();
const int 深度宽度 = net->GetDepthWidth();
const int height_height = net->GetDepthHeight();


而（真）
{
    uchar3* img = NUL;
    输入->捕获(&img);  // 假设您已经创建了输入 videoSource 流
    net->Process(img, 输入->GetWidth(), 输入->GetHeight());


// 等待GPU处理完成
    CUDA(cudaDeviceSynchronize());


// 您现在可以从CPU（或GPU）安全地访问深度场
    for( int y=0; y < 深度高度; y++ )
        for( int x=0; x < 深度宽度; x++ )
	       printf("深度 x=%i y=%i -> %f\n", x, y, 深度图[y * 深度宽度 + x]);
}
````


尝试使用单一深度测量绝对距离可能会导致不准确，因为它通常在相对深度估计方面更有效。  原始深度场中的值范围可能会根据场景而变化，因此通常会动态重新计算这些值。  例如，在可视化期间，对深度场执行直方图均衡，以在深度值范围内更均匀地分布颜色图。


接下来，我们将介绍[迁移学习](pytorch-transfer-learning-CN.md) 的概念，并使用 PyTorch 在 Jetson 上训练我们自己的 DNN 模型。


##
<palign="right">下一个| <b><a href="pytorch-transfer-learning-CN.md">使用 PyTorch 进行迁移学习</a></b>
<br/>
返回 | <b><a href="backgroundnet-CN.md">背景去除</a></p>
</b><palign="center"><sup>© 2016-2021 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>