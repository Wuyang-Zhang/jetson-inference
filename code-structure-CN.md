# 代码结构说明

这份文档专门解释这个仓库里几层最容易混淆的代码：

- `c/`
- `examples/`
- `python/examples/`
- `python/bindings/`

如果你刚开始看代码，最容易搞混的一点就是：

- 为什么会同时有 `detectNet.cpp`
- 又有 `examples/detectnet/detectnet.cpp`
- Python 下面又有 `python/examples/detectnet.py`

它们名字很像，但职责并不一样。

## 一句话先记住

- `c/` = 核心实现层
- `examples/` = C++ 示例层
- `python/bindings/` = Python 绑定层
- `python/examples/` = Python 示例层

## 1. `c/` 是做什么的

[c](g:/jetson-inference/c) 里的代码是这个项目的核心推理实现。

这里定义了真正的网络类，比如：

- [c/imageNet.h](g:/jetson-inference/c/imageNet.h)
- [c/detectNet.h](g:/jetson-inference/c/detectNet.h)
- [c/segNet.h](g:/jetson-inference/c/segNet.h)
- [c/actionNet.h](g:/jetson-inference/c/actionNet.h)
- [c/tensorNet.h](g:/jetson-inference/c/tensorNet.h)

对应实现文件是：

- [c/imageNet.cpp](g:/jetson-inference/c/imageNet.cpp)
- [c/detectNet.cpp](g:/jetson-inference/c/detectNet.cpp)
- [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp)
- [c/actionNet.cpp](g:/jetson-inference/c/actionNet.cpp)
- [c/tensorNet.cpp](g:/jetson-inference/c/tensorNet.cpp)

这些文件做的是真正的核心工作，例如：

- 加载模型
- 申请 TensorRT / CUDA 资源
- 图像预处理
- 推理执行
- 后处理
- 分类结果、检测框、分割 mask 的生成

所以 `c/` 里的代码不是“演示怎么用”，而是“功能本体”。

## 2. `examples/` 是做什么的

[examples](g:/jetson-inference/examples) 里的代码是 C++ 示例程序。

例如：

- [examples/imagenet/imagenet.cpp](g:/jetson-inference/examples/imagenet/imagenet.cpp)
- [examples/detectnet/detectnet.cpp](g:/jetson-inference/examples/detectnet/detectnet.cpp)
- [examples/segnet/segnet.cpp](g:/jetson-inference/examples/segnet/segnet.cpp)
- [examples/actionnet/actionnet.cpp](g:/jetson-inference/examples/actionnet/actionnet.cpp)

这些文件通常做的是：

1. 解析命令行参数
2. 打开摄像头 / 视频 / 图片输入
3. 创建网络对象
4. 调用推理接口
5. 把结果画到画面上
6. 输出到窗口或文件

比如：

- `imagenet.cpp` 会调用 `imageNet::Create()` 和 `Classify()`
- `detectnet.cpp` 会调用 `detectNet::Create()` 和 `Detect()`
- `segnet.cpp` 会调用 `segNet::Create()` 和 `Process()`
- `actionnet.cpp` 会调用 `actionNet::Create()` 和 `Classify()`

所以：

- `examples/` 不是网络实现层
- 它是网络的“C++ 用法示例层”

## 3. `python/bindings/` 是做什么的

[python/bindings](g:/jetson-inference/python/bindings) 的作用，是把 `c/` 里的 C++ 网络类包装成 Python 可以调用的接口。

例如：

- [python/bindings/PyDetectNet.cpp](g:/jetson-inference/python/bindings/PyDetectNet.cpp)
- [python/bindings/PySegNet.cpp](g:/jetson-inference/python/bindings/PySegNet.cpp)
- [python/bindings/PyInference.cpp](g:/jetson-inference/python/bindings/PyInference.cpp)

这一层不是业务 demo，也不是核心推理本体，而是“跨语言适配层”。

你可以把它理解成：

- `c/` 说的是 C++ 世界里的 `detectNet`
- `python/bindings/` 负责把它变成 Python 里的 `jetson_inference.detectNet`

## 4. `python/examples/` 是做什么的

[python/examples](g:/jetson-inference/python/examples) 是 Python 示例层。

例如：

- [python/examples/imagenet.py](g:/jetson-inference/python/examples/imagenet.py)
- [python/examples/detectnet.py](g:/jetson-inference/python/examples/detectnet.py)
- [python/examples/segnet.py](g:/jetson-inference/python/examples/segnet.py)
- [python/examples/actionnet.py](g:/jetson-inference/python/examples/actionnet.py)

它们和 `examples/` 的角色很像，区别只是语言不同：

- `examples/` 用 C++
- `python/examples/` 用 Python

它们同样也是：

1. 打开输入源
2. 创建网络对象
3. 调用推理
4. 显示或保存结果

所以你之前说的那句话，基本是对的：

**`examples/` 和 `python/examples/` 本质上都是示例层，只是一个用 C++ 写，一个用 Python 写。**

## 5. 为什么网络名字会一样

这是这个项目里非常正常、而且是故意的命名方式。

例如：

- [c/detectNet.cpp](g:/jetson-inference/c/detectNet.cpp)
- [examples/detectnet/detectnet.cpp](g:/jetson-inference/examples/detectnet/detectnet.cpp)
- [python/examples/detectnet.py](g:/jetson-inference/python/examples/detectnet.py)

它们都叫“detectnet”，是因为它们围绕的是同一类功能：目标检测。

但它们其实分别代表三层：

- `detectNet`
  C++ 核心类

- `examples/detectnet`
  这个核心类的 C++ 示例程序

- `python/examples/detectnet.py`
  这个核心类的 Python 示例程序

同样的对应关系还有：

- `imageNet` <-> `examples/imagenet` <-> `python/examples/imagenet.py`
- `segNet` <-> `examples/segnet` <-> `python/examples/segnet.py`
- `actionNet` <-> `examples/actionnet` <-> `python/examples/actionnet.py`

所以名字一样，不是重复，而是“同一个能力在不同层的对应关系”。

## 6. 三层调用关系怎么理解

最简单可以画成这样：

### C++ 路线

```text
examples/detectnet/detectnet.cpp
        ↓
   detectNet::Create()
        ↓
      c/detectNet.cpp
        ↓
      c/tensorNet.cpp
        ↓
   TensorRT / CUDA / 模型文件
```

### Python 路线

```text
python/examples/detectnet.py
        ↓
python/bindings/PyDetectNet.cpp
        ↓
      c/detectNet.cpp
        ↓
      c/tensorNet.cpp
        ↓
   TensorRT / CUDA / 模型文件
```

所以最核心的区别是：

- C++ 示例直接调用 `c/`
- Python 示例先经过 `python/bindings/`，再调用 `c/`

## 7. 那 `examples/` 和 `python/examples/` 是不是“同一个东西”

可以说：

- **角色上基本一样**
- **实现路径上不完全一样**

角色上：

- 都是让你学怎么用
- 都是 demo / sample / example

实现路径上：

- C++ 示例直接用 C++ API
- Python 示例用绑定后的 Python API

所以更准确的表述是：

**它们都是示例层，但 Python 示例不是直接调用 `c/`，而是通过 Python 绑定间接调用。**

## 8. 如果你要看源码，建议怎么读

如果你是刚开始学，推荐用“示例 -> 接口 -> 实现”的顺序。

例如想看目标检测：

1. 先看 [examples/detectnet/detectnet.cpp](g:/jetson-inference/examples/detectnet/detectnet.cpp)
2. 再看 [c/detectNet.h](g:/jetson-inference/c/detectNet.h)
3. 最后看 [c/detectNet.cpp](g:/jetson-inference/c/detectNet.cpp)

如果你想看 Python 怎么调用：

1. 先看 [python/examples/detectnet.py](g:/jetson-inference/python/examples/detectnet.py)
2. 再看 [python/bindings/PyDetectNet.cpp](g:/jetson-inference/python/bindings/PyDetectNet.cpp)
3. 再回到 [c/detectNet.cpp](g:/jetson-inference/c/detectNet.cpp)

## 9. 再压缩成一句最短结论

这个仓库同名网络之所以在多个目录里同时出现，是因为它们分属不同层：

- `c/` 负责“实现网络”
- `examples/` 负责“演示 C++ 怎么调用网络”
- `python/bindings/` 负责“把网络暴露给 Python”
- `python/examples/` 负责“演示 Python 怎么调用网络”

所以它们不是重复代码，而是同一能力在不同层次上的分工。
