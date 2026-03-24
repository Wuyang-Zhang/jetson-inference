# tools 目录说明

`tools/` 是这个项目的辅助工具目录。

它不是核心推理库本体，而是围绕项目使用流程提供的配套工具箱，主要负责这些事情：

- 下载模型
- 安装依赖
- 测试模型
- 采集数据
- 准备训练数据集
- 做一些一次性的格式转换或批处理

如果你把整个仓库粗略分层，可以这样理解：

- `c/`、`utils/`
  核心库和底层实现

- `python/`
  Python 训练、绑定和示例

- `examples/`
  常见功能 demo

- `tools/`
  辅助脚本和小工具

## 先说最重要的结论

你平时不需要把 `tools/` 里的东西全部都学一遍。

这个目录里的文件大致分成三类：

### 1. 常用工具

这些是相对更常见、也更值得优先了解的：

- `download-models.sh`
- `install-pytorch.sh`
- `tao-model-downloader.sh`
- `test-models.py`
- `camera-capture/`

### 2. 特定任务才会用到的工具

这些通常只有你在做某一类数据准备或特殊测试时才会碰到：

- `coco2kitti.py`
- `imagenet-download.py`
- `imagenet-subset.sh`
- `cat-dog-dataset.sh`
- `depth-viewer/`

### 3. 偏历史/一次性辅助脚本

这些通常是某个数据集流程里的临时脚本，或者依赖旧环境，不是每个人都要用：

- `cityscapes-prep.sh`
- `cityscapes-prep2.sh`
- `synthia-all-prepare.sh`
- `synthia-seq-prepare.sh`
- `synthia-seq-remap-labels.sh`
- `resize-images.sh`
- `resize-images2.sh`
- `depallet-images.sh`
- `segnet-batch.sh`
- `depthnet-batch.sh`
- `trt-console/`
- `trt-bench/`
- `translate_markdown.py`

## 什么时候该用哪个

如果你不知道先看哪个，可以按需求对照：

| 你现在要做什么 | 应该先看哪个 |
|---|---|
| 我想把项目先跑起来，缺模型 | `download-models.sh` |
| 我想装这个项目常用的 PyTorch | `install-pytorch.sh` |
| 我想下载 NVIDIA TAO 的部署模型 | `tao-model-downloader.sh` |
| 我想批量验证项目自带模型是否正常 | `test-models.py` |
| 我想从摄像头采集和标注分类/检测数据 | `camera-capture/` |
| 我想看深度网络或双目深度效果 | `depth-viewer/` |
| 我想把 COCO 标注转成 KITTI 风格标签 | `coco2kitti.py` |
| 我想下载 ImageNet 图片 | `imagenet-download.py` |
| 我想从大 ImageNet 数据集中做一个较小子集 | `imagenet-subset.sh` |
| 我想快速生成一个猫狗分类数据集 | `cat-dog-dataset.sh` |
| 我想整理 Cityscapes 分割数据 | `cityscapes-prep.sh` |
| 我想整理 SYNTHIA 分割数据 | `synthia-*.sh` |
| 我想批量缩放或去掉调色板 PNG | `resize-images.sh` / `depallet-images.sh` |
| 我想批量跑分割或深度推理 | `segnet-batch.sh` / `depthnet-batch.sh` |

## 这个目录的构建状态

从 [tools/CMakeLists.txt](g:/jetson-inference/tools/CMakeLists.txt) 可以看到，当前顶层会正式接入这个目录，但并不是所有工具都会默认编译。

当前比较明确的是：

- 会进入：
  - `camera-capture/`
  - `depth-viewer/`

- 其中 `depth-viewer/` 本身还受 `BUILD_EXPERIMENTAL` 控制

- `trt-bench/`、`trt-console/` 在顶层 `tools/CMakeLists.txt` 里是注释掉的

也就是说：

- `camera-capture` 相对更正式
- `depth-viewer` 属于实验性
- `trt-bench`、`trt-console` 更像保留工具，不是当前默认主流程的一部分

## 使用前的注意事项

这个目录里的大多数脚本都有这些共同特点：

- 主要面向 Linux / Ubuntu / Jetson 环境
- 很多脚本会直接修改当前目录下的数据文件
- 有些脚本依赖额外命令行工具，比如：
  - `wget`
  - `dialog`
  - `mmv`
  - `convert`（ImageMagick）
  - `pycocotools`
- 有些脚本写法比较老，路径是硬编码的，需要你先改脚本里的变量

所以不要把这里的脚本当成“都能开箱即用”的稳定 CLI 工具。更准确地说，它们是：

- 一部分正式辅助工具
- 一部分项目作者自己使用的数据准备脚本

## 每个文件/目录的作用、怎么用、什么时候用

### `CMakeLists.txt`

作用：

- 定义 `tools/` 这个目录如何接入整个项目构建
- 指定哪些子工具参与编译
- 指定哪些脚本会复制/安装到输出目录

什么时候看它：

- 你想知道哪些工具会随项目一起构建
- 你想知道为什么某个工具没有被编出来

怎么用：

- 它不是直接运行的文件
- 只有在研究构建流程时需要看

### `download-models.sh`

作用：

- 下载项目常用的预训练模型到 `../data/networks`

什么时候用：

- 你第一次配置项目
- demo 运行时提示找不到模型
- 你想补全项目自带模型

怎么用：

```bash
cd tools
./download-models.sh
```

脚本特点：

- 会调用 `wget`
- 默认带交互重试逻辑
- 输出目录是仓库里的 `data/networks`

优先级：

- 很高
- 如果你只是想把项目跑起来，这是 `tools/` 里最常用的脚本之一

### `download-models.rc`

作用：

- `dialog` 的界面配置文件

什么时候用：

- 基本不用手动碰
- 除非你在改 `download-models.sh` 的交互界面样式

怎么用：

- 一般不直接运行

### `install-pytorch.sh`

作用：

- 按项目预期环境下载/安装 PyTorch 相关包

什么时候用：

- 你在 Jetson / Ubuntu 环境里配置训练或 Python 推理环境
- 你想沿用这个仓库作者预设的 PyTorch 安装方式

怎么用：

```bash
cd build
./install-pytorch.sh
```

或者从 `tools/` 里直接看脚本逻辑。

脚本特点：

- 会下载 wheel 或相关包
- 依赖系统环境和 JetPack / L4T 版本
- 带交互重试逻辑

优先级：

- 高
- 但如果你已经有自己稳定的 PyTorch 安装方式，不一定必须用它

### `install-pytorch.rc`

作用：

- `install-pytorch.sh` 对应的 `dialog` 界面配置文件

什么时候用：

- 一般不用手工改

### `tao-model-downloader.sh`

作用：

- 下载 NVIDIA TAO 训练/部署模型及配套文件

什么时候用：

- 你要用 TAO 导出的模型
- 你需要对应的：
  - `.onnx` / `.etlt`
  - `labels.txt`
  - `colors.txt`
  - calibration 文件

怎么用：

它更像一个内部菜单式/函数式下载脚本，不是特别通用的极简命令。通常需要在 Linux/Jetson 环境里按脚本预设方式使用。

优先级：

- 中等
- 只有在你明确要用 TAO 模型时才需要

### `l4t_version.sh`

作用：

- 检测当前系统的架构和 L4T 版本

什么时候用：

- 你在 Jetson 上排查环境版本
- 某个安装脚本依赖 L4T 版本判断

怎么用：

```bash
./l4t_version.sh
```

它会输出类似：

- 架构
- L4T Release / Revision

优先级：

- 中等
- 主要是环境排查辅助工具

### `test-models.py`

作用：

- 批量测试项目中的模型输出是否符合预期

什么时候用：

- 你改了推理代码，想做回归测试
- 你想验证内置模型能不能正常跑
- 你在 CI 或本地做结果对比

怎么用：

```bash
python3 test-models.py
python3 test-models.py --module detectnet
python3 test-models.py --generate
python3 test-models.py --no-python
```

关键参数：

- `--module`
  只测某个模块

- `--generate`
  重新生成期望输出

- `--no-python`
  跳过 Python 模块测试

- `--python-only`
  只测 Python 模块

- `--stop-on-failure`
  一旦失败就停

- `--verbose`
  打印更详细的子进程输出

优先级：

- 高
- 适合开发者和维护者

### `benchmark-models.sh`

作用：

- 用 TensorRT 的 `trtexec` 跑一组 ONNX 模型基准测试

什么时候用：

- 你想比较不同 segmentation 模型的速度
- 你在做性能记录或横向对比

怎么用：

```bash
./benchmark-models.sh <log-dir> <iterations> <runs>
```

例如：

```bash
./benchmark-models.sh benchmark_logs 10 10
```

注意：

- 依赖 `/usr/src/tensorrt/bin/trtexec`
- 主要针对分割模型列表

优先级：

- 中等
- 更偏性能测试，不是日常入门必备

### `coco2kitti.py`

作用：

- 把 MS COCO 的标注 JSON 转成 KITTI 风格的检测标签文本

什么时候用：

- 你想把 COCO 数据接到使用 KITTI 标签格式的训练流程
- 你在准备检测数据集转换

怎么用：

这个脚本默认假设：

- 当前目录结构接近 COCO 的 `annotations/`
- 输出写到当前目录下 `./labels`

通常需要先按你自己的数据路径改脚本里这些变量：

- `dataDir`
- `dataType`
- `annFile`
- `catNms`

再运行：

```bash
python3 coco2kitti.py
```

优先级：

- 中等
- 只有在做 COCO -> KITTI 转换时才需要

### `imagenet-download.py`

作用：

- 按图片 URL 列表批量下载 ImageNet 图片

什么时候用：

- 你手上有 ImageNet URL 列表文件
- 你想批量拉取指定类别样本

怎么用：

它是一个带参数的 Python 下载器，但脚本写法偏旧，使用的是 Python 2 风格模块：

- `Queue`
- `urllib2`

所以只有在兼容环境里才适合直接用，或者你自己先改造后再用。

优先级：

- 较低
- 更偏历史下载脚本

### `imagenet-subset.sh`

作用：

- 从 ILSVRC12 目录中挑出一部分语义类，组织成较小的子集

什么时候用：

- 你已经有完整或较大的 ILSVRC12 数据目录
- 想快速做一个较小的分类数据集

怎么用：

```bash
./imagenet-subset.sh <ilsvrc12绝对路径> <输出路径>
```

脚本特点：

- 使用符号链接而不是复制原图
- 会创建像 `cat/`、`dog/`、`bird/`、`bear/` 这类大类目录

优先级：

- 中等
- 适合做教学、小样本实验、快速子集整理

### `cat-dog-dataset.sh`

作用：

- 从一个较大的 ImageNet 子集中随机抽样，生成猫狗分类数据集

什么时候用：

- 你想快速做一个猫狗二分类训练集
- 你手头已经有脚本要求的 ImageNet 子集目录

怎么用：

这个脚本主要靠修改脚本顶部变量来使用：

- `IMAGENET_DIR`
- `OUTPUT_DIR`
- `NUM_TRAIN`
- `NUM_VAL`
- `NUM_TEST`

改完后运行：

```bash
./cat-dog-dataset.sh
```

输出会生成：

- `train/cat`
- `train/dog`
- `val/cat`
- `val/dog`
- `test/cat`
- `test/dog`

优先级：

- 中等
- 适合快速造一个教学级二分类数据集

### `cityscapes-prep.sh`

作用：

- 整理 Cityscapes 数据集文件结构
- 把图像和标签分别汇总到 `train/images`、`train/labels`、`val/images`、`val/labels`

什么时候用：

- 你在准备 Cityscapes 分割数据
- 你想把原始目录整理成更扁平的训练目录

怎么用：

它主要依赖脚本里写死的路径：

- `c=~/Downloads/cityscapes`

通常需要先修改路径，再在 Cityscapes 数据目录附近执行。

注意：

- 会重命名和移动文件
- 依赖 `mmv`
- 更像一次性数据整理脚本

优先级：

- 较低
- 只在做 Cityscapes 数据准备时用

### `cityscapes-prep2.sh`

作用：

- 对 `cityscapes-prep.sh` 的某一小段补充操作，主要是移动 `gtCoarse/val` 标签

什么时候用：

- 你明确知道自己在延续旧的 Cityscapes 整理流程

怎么用：

- 先改脚本里硬编码路径
- 再执行

优先级：

- 很低
- 基本属于特定历史流程辅助脚本

### `depallet-images.sh`

作用：

- 用 ImageMagick 把 PNG 转成非调色板形式

什么时候用：

- 某些 PNG 标签图有 palette/color-type 问题
- 下游程序不接受调色板 PNG

怎么用：

```bash
./depallet-images.sh <目录>
```

注意：

- 脚本第二个参数虽然声明了 `FACTOR`，但实际没有使用
- 会原地覆盖 PNG
- 依赖 `convert`

优先级：

- 低
- 只有碰到 PNG 色板兼容问题时才用

### `resize-images.sh`

作用：

- 批量缩放一个目录下的 PNG 图片

什么时候用：

- 你在压缩训练数据分辨率
- 你想快速做低分辨率版本的数据集

怎么用：

```bash
./resize-images.sh <目录> <百分比>
```

例如：

```bash
./resize-images.sh train/images 50
```

注意：

- 会原地覆盖图片
- 依赖 `convert`

### `resize-images2.sh`

作用：

- 批量调用 `resize-images.sh` 处理固定目录：
  - `train/images`
  - `train/labels`
  - `val/images`
  - `val/labels`

什么时候用：

- 你刚好采用了这套固定目录结构

怎么用：

```bash
./resize-images2.sh
```

注意：

- 它本质只是一个快捷批处理封装
- 目录写死，不够通用

### `segnet-batch.sh`

作用：

- 对一个目录里的 PNG 批量运行 `segnet-console`

什么时候用：

- 你想批量生成语义分割结果

怎么用：

```bash
./segnet-batch.sh <输入目录> <segnet-console参数>
```

注意：

- 依赖 `segnet-console`
- 会把每个输入文件作为输入和输出路径传进去，这种写法比较粗糙，更适合你自己改造后再批量用

### `depthnet-batch.sh`

作用：

- 对一个目录下的 JPG 批量运行 `depthnet-console`

什么时候用：

- 你想批量生成深度图结果

怎么用：

```bash
./depthnet-batch.sh <输入目录> <输出目录> <网络名>
```

例如：

```bash
./depthnet-batch.sh images out monodepth-fcn-mobilenet
```

### `make-legends.sh`

作用：

- 调用 `segnet-console` 生成各个 segmentation 模型对应的类别图例图片

什么时候用：

- 你在做文档、演示、结果解释
- 你想导出不同分割模型的 legend

怎么用：

```bash
./make-legends.sh <输出目录>
```

例如：

```bash
./make-legends.sh legends
```

### `synthia-seq-remap-labels.sh`

作用：

- 对 SYNTHIA 数据集标签图逐张运行 `seg-img-tool`

什么时候用：

- 你在准备 SYNTHIA 分割标签
- 你需要把标签图重新映射成项目使用的类别编码

怎么用：

```bash
./synthia-seq-remap-labels.sh <SYNTHIA序列目录>
```

### `synthia-seq-prepare.sh`

作用：

- 整理一个 SYNTHIA 序列
- 重命名文件并汇总到统一输出目录

什么时候用：

- 你要把多个 SYNTHIA 子序列并成一个训练集

怎么用：

```bash
./synthia-seq-prepare.sh <序列号> <输入目录> <输出目录>
```

注意：

- 会移动文件
- 会调用 `mmv`

### `synthia-all-prepare.sh`

作用：

- 批量串联多个 SYNTHIA 序列准备步骤

什么时候用：

- 你完全沿用这个仓库原来的 SYNTHIA 数据整理方式

怎么用：

直接运行：

```bash
./synthia-all-prepare.sh
```

但它里面路径几乎都是硬编码的 `~/Downloads/...`，所以通常需要先改路径。

### `translate_markdown.py`

作用：

- 一个本地 Markdown 翻译辅助脚本

什么时候用：

- 只有你自己要批量把英文 Markdown 转成中文时才可能用

怎么用：

- 它依赖网络翻译接口
- 更像仓库里的维护辅助脚本，不属于项目运行主流程

优先级：

- 很低

## 子目录工具说明

### `camera-capture/`

作用：

- 一个带 GUI 的数据采集和标注工具

最适合什么场景：

- 你拿着摄像头实时采图
- 想一边看画面一边保存样本
- 想做人脸/物体/分类数据采集

入口文件：

- `camera-capture.cpp`
  主程序入口，支持 `camera-capture [-h] input_URI`

- `captureWindow.cpp/.h`
  负责视频采集和显示窗口

- `controlWindow.cpp/.h`
  负责主控制界面

- `controlClassify.cpp/.h`
  负责分类数据采集相关控制逻辑

- `controlDetection.cpp/.h`
  负责检测数据采集相关控制逻辑

- `CMakeLists.txt`
  构建脚本，依赖 Qt5、`jetson-utils`、`jetson-inference`

- `README.md`
  极简英文说明

怎么用：

```bash
camera-capture csi://0
```

或者：

```bash
camera-capture /dev/video0
camera-capture file.mp4
```

什么时候优先用它：

- 你要自己采集训练数据时
- 这是 `tools/` 里相对最实用、最“像产品”的一个 GUI 工具

### `depth-viewer/`

作用：

- 可视化深度网络、双目深度和分割叠加结果

最适合什么场景：

- 你在调试单目/双目深度估计
- 想直观看深度图和叠加效果

入口文件：

- `depth-viewer.cpp`
  主程序入口，支持这些参数：
  - `--camera`
  - `--width`
  - `--height`
  - `--calibration`
  - `--depth`
  - `--stereo`
  - `--colormap`
  - `--filter-mode`
  - `--segmentation`
  - `--alpha`

- `depthWindow.cpp/.h`
  深度显示窗口逻辑

- `CMakeLists.txt`
  构建脚本，只在 `BUILD_EXPERIMENTAL` 开启时编译

怎么用：

```bash
depth-viewer --camera=0
```

或者：

```bash
depth-viewer left.jpg right.jpg --stereo=<模型名>
```

什么时候用：

- 你明确在做深度相关实验时
- 否则平时可以先忽略它

### `seg-img-tool/`

作用：

- 对 segmentation 标签图做像素级转换/检查

最适合什么场景：

- 你要处理标签 PNG
- 你要把一类颜色编码图转成项目内部期望的类别图

入口文件：

- `seg-img-tool.cpp`
  主程序入口，基本形式是：

```bash
seg-img-tool <输入PNG> <输出PNG>
```

特点：

- 更像数据预处理小工具
- 常被 `synthia-seq-remap-labels.sh` 这种脚本调用

什么时候用：

- 只有在做分割标签预处理时才用

### `trt-bench/`

作用：

- 一个较老的 TensorRT 并发推理基准测试工具

入口文件：

- `trt-bench.cpp`
  启动方式大致是：

```bash
trt-bench --image=<path> [--GPU=FP16|INT8] [--DLA_0=FP16] [--DLA_1=FP16] [--allowGPUFallback]
```

什么时候用：

- 你在做底层 TensorRT / DLA 并发性能实验

什么时候不建议优先用：

- 你只是想正常运行项目 demo
- 因为它现在不是默认构建目标

### `trt-console/`

作用：

- 目前几乎是空壳程序

入口文件：

- `trt-console.cpp`
  当前 `main()` 直接返回，没有实际逻辑

什么时候用：

- 基本不用

什么时候可以忽略：

- 几乎总是可以忽略

## 给初学者的建议顺序

如果你刚开始看 `tools/`，建议按这个顺序了解：

1. `download-models.sh`
2. `install-pytorch.sh`
3. `test-models.py`
4. `camera-capture/`
5. 只在需要时再看数据准备脚本

这样最省时间，因为这几个最接近日常使用。

## 最后给你一句话总结

`tools/` 不是“项目核心代码目录”，而是“围绕这个项目干活时会用到的工具箱”。

如果你当前目标只是把项目跑起来，优先看：

- `download-models.sh`
- `install-pytorch.sh`
- `test-models.py`
- `camera-capture/`

其他很多脚本都属于：

- 某种数据集转换流程才会用
- 或者作者自己当时写的批处理辅助脚本

所以看到它们不用紧张，不是每个都必须学。
