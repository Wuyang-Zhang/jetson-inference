<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="imagenet-training-CN.md">返回</a> | <a href="imagenet-custom-CN.md">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>图像识别</sup></p>


# 将模型快照下载到 Jetson


现在我们确认训练后的模型可以在 DIGITS 中运行，让我们下载模型快照并将其提取到 Jetson。


从 Jetson 上的浏览​​器导航到 DIGITS 服务器和 `GoogleNet-ILSVRC12-subset` 模型。  在 `Trained Models` 部分下，从下拉列表中选择所需的快照（通常是具有最高纪元的快照），然后单击 `Download Model` 按钮。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet-digits-model-download.png" width="650">


或者，如果您的 Jetson 和 DIGITS 服务器无法从同一网络访问，您可以使用上述步骤将快照下载到中间计算机，然后使用 SCP 或 USB 记忆棒将其复制到 Jetson。


然后使用类似于以下的命令提取存档: 


```cd <directory where you downloaded the snapshot>
tar -xzvf 20170524-140310-8c0b_epoch_30.0.tar.gz
```


接下来，我们将自定义快照加载到在 Jetson 上运行的 TensorRT 中。


##
<palign="right">下一个| <b><a href="imagenet-custom-CN.md">在 Jetson 上加载自定义模型</a></b>
<br/>
返回 | <b><a href="imagenet-training-CN.md">重新训练识别网络</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>