<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="segnet-training-CN.md">返回</a> | <a href="segnet-console-CN.md">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>语义分割</sup></s></p>


# TensorRT 的 FCN-Alexnet 补丁


原始 FCN-Alexnet 中存在几个非必要层，TensorRT 不支持这些层，应从快照中包含的 `deploy.prototxt` 中删除。


在 `deploy.prototxt` 末尾，删除反卷积层和裁剪层: 


```
layer {
  name: "upscore"
  type: "Deconvolution"
  bottom: "score_fr"
  top: "upscore"
  param {
    lr_mult: 0.0
  }
  convolution_param {
    num_output: 21
    bias_term: false
    kernel_size: 63
    group: 21
    stride: 32
    weight_filler {
      type: "bilinear"
    }
  }
}
layer {
  name: "score"
  type: "Crop"
  bottom: "upscore"
  bottom: "data"
  top: "score"
  crop_param {
    axis: 2
    offset: 18
  }
}
```


在 `deploy.prototxt` 的第 24 行，将 `pad: 100` 更改为 `pad: 0`。


最后，将航空数据集中的 `fpv-labels.txt` 和 `fpv-deploy-colors.txt` 复制到 Jetson 上的模型快照文件夹中。  您的 FCN-Alexnet 模型快照现在与 TensorRT 兼容。  现在我们可以在 Jetson 上运行它并对图像进行推理。


##
<palign="right">下一个| <b><a href="segnet-console-CN.md">在 Jetson 上运行分割模型</a></b>
<br/>
返回 | <b><a href="segnet-training-CN.md">使用 DIGITS 训练 FCN-Alexnet</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>