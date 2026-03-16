<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="detectnet-training-CN.md">返回</a> | <a href="detectnet-console-CN.md">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>物体检测</sup></p>


# 将检测模型下载到 Jetson


接下来，下载训练好的模型快照并将其提取到 Jetson。  从 Jetson TX1/TX2 上的浏览​​器导航到 DIGITS 服务器和 `DetectNet-COCO-Dog` 模型。  在 `Trained Models` 部分下，从下拉列表中选择所需的快照（通常是具有最高纪元的快照），然后单击 `Download Model` 按钮。


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-digits-model-download-dog.png" width="650">


或者，如果您的 Jetson 和 DIGITS 服务器无法从同一网络访问，您可以使用上述步骤将快照下载到中间计算机，然后使用 SCP 或 USB 记忆棒将其复制到 Jetson。


然后使用类似于以下的命令提取存档: 


```cd <directory where you downloaded the snapshot>
tar -xzvf 20170504-190602-879f_epoch_100.0.tar.gz
```


### TensorRT 的 DetectNet 补丁


在原始 DetectNet prototxt 中存在一个 Python 聚类层，该层在 TensorRT 中不可用，应从快照中包含的 `deploy.prototxt` 中删除。  在此存储库中，[`detectNet`](detectNet.h) 类处理聚类，而不是 Python。


在 `deploy.prototxt` 末尾，删除名为 `cluster` 的图层: 


```
layer {
  name: "cluster"
  type: "Python"
  bottom: "coverage"
  bottom: "bboxes"
  top: "bbox-list"
  python_param {
    module: "caffe.layers.detectnet.clustering"
    layer: "ClusterDetections"
    param_str: "640, 640, 16, 0.6, 2, 0.02, 22, 1"
  }
}
```


如果没有这个 Python 层，快照现在可以导入到 Jetson 上的 TensorRT 中。


##
<palign="right">下一个| <b><a href="detectnet-console-CN.md">从命令行检测对象</a></b>
<br/>
返回 | <b><a href="detectnet-training-CN.md">使用 DetectNet 定位物体坐标</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>