<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="imagenet-console-2-CN.md">返回</a> | <a href="imagenet-example-2-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>图像分类</sup></p>


# 编写您自己的图像识别程序 (Python)
在上一步中，我们运行了 `jetson-inference` 存储库附带的示例应用程序。


现在，我们将逐步使用 Python 从头开始​​创建一个用于图像识别的新程序，名为 [`my-recognition.py`](../python/examples/my-recognition.py)。  该脚本将从磁盘加载任意图像并使用 [`imageNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#imageNet) 对象对其进行分类。  完整的源代码位于 [`python/examples/my-recognition.py`](../python/examples/my-recognition.py)


```` 蟒蛇
#!/usr/bin/python3


导入 jetson.inference
导入jetson.utils


导入argparse


# 解析命令行
解析器 = argparse.ArgumentParser()
parser.add_argument("filename", type=str, help="要处理的图像的文件名")
parser.add_argument("--network", type=str, default="googlenet", help="要使用的模型，可以是: googlenet、resnet-18 等。")
args = parser.parse_args()


# 加载图像（到共享CPU/GPU内存中）
img = jetson.utils.loadImage(args.文件名)


# 加载识别网络
net = jetson.inference.imageNet(args.network)


# 对图像进行分类
class_idx，置信度 = net.Classify(img)


# 查找对象描述
class_desc = net.GetClassDesc(class_idx)


# 打印出结果
print("图像被识别为 '{:s}' (class #{:d})，置信度为 {:f}%".format(class_desc, class_idx,confidence * 100))
````


## 设置项目


如果您使用 Docker 容器，则需要将代码存储在 [已安装目录](aux-docker-CN.md#mounted-data-volumes) 中。  这样，当您关闭容器时，您的代码就不会丢失。  为简单起见，本指南将在主机设备上位于 `~/my-recognition-python` 的用户主目录下的目录中创建它，然后将该路径挂载到容器中。


从终端（容器外部）运行这些命令来创建目录、源文件并下载一些测试图像: 


``` bash
# run these commands outside of container
$ cd ~/
$ mkdir my-recognition-python
$ cd my-recognition-python
$ touch my-recognition.py
$ chmod +x my-recognition.py
$ wget https://github.com/dusty-nv/jetson-inference/raw/master/data/images/black_bear.jpg 
$ wget https://github.com/dusty-nv/jetson-inference/raw/master/data/images/brown_bear.jpg
$ wget https://github.com/dusty-nv/jetson-inference/raw/master/data/images/polar_bear.jpg 
```


然后，当您启动容器时，[挂载您刚刚创建的目录](aux-docker-CN.md#mounted-data-volumes): 


```bash
$ docker/run.sh --volume ~/my-recognition-python:/my-recognition-python   # mounted inside the container to /my-recognition-python 
```


接下来，我们将该程序的 Python 代码添加到我们在此处创建的空源文件中。


## 源代码


在您选择的编辑器中打开 `my-recognition.py` （或运行 `gedit my-recognition.py`）。  您可以从容器外部进行编辑。


首先，让我们将 shebang 序列添加到文件的最顶部以自动使用 Python 解释器: 


``` python
#!/usr/bin/python3
```


接下来，我们将导入将在脚本中使用的 Python 模块。


#### 导入模块


添加 `import` 语句来加载用于识别图像和图像加载的 [`jetson.inference`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html) 和 [`jetson.utils`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.utils.html) 模块。  我们还将加载标准 `argparse` 包来解析命令行。


```` 蟒蛇
导入 jetson.inference
导入jetson.utils


导入argparse
````


> **注意**: 这些 Jetson 模块是在[构建存储库](building-repo-2-CN.md#compiling-the-project) 的 `sudo make install` 步骤中安装的。

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;如果您没有运行 `sudo make install`，那么当我们运行示例时将找不到这些包。



#### 解析命令行


接下来，添加一些样板代码来解析图像文件名和可选的 `--network` 参数: 


``` python
# parse the command line
parser = argparse.ArgumentParser()
parser.add_argument("filename", type=str, help="filename of the image to process")
parser.add_argument("--network", type=str, default="googlenet", help="model to use, can be:  googlenet, resnet-18, ect. (see --help for others)")
args = parser.parse_args()
```


此示例加载并分类用户指定的图像。  预计它将像这样运行: 


``` bash
$ ./my-recognition.py my_image.jpg
```


所需加载的图像文件名应替换为 `my_image.jpg`。  您还可以选择指定 `--network` 参数来更改使用的分类网络（默认为 GoogleNet）: 


``` bash
$ ./my-recognition.py --network=resnet-18 my_image.jpg
```


有关下载其他网络的详细信息，请参阅上一页的[下载其他分类模型](imagenet-console-2-CN.md#downloading-other-classification-models) 部分。



#### 从磁盘加载图像


您可以使用 `loadImage()` 函数将图像从磁盘加载到共享 CPU/GPU 内存中。支持的格式有 JPG、PNG、TGA 和 BMP。


添加此行以加载具有从命令行指定的文件名的图像: 


``` python
img = jetson.utils.loadImage(args.filename)
```


返回的图像将是一个 [`jetson.utils.cudaImage`](aux-image-CN.md#image-capsules-in-python) 对象，其中包含宽度、高度和像素格式等属性: 


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


有关从 Python 访问图像的更多信息，请参阅[使用 CUDA 进行图像操作](aux-image-CN.md) 页面。  为了简单起见，我们在这里只加载单个图像。要加载视频或图像序列，您需要像之前的 [`imagenet.py`](../python/examples/imagenet.py) 示例一样使用 [`videoSource`](aux-streaming-CN.md#source-code) API。


#### 加载图像识别网络


使用 [`imageNet`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#imageNet) 对象，以下代码将使用 TensorRT 加载所需的分类模型。  除非您使用 `--network` 标志指定了不同的网络，否则默认情况下它将加载 GoogleNet，该网络在您最初[构建 `jetson-inference` 存储库](building-repo-2-CN.md#downloading-models) 时已下载（默认情况下也选择下载 `ResNet-18` 模型）。


所有可用的分类模型都在 ImageNet ILSVRC 数据集上进行了预训练，该数据集可以识别多达 [1000 个不同类别](../data/networks/ilsvrc12_synset_words.txt) 的物体，例如不同种类的水果和蔬菜、许多不同种类的动物，以及日常人造物体，例如车辆、办公家具、运动器材等。


``` python
# load the recognition network
net = jetson.inference.imageNet(args.network)
```


#### 对图像进行分类


接下来，我们将使用 `imageNet.Classify()` 函数通过识别网络对图像进行分类: 


``` python
# classify the image
class_idx, confidence = net.Classify(img)
```


`imageNet.Classify()` 接受图像及其尺寸，并使用 TensorRT 执行推理。


它返回一个元组，其中包含图像被识别为的对象类的整数索引以及结果的浮点置信度值。


#### 解释结果


作为最后一步，让我们检索类描述并打印出分类结果: 


```` 蟒蛇
# 查找对象描述
class_desc = net.GetClassDesc(class_idx)


# 打印出结果
print("图像被识别为 '{:s}' (class #{:d})，置信度为 {:f}%".format(class_desc, class_idx,confidence * 100))
````


`imageNet.Classify()` 返回已识别对象类的索引（对于在 ILSVRC 上训练的这些模型，在 `0` 和 `999` 之间）。  给定类索引，`imageNet.GetClassDesc()` 函数将返回包含该类的文本描述的字符串。  这些描述会自动从 [`ilsvrc12_synset_words.txt`](../data/networks/ilsvrc12_synset_words.txt) 加载。


就是这样！  这就是图像分类所需的全部 Python 代码。  请参阅上面的[完整来源](#coding-your-own-image-recognition-program-python)。


## 运行示例


现在我们的 Python 程序已完成，让我们对本页开头[下载](#setting-up-the-project) 的测试图像进​​行分类: 


``` bash
$ ./my-recognition.py polar_bear.jpg
image is recognized as 'ice bear, polar bear, Ursus Maritimus, Thalarctos maritimus' (class #296) with 99.999878% confidence
```
<img src="https://github.com/dusty-nv/jetson-inference/raw/master/data/images/polar_bear.jpg" width="400">


``` bash
$ ./my-recognition.py brown_bear.jpg
image is recognized as 'brown bear, bruin, Ursus arctos' (class #294) with 99.928925% confidence
```
<img src="https://github.com/dusty-nv/jetson-inference/raw/master/data/images/brown_bear.jpg" width="400">



``` bash
$ ./my-recognition.py black_bear.jpg
image is recognized as 'American black bear, black bear, Ursus americanus, Euarctos americanus' (class #295) with 98.898628% confidence
```
<img src="https://github.com/dusty-nv/jetson-inference/raw/master/data/images/black_bear.jpg" width="400">


您还可以通过指定 `--network` 标志来选择使用[不同网络](imagenet-console-2-CN.md#downloading-other-classification-models)，如下所示: 


``` bash
$ ./my-recognition.py --network=resnet-18 polar_bear.jpg
image is recognized as 'ice bear, polar bear, Ursus Maritimus, Thalarctos maritimus' (class #296) with 99.743396% confidence
```


接下来，我们将逐步创建该程序的 C++ 版本。


##
<palign="right">下一个| <b><a href="imagenet-example-2-CN.md">编写您自己的图像识别程序（C++）</a></b>
<br/>
返回 | <b><a href="imagenet-console-2-CN.md">使用 ImageNet 对图像进行分类</a></b></p>
<palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>