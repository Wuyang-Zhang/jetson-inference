<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="imagenet-camera-2-CN.md">返回</a> | <a href="detectnet-console-2-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>图像分类</sup></p>


# 用于图像标记的多标签分类


多标签分类模型能够同时识别多个对象类别，以执行图像标记等任务。  多标签 DNN 在拓扑上几乎与普通单类模型相同，只是它们使用 sigmoid 激活层而不是 softmax。  有一个预训练的 `resnet18-tagging-voc` 多标签模型可用，该模型是在 Pascal VOC 数据集上进行训练的: 


<img src=https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet_tagging.jpg>


要启用图像标记，您需要使用您选择的 `--topK=0` 和 `--threshold` 运行 imagenet/imagenet.py: 


`` 重击
# C++
$ imagenet --model=resnet18-tagging-voc --topK=0 --threshold=0.25 "images/object_*.jpg" images/test/tagging_%i.jpg


# Python
$ imagenet.py --model=resnet18-tagging-voc --topK=0 --threshold=0.25 "images/object_*.jpg" images/test/tagging_%i.jpg
````


使用 `--topK=0` 意味着分类器将返回所有置信度分数超过阈值的类。


### 从代码中检索多个图像标签


当指定 topK 参数时，imageNet.Classify() 函数将返回 `(classID, confidence)` 元组。  有关使用多个分类结果的代码示例，请参阅 [`imagenet.cpp`](../examples/imagenet/imagenet.cpp) 或 [`imagenet.py`](../python/examples/imagenet.py): 


#### C++


````.cpp
imageNet::Classifications 分类；	// std::vector<std::pair<uint32_t, float>> (classID, 置信度)


if( net->Classify(图像, 输入->GetWidth(), 输入->GetHeight(), 分类, topK) < 0 )
	继续；


for( uint32_t n=0; n < 分类.size(); n++ )
{
	const uint32_t classID = 分类[n].first;
	const char* classLabel = net->GetClassLabel(classID);
	const float 置信度 = 分类[n].second * 100.0f；


printf("imagenet: %2.5f%% 类 #%i (%s)\n", 置信度, classID, classLabel);	
}
````


#### Python


```` 蟒蛇
预测 = net.Classify(img, topK=args.topK)


对于枚举（预测）中的 n（classID，置信度）: 
   类标签 = net.GetClassLabel(classID)
   置信度 *= 100.0
   print(f"imagenet: {confidence:05.2f}% 类 #{classID} ({classLabel})")
````


请注意，topK 也可以用于单类分类以获得前 N 个结果，尽管这些模型没有经过图像标记训练。


<palign="right">下一个| <b><a href="detectnet-console-2-CN.md">从图像中检测对象</a></b>
<br/>
返回 | <b><a href="imagenet-camera-2-CN.md">运行实时摄像头识别演示</a></p>
</b><palign="center"><sup>© 2016-2023 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>