<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="segnet-patches-CN.md">返回</a> | <a href="../README-CN.md#two-days-to-a-demo-digits">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>语义分割</sup></s></p>


# 在 Jetson 上运行分割模型


要在 Jetson 上测试自定义分段网络模型快照，请使用命令行界面 [`segnet-console`](../examples/segnet-console/segnet-console.cpp)


首先，为了方便起见，将提取的快照的路径设置为 `$NET` 变量: 


`` 重击
$ NET=20170421-122956-f7c0_epoch_5.0


$ ./segnet-console Drone_0428.png Output_0428.png \
--prototxt=$NET/deploy.prototxt \
--model=$NET/snapshot_iter_22610.caffemodel \
--labels=$NET/fpv-labels.txt \
--colors=$NET/fpv-deploy-colors.txt \
--input_blob=数据 \ 
--output_blob=score_fr
````


这将在随存储库下载的测试图像上运行指定的分割模型。


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-aerial-tensorRT.png)


除了本教程中的航空模型之外，该存储库还包括其他分割数据集上的预训练模型，包括 **[Cityscapes](https://www.cityscapes-dataset.com/)**、**[SYNTHIA](http://synthia-dataset.net/)** 和 **[Pascal-VOC](http://host.robots.ox.ac.uk/pascal/VOC/)**。


##
<palign="right">返回 | <b><a href="segnet-training-CN.md">TensorRT 的 FCN-Alexnet 补丁</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>