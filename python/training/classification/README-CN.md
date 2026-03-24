# 图像分类训练说明

这个目录用于训练图像分类模型，并把训练好的 PyTorch 模型导出成 ONNX，方便后续部署到 `jetson-inference` 的推理流程中。

如果你刚开始接触这个目录，可以先把它理解成一条很简单的流程：

1. 准备数据集
2. 用 `train.py` 训练或微调分类模型
3. 在 `models/` 里得到 `model_best.pth.tar`
4. 用 `onnx_export.py` 导出成 ONNX
5. 用 `onnx_validate.py` 检查导出的 ONNX 是否正常

## 这个目录里每个文件是干什么的

- `README.md`
  原始英文说明。它来自较早版本，整体思路仍然可参考，但里面的训练脚本名写的是 `main.py`，而当前目录里实际使用的是 `train.py`。

- `README-CN.md`
  这份中文说明文档。适合刚开始看这个目录时先通读一遍。

- `train.py`
  训练主脚本。负责：
  - 解析命令行参数
  - 加载数据集
  - 创建模型
  - 把模型最后一层改成适配你的分类数
  - 训练、验证、保存 checkpoint
  - 输出 `labels.txt`
  - 记录 TensorBoard 日志

- `reshape.py`
  模型结构适配脚本。不同网络最后一层名字不一样，比如 `resnet` 用的是 `fc`，`alexnet/vgg` 用的是 `classifier`。这个文件专门负责把这些网络的最后输出层改成你的类别数。

- `onnx_export.py`
  ONNX 导出脚本。它会读取训练好的 `.pth.tar` checkpoint，恢复模型结构和权重，然后导出成 `.onnx` 文件。

- `onnx_validate.py`
  ONNX 检查脚本。用于验证导出的 ONNX 模型格式是否正常，能否通过 `onnx.checker`。

- `voc.py`
  Pascal VOC 数据集适配器。把 VOC 检测标注里的目标类别提取出来，转成多标签分类任务可用的数据集。

- `nuswide.py`
  NUS-WIDE 数据集适配器。用于多标签图像分类。

- `requirements.txt`
  这个目录额外依赖的 Python 包，当前主要是 `torch`、`torchvision`、`tensorboard`。

- `data/`
  数据目录占位。仓库默认不带真实数据集，这里只是留一个位置给你放训练数据。

- `models/`
  模型输出目录占位。训练后的 checkpoint、最优模型、导出的 ONNX 通常都会放在这里。

- `.gitignore`
  忽略训练过程中生成的大文件，比如模型权重、ONNX、样例图片和 `data/`、`models/` 目录内容。

## 这个目录到底在做什么

这里做的是图像分类训练，不是目标检测，也不是分割。

图像分类的意思通常是：

- 输入一张图片
- 模型输出这张图属于哪个类别

例如：

- 猫 / 狗 二分类
- 苹果 / 香蕉 / 橙子 多分类

这里也支持多标签分类，也就是一张图里可以同时有多个标签，例如：

- 一张图同时有 `person`、`dog`、`car`

这时就不是“只能选一个类别”，而是“多个标签都可能成立”。

## 当前代码实际支持哪些数据集形式

`train.py` 里通过 `--dataset-type` 控制数据集类型，当前支持这三种：

- `folder`
  最常见。使用 `torchvision.datasets.ImageFolder` 读取目录结构。

- `voc`
  使用 `voc.py`，把 Pascal VOC 转成多标签分类数据。

- `nuswide`
  使用 `nuswide.py`，读取 NUS-WIDE 的图像和 CSV 标签。

### 1. folder 模式

这是最适合初学者先用的模式。

目录一般类似这样：

```text
your-dataset/
  train/
    cat/
      001.jpg
      002.jpg
    dog/
      001.jpg
      002.jpg
  val/
    cat/
      101.jpg
    dog/
      101.jpg
```

在这种模式下：

- 子目录名就是类别名
- `train/` 用于训练
- `val/` 用于验证

### 2. voc 模式

适合多标签分类任务。

`voc.py` 会读取 VOC 数据集里的：

- `Annotations/`
- `ImageSets/Main/`
- `JPEGImages/`

然后根据每张图中出现了哪些目标，把它们转成多标签分类标签。

### 3. nuswide 模式

同样是多标签分类。

`nuswide.py` 会读取：

- 图像路径列表
- `classification_labels` 下的 CSV 标签文件

然后把每张图对应的多个标签读出来。

## 最重要的脚本：train.py

如果你只先学一个文件，先看 `train.py`。

它大致做了这些事：

1. 读取命令行参数
2. 选择数据集类型
3. 创建训练集和验证集
4. 根据 `--arch` 创建 torchvision 模型
5. 调用 `reshape_model()` 把最后输出层改成你的类别数
6. 选择损失函数
7. 开始训练和验证
8. 保存 checkpoint 和最优模型

### 常见参数

- `data`
  数据集路径

- `--dataset-type`
  数据集类型，支持 `folder`、`voc`、`nuswide`

- `--model-dir`
  模型输出目录，默认是 `models`

- `--arch`
  模型结构，例如 `resnet18`、`resnet50`、`alexnet`、`vgg16` 等

- `--resolution`
  输入图像分辨率，默认 `224`

- `--batch-size`
  batch 大小，默认 `8`

- `--epochs`
  训练轮数，默认 `35`

- `--resume`
  从已有 checkpoint 恢复训练

- `--evaluate`
  只做验证，不继续训练

- `--pretrained`
  使用 torchvision 预训练权重

- `--multi-label`
  开启多标签分类模式

- `--multi-label-threshold`
  多标签模式下，判断某个标签是否算命中的阈值

## 单标签和多标签有什么区别

### 单标签分类

一张图只属于一个类别，例如：

- 这是一张猫
- 这是一张狗

这时模型通常输出一组分数，最后只选一个最高类别。

### 多标签分类

一张图可以同时属于多个类别，例如：

- 这张图同时有 `person`
- 同时也有 `dog`
- 同时还有 `bicycle`

在 `train.py` 里：

- 单标签时使用普通分类逻辑
- 多标签时会启用 `BCEWithLogitsLoss`

而在 `onnx_export.py` 里：

- 多标签模型默认会补一个 `Sigmoid`
- 单标签模型默认会补一个 `Softmax`

## 为什么还要有 reshape.py

因为不同模型最后分类层的位置不一样。

例如：

- `resnet` 的最后层通常是 `fc`
- `alexnet` / `vgg` 的最后层在 `classifier`
- `squeezenet`、`densenet`、`mobilenet`、`efficientnet` 的写法又不一样

所以不能简单只写一行代码改所有模型。`reshape.py` 的作用，就是根据不同架构，把最后输出层改成正确的类别数。

## 训练会生成什么文件

训练过程中，`train.py` 会在 `--model-dir` 指定的目录下生成这些内容：

- `checkpoint.pth.tar`
  当前轮次保存的 checkpoint

- `model_best.pth.tar`
  当前验证效果最好的模型

- `labels.txt`
  类别名称列表

- `tensorboard/`
  TensorBoard 日志目录

其中最常用的是：

- `model_best.pth.tar`
- `labels.txt`

## 最常见的使用方式

### 1. 训练一个普通目录分类数据集

```bash
python train.py data/my-dataset --dataset-type folder --arch resnet18 --batch-size 8 --epochs 35 --model-dir models/my-dataset
```

### 2. 从 checkpoint 恢复训练

```bash
python train.py data/my-dataset --dataset-type folder --resume models/my-dataset/checkpoint.pth.tar --model-dir models/my-dataset
```

### 3. 只做验证

```bash
python train.py data/my-dataset --dataset-type folder --resume models/my-dataset/model_best.pth.tar --evaluate
```

### 4. 导出 ONNX

```bash
python onnx_export.py --input model_best.pth.tar --model-dir models/my-dataset
```

### 5. 检查 ONNX

```bash
python onnx_validate.py --model models/my-dataset/resnet18.onnx
```

## ONNX 导出脚本在做什么

`onnx_export.py` 的流程大致是：

1. 读取训练好的 checkpoint
2. 找到里面记录的 `arch`、`num_classes`、`resolution`
3. 重新创建同样的 torchvision 模型
4. 用 `reshape_model()` 改成相同类别数
5. 加载 `state_dict`
6. 按需要补上 `Softmax` 或 `Sigmoid`
7. 导出成 `.onnx`

这也是为什么训练时保存的 checkpoint 里，不只是权重，还会保存：

- `arch`
- `resolution`
- `classes`
- `num_classes`
- `multi_label`

这些信息后面导出时都要用到。

## 给刚开始看代码的人一个建议阅读顺序

如果你对这个目录还不熟，建议按这个顺序看：

1. 先看这份 `README-CN.md`
2. 再看 `train.py`
3. 然后看 `reshape.py`
4. 再看 `onnx_export.py`
5. 最后按需要看 `voc.py` 或 `nuswide.py`

原因很简单：

- `train.py` 是主流程
- `reshape.py` 是模型结构适配
- `onnx_export.py` 是部署前转换
- `voc.py` 和 `nuswide.py` 是特定数据集支持

## 原 README 里的内容，怎么理解

原英文 `README.md` 主要想表达的是：

- 这里基于 PyTorch 训练常见分类网络
- 可以使用 ResNet、AlexNet、VGG 等模型
- 需要自己准备数据集
- 训练完成后可以继续用于推理部署

不过要注意两点：

1. 原文是较早版本，训练命令里写的是 `main.py`
   当前实际脚本名是 `train.py`

2. 原文主要围绕 ImageNet 风格训练写的
   但当前代码已经扩展支持：
   - `folder`
   - `voc`
   - `nuswide`
   - 多标签分类
   - ONNX 导出

所以实际使用时，应该优先以当前代码和这份中文说明为准。

## 最后给初学者一句话总结

如果你现在只想先跑通一次，不要一开始就看 `voc.py` 或 `nuswide.py`。

先准备一个最简单的 `train/类名/图片`、`val/类名/图片` 目录数据集，然后直接从 `train.py` 开始。等你把普通分类流程跑通，再去看多标签数据集脚本，会更容易理解。
