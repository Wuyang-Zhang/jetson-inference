<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="segnet-pretrained-CN.md">返回</a> | <a href="segnet-patches-CN.md">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>语义分割</sup></s></p>


# 使用 DIGITS 训练 FCN-Alexnet


上一个数据导入作业完成后，返回 DIGITS 主屏幕。  选择 `Models` 选项卡，然后从下拉列表中选择创建新的 `Segmentation Model`: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-digits-create-model.png" width="250">


在模型创建表单中，选择您之前创建的数据集。  将 `Subtract Mean` 设置为 None，将 `Base Learning Rate` 设置为 `0.0001`。  要在 DIGITS 中设置网络拓扑，请选择 `Custom Network` 选项卡并确保选择 `Caffe` 子选项卡。  将 **[FCN-Alexnet prototxt](https://raw.githubusercontent.com/NVIDIA/DIGITS/master/examples/semantic-segmentation/fcn_alexnet.prototxt)** 复制/粘贴到文本框中。  最后，将 `Pretrained Model` 设置为上面 `net_surgery` 生成的输出: `DIGITS/examples/semantic-segmentation/fcn_alexnet.caffemodel`


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-digits-aerial-model-options.png)


为您的航空模型命名，然后单击页面底部的 `Create` 按钮开始训练作业。  大约 5 个时期后，`Accuracy` 图（橙色）应该逐渐上升，模型变得可用: 


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-digits-aerial-model-converge.png)


此时，我们可以尝试在 DIGITS 中的一些示例图像上测试新模型的推理。


### 在 DIGITS 中测试推理模型


在将训练好的模型传输到 Jetson 之前，我们先在 DIGITS 中进行测试。  在与上一图相同的页面上，向下滚动到 `Trained Models` 部分。  将 `Visualization Model` 设置为 *图像分割*，然后在 `Test a Single Image` 下选择要尝试的图像（例如 `/NVIDIA-Aerial-Drone-Dataset/FPV/SFWA/720p/images/0428.png`）: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-digits-aerial-visualization-options.png" width="350">


按 `Test One`，您应该会看到类似以下内容的显示: 


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/segmentation-digits-aerial-infer.png)


接下来，下载训练好的模型快照并将其提取到 Jetson，然后继续下一步。


##
<palign="right">下一个| <b><a href="segnet-patches-CN.md">TensorRT 的 FCN-Alexnet 补丁</a></b>
<br/>
返回 | <b><a href="segnet-pretrained-CN.md">生成预训练的 FCN-Alexnet</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>