<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">


# 其他例子


此区域列出了可用于 Jetson TX1 的其他深度学习推理资源<br />


## GPU 推理引擎 (GIE) 示例
### 安装GPU推理引擎


NVIDIA 的 [GPU 推理引擎](https://developer.nvidia.com/gie) (GIE) 是一个优化的后端，用于评估 prototxt 格式的深度推理网络。


#### 1. 包装内容


首先，解压存档: 
```
$ tar -zxvf gie.aarch64-cuda7.0-1.0-ea.tar.gz
```


目录结构如下: 
```
|-GIE
|  \bin  where the samples are built to
|  \data sample network model / prototxt's
|  \doc  API documentation and User Guide
|  \include
|  \lib 
|  \samples 
```


#### 2. 删除打包的cuDNN


如果您使用 JetPack 刷新了 Jetson TX1 或者已经安装了 cuDNN，请删除 GIE 附带的 cuDNN 版本: 


```
$ cd GIE/lib
$ rm libcudnn*
$ cd ../../
```


#### 3. 构建样品


````
$ cd GIE/samples/sampleMNIST
$ make TARGET=tx1
Compiling: sampleMNIST.cpp
Linking: ../../bin/sample_mnist_debug
Compiling: sampleMNIST.cpp
Linking: ../../bin/sample_mnist
$ cd ../sampleGoogleNet
$ make TARGET=tx1
Compiling: sampleGoogleNet.cpp
Linking: ../../bin/sample_googlenet_debug
Compiling: sampleGoogleNet.cpp
Linking: ../../bin/sample_googlenet
$ cd ../../../
````


#### 4. 运行样品


````
$ cd GIE/bin
$ ./sample_mnist
@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@%+-: =@@@@@@@@@@@@
@@@@@@@%=-@@@**@@@@@@@
@@@@@@@: %#@-#@@@。 #@@@@@@
@@@@@@* +@@@@:*@@@ *@@@@@@
@@@@@@# +@@@@ @@@% @@@@@@@
@@@@@@@。  : %@@。@@@。 *@@@@@@@
@@@@@@@@-=@@@@。 -@@@@@@@@
@@@@@@@@@%: +@- :@@@@@@@@@
@@@@@@@@@@@%。  : -@@@@@@@@@@
@@@@@@@@@@@@@+ #@@@@@@@@@@@
@@@@@@@@@@@@@@+ :@@@@@@@@@@@
@@@@@@@@@@@@@@+ *@@@@@@@@@
@@@@@@@@@@@@@@@: = @@@@@@@@@@
@@@@@@@@@@@@@@@ :@ @@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@# +@ @@@@@@@@@
@@@@@@@@@@@@@* ++ @@@@@@@@@
@@@@@@@@@@@@@* *@@@@@@@@@@
@@@@@@@@@@@@@# =@@@@@@@@@@
@@@@@@@@@@@@@@。 +@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@


0:
1: 
2: 
3: 
4: 
5: 
6: 
7: 
8: ************
9: 
````
MNIST 样本随机选择数字 0-9 的图像，然后使用 GIE 通过 MNIST 网络进行分类。  在此示例中，网络正确地将图像识别为#8。