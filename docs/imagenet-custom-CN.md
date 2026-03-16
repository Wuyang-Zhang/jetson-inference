<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="imagenet-snapshot-CN.md">返回</a> | <a href="detectnet-training-CN.md">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>图像识别</sup></p>


# 在 Jetson 上加载自定义模型


我们之前使用的 [`imagenet-console`](../examples/imagenet-console/imagenet-console.cpp) 和 [`imagenet-camera`](../examples/imagenet-camera/imagenet-camera.cpp) 程序也接受扩展命令行参数来加载自定义模型快照。  将下面的 `$NET` 变量设置为提取的快照的路径: 


`` 重击
$ NET=网络/GoogleNet-ILSVRC12-子集


$ ./imagenet-console Bird_0.jpg 输出_0.jpg \
--prototxt=$NET/deploy.prototxt \
--model=$NET/snapshot_iter_184080.caffemodel \
--labels=$NET/labels.txt \
--input_blob=数据 \
--output_blob=softmax
````


和以前一样，分类和置信度将叠加到输出图像上。  与原始网络的输出相比，重新训练的 GoogleNet-12 与原始 GoogleNet-1000 进行类似的分类，只不过现在它输出我们重新训练的元类: 


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet-tensorRT-console-bird.png)


上面的扩展命令行参数还使用 [`imagenet-camera`](../examples/imagenet-camera/imagenet-camera.cpp) 加载自定义分类模型。


##
<palign="right">下一个| <b><a href="detectnet-training-CN.md">使用DetectNet定位物体坐标</a></b>
<br/>
返回 | <b><a href="imagenet-snapshot-CN.md">将模型快照下载到 Jetson</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>