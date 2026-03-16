<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="imagenet-camera-CN.md">返回</a> | <a href="imagenet-snapshot-CN.md">下一步</a> | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>内容</sup></a>
<br/>
<sup>图像识别</sup></p>


# 重新训练识别网络


存储库下载的现有 GoogleNet 和 AlexNet 模型已在 ImageNet ILSVRC12 基准测试中的 [1000 个对象类别](../data/networks/ilsvrc12_synset_words.txt) 上进行了预训练。


要识别新的对象类，您可以使用 DIGITS 根据新数据重新训练网络。  您还可以以不同的方式组织现有类，包括将多个子类分组为一个子类。  例如，在本教程中，我们将选取 1000 个类中的 230 个类，将它们分为 12 个类并重新训练网络。


我们首先下载要使用的 ILSVRC12 图像，或者您可以在 **[图像文件夹](https://github.com/NVIDIA/DIGITS/blob/master/docs/ImageFolderFormat.md)** 中替换您自己的数据集。


### 下载图像识别数据集


图像识别数据集由大量按分类类型（通常按目录）排序的图像组成。  ILSVRC12 数据集用于默认 GoogleNet 和 AlexNet 模型的训练。  它的大小约为 100GB，包含超过 1000 个不同类别的 100 万张图像。  使用 [`imagenet-download.py`](../tools/imagenet-download.py) 图像爬虫将数据集下载到 DIGITS 服务器。


要下载数据集，首先请确保 DIGITS 服务器上有足够的磁盘空间（建议 120GB），然后从要存储数据集的计算机上的目录运行以下命令: 


``` bash
$ wget --no-check-certificate https://nvidia.box.com/shared/static/gzr5iewf5aouhc5exhp3higw6lzhcysj.gz -O ilsvrc12_urls.tar.gz
$ tar -xzvf ilsvrc12_urls.tar.gz
$ wget https://rawgit.com/dusty-nv/jetson-inference/master/tools/imagenet-download.py
$ python imagenet-download.py ilsvrc12_urls.txt . --jobs 100 --retry 3 --sleep 0
```


在上面的命令中，在启动爬网程序之前会下载图像 URL 列表以及脚本。


> **注意**: 请考虑从公司网络运行图像爬虫，IT 可能会标记该活动。

> 在良好的连接条件下，可能需要一整夜才能下载 1000 个 ILSVRC12 类 (100GB)。


爬虫会将图像下载到与其分类相对应的子目录中。  每个图像类都存储在其自己的目录中，总共有 1000 个目录（ILSVRC12 中每个类一个目录）。  这些文件夹的组织方式类似于: 


```
n01440764/
n01443537/
n01484850/
n01491361/
n01494475/
...
```


这些以 N 为前缀的 8 位 ID 称为该类的 **synset ID**。  类的名称字符串可以在 [`ilsvrc12_synset_words.txt`](../data/networks/ilsvrc12_synset_words.txt) 中查找。  例如，同义词集 `n01484850 great white shark`。


### 自定义对象类


我们在上一步中下载的数据集用于训练默认的 AlexNet 和 GoogleNet 模型，其中包含来自多个核心组的 1000 个对象类，包括不同种类的鸟类、植物、水果和鱼类、狗和猫的品种、车辆类型等。  出于实用目的，让我们考虑 GoogleNet 模型的一个配套模型，该模型可识别由原始 1000 个类组成的十几个核心组（例如，不是检测 122 个单独品种的狗，而是将它们全部组合成一个常见的 `dog` 类）。  这 12 个核心组可能比 1000 个单独的同义词集更实用，并且跨类组合会产生更多的训练数据和更强的组分类。


DIGITS 需要文件夹层次结构中的数据，因此我们可以为组创建目录，然后符号链接到上面下载的 ILSVRC12 中的同义词集。  DIGITS 将自动合并顶级组下所有文件夹中的图像。  目录结构如下所示，括号中的值表示用于组成组的类的数量，箭头旁边的值表示链接到的同义词集 ID。


```
‣ ball/  (7)
	• baseball     (→n02799071)
	• basketball   (→n02802426)
	• soccer ball  (→n04254680)
	• tennis ball  (→n04409515)
	• ...
‣ bear/  (4)
	• brown bear   (→n02132136)
	• black bear   (→n02133161)
	• polar bear   (→n02134084)
	• sloth bear   (→n02134418)
• bike/  (3)
• bird/  (17)
• bottle/ (7)
• cat/  (13)
• dog/  (122)
• fish/   (5)
• fruit/  (12)
• turtle/  (5)
• vehicle/ (14)
• sign/  (2)
```


由于实际上有很多从 ILSVRC12 链接到的同义词集，因此我们提供 **[`imagenet-subset.sh`](../tools/imagenet-subset.sh)** 脚本来生成给定数据集路径的目录结构和链接。  从 DIGITS 服务器运行以下命令: 


``` bash
$ wget https://rawgit.com/dusty-nv/jetson-inference/master/tools/imagenet-subset.sh
$ chmod +x imagenet-subset.sh
$ mkdir 12_classes
$ ./imagenet-subset.sh /opt/datasets/imagenet/ilsvrc12 12_classes
```


在此示例中，链接是在 `12_classes` 文件夹中创建的，脚本的第一个参数是上一步中下载的 ILSVRC12 的路径。


### 将分类数据集导入 DIGITS


将浏览器导航到 DIGITS 服务器实例，然后从“数据集”选项卡下的下拉列表中选择创建新的 `Classification Dataset`: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet-digits-new-dataset-menu.png" width="250">


将 `Training Images` 路径设置为上一步中的 `12_classes` 文件夹，并执行以下操作


* % 用于验证: `10`

* 群组名称: `ImageNet`

* 数据集名称: `ImageNet-ILSVRC12-subset`


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet-digits-new-dataset.png)


使用页面底部的 `Create` 按钮启动数据集导入作业。  数据子集的大小约为 20GB，因此根据服务器 I/O 性能，需要 10-15 分钟。  接下来我们将创建新模型并开始训练它。


### 使用 DIGITS 创建图像分类模型


上一个数据导入作业完成后，返回 DIGITS 主屏幕。  选择 `Models` 选项卡，然后从下拉列表中选择创建新的 `Classification Model`: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet-digits-new-model-menu.png" width="250">


在表单中进行如下设置: 


* 选择数据集: `ImageNet-ILSVRC12-subset`

* 减去平均值: `Pixel`

* 标准网络: `GoogleNet`

* 群组名称: `ImageNet`

* 型号名称: `GoogleNet-ILSVRC12-subset`


选择要训练的 GPU 后，单击底部的 `Create` 按钮开始训练。


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet-digits-new-model.png)


### 在 DIGITS 中测试分类模型


训练作业完成 30 个 epoch 后，训练后的模型应如下所示: 


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet-digits-model.png)


此时，我们可以尝试在 DIGITS 中的一些示例图像上测试新模型的推理。  在与上图相同的页面上，向下滚动到 `Trained Models` 部分。  在 `Test a Single Image` 下，选择要尝试的图像（例如 `/ilsvrc12/n02127052/n02127052_1203.jpg`）: 


<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet-digits-test-single-image.png" width="350">


按 `Classify One` 按钮，您应该看到类似以下内容的页面: 


![替代文本](https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/imagenet-digits-infer-cat.png)


该图像在新的 GoogleNet-12 模型中被分类为 `cat`，而在原始 GoogleNet-1000 中则被分类为 `Lynx`。  这表明新模型工作正常，因为 GoogleNet-12 对猫的训练中包含了 Lynx 类别。


##
<palign="right">下一个| <b><a href="imagenet-snapshot-CN.md">将模型快照下载到 Jetson</a></b>
<br/>
返回 | <b><a href="imagenet-camera-CN.md">运行实时摄像头识别演示</a></p>
</b><palign="center"><sup>© 2016-2019 NVIDIA | </sup><a href="../README-CN.md#two-days-to-a-demo-digits"><sup>目录</sup></a></p>