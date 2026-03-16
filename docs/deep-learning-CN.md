<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="digits-workflow-CN.md">返回</a> | <a href="digits-workflow-CN.md">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>概述</sup></p>


# 什么是深度学习？


### 介绍


*深度学习*网络通常有两个主要的发展阶段: **训练**和**推理**


#### 培训
在训练阶段，网络从大量标记示例数据集中学习。  神经网络的权重经过优化，可以识别训练数据集中包含的模式。  深度神经网络有许多层神经元连接在一起。  更深的网络需要越来越长的时间来训练和评估，但最终能够在其中编码更多的智能。


![替代文本](https://a70ad2d16996820e6285-3c315462976343d903d5b3a03b69072d.ssl.cf2.rackcdn.com/fd4ba9e7e68b76fc41c8312856c7d0ad)


在整个训练过程中，使用试验数据集测试和改进网络的推理性能。与训练数据集一样，试验数据集也用真实值标记，因此可以评估网络的准确性，但不包含在训练数据集中。  网络继续迭代训练，直到达到用户设定的一定精度水平。


由于数据集和深度推理网络的规模，训练通常非常耗费资源，并且在传统计算架构上可能需要数周或数月的时间。  然而，使用 GPU 可以极大地将这一过程缩短至数天或数小时。


##### 数字


使用 [DIGITS](https://developer.nvidia.com/digits)，任何人都可以轻松入门并通过 GPU 加速交互式地训练其网络。  <br />DIGITS 是 NVIDIA 贡献的开源项目，位于: https://github.com/NVIDIA/DIGITS.


本教程将结合使用 DIGITS 和 Jetson TX1 来训练和部署深度学习网络，<br />简称 DIGITS 工作流程: 


![替代文本](https://a70ad2d16996820e6285-3c315462976343d903d5b3a03b69072d.ssl.cf2.rackcdn.com/90bde1f85a952157b914f75a9f8739c2)



#### 推论
网络使用经过训练的权重在运行时评估实时数据。  网络根据学到的例子进行预测和应用推理，这被称为推理。  由于深度学习网络的深度，推理需要大量计算资源来实时处理图像和其他传感器数据。  然而，使用 NVIDIA 的 GPU 推理引擎（使用 Jetson 的集成 NVIDIA GPU），推理可以部署在嵌入式平台上。  拣选、自主导航、农业和工业检查等机器人应用对于部署深度推理有很多用途，包括: 


  - 图像识别

  - 物体检测

  - 分割

  - 图像配准（单应性估计）

  - 原始立体声的深度

  - 信号分析


##
<palign="right">下一个| <b><a href="digits-workflow-CN.md">DIGITS 工作流程</a></b></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>