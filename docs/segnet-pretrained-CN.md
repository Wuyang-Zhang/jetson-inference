<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="segnet-dataset-CN.md">返回</a> | <a href="segnet-training-CN.md">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>语义分割</sup></s></p>


# 生成预训练的 FCN-Alexnet


全卷积网络 (FCN) Alexnet 是我们将用于 DIGITS 和 TensorRT 分割模型的网络拓扑。  请参阅这篇有关卷积过程的 [Parallel ForAll](https://devblogs.nvidia.com/parallelforall/image-segmentation-using-digits-5) 文章。  DIGITS5 的一项新功能是支持分割数据集和训练模型。


DIGITS 语义分割示例中包含一个脚本，可将 Alexnet 模型转换为 FCN-Alexnet。  然后，将该基本模型用作预训练的起点，用于在自定义数据集上训练未来的 FCN-Alexnet 分割模型。


要生成预训练的 FCN-Alexnet 模型，请打开终端，导航到 DIGITS 语义分割示例，然后运行 ​​`net_surgery` 脚本: 


``` bash
$ cd DIGITS/examples/semantic-segmentation
$ ./net_surgery.py
Downloading files (this might take a few minutes)...
Downloading https://raw.githubusercontent.com/BVLC/caffe/rc3/models/bvlc_alexnet/deploy.prototxt...
Downloading http://dl.caffe.berkeleyvision.org/bvlc_alexnet.caffemodel...
Loading Alexnet model...
...
Saving FCN-Alexnet model to fcn_alexnet.caffemodel
```


接下来，我们将在 DIGITS 中的无人机数据集上训练 FCN-Alexnet 模型。


##
<palign="right">下一个| <b><a href="segnet-training-CN.md">使用 DIGITS 训练 FCN-Alexnet</a></b>
<br/>
返回 | <b><a href="segnet-dataset-CN.md">使用 SegNet 进行语义分割</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>