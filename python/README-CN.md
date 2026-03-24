# `python/CMakeLists.txt` 说明

这份文档说明两个文件的分工：

- 仓库根目录的 [`CMakeLists.txt`](../CMakeLists.txt)
- Python 子目录的 [`python/CMakeLists.txt`](./CMakeLists.txt)

同时也补充说明真正负责构建 Python 扩展模块的
[`python/bindings/CMakeLists.txt`](./bindings/CMakeLists.txt) 在整个链路中的位置。

## 先说结论

可以把它们理解成三层：

1. 根目录 `CMakeLists.txt`
   负责整个项目的总构建流程。
2. `python/CMakeLists.txt`
   负责 Python 子系统的组织和分发。
3. `python/bindings/CMakeLists.txt`
   负责真正把 C++/CUDA 代码编译成 Python 可导入的扩展模块。

也就是说：

- 顶层 `CMakeLists.txt` 是总控
- `python/CMakeLists.txt` 是 Python 分控
- `python/bindings/CMakeLists.txt` 才是 Python 绑定的具体实现层

## 根目录 `CMakeLists.txt` 的作用

根目录 [`CMakeLists.txt`](../CMakeLists.txt) 是整个 `jetson-inference` 项目的主入口。
它做的是“全项目级别”的事情，而不只是 Python。

它主要负责：

- 检查 git submodule 是否齐全
- 设置全局构建选项，例如 `BUILD_DEPS`、`BUILD_INTERACTIVE`、`BUILD_EXPERIMENTAL`
- 检测系统发行版信息
- 处理依赖安装脚本
- 查找 CUDA、OpenCV、VPI 等系统级依赖
- 设置全局输出目录，例如 `bin/`、`lib/`
- 编译核心共享库 `jetson-inference`
- 安装头文件、核心库、CMake 导出配置
- 把 `docs`、`examples`、`tools`、`utils`、`python` 这些子目录纳入构建

其中很关键的一点是：

根目录文件会先构建核心 C++ 库 `jetson-inference`，然后再通过
[`add_subdirectory(python)`](../CMakeLists.txt#L190) 进入 Python 子目录。

这说明 Python 绑定并不是独立工程，而是建立在整个项目核心库已经被配置好的前提上。

## `python/CMakeLists.txt` 的作用

[`python/CMakeLists.txt`](./CMakeLists.txt) 不是整个项目的主入口，它只在顶层
`CMakeLists.txt` 调用 `add_subdirectory(python)` 之后才会执行。

它主要负责的是 Python 子系统的“调度”和“打包组织”，而不是直接完成全部编译工作。

它主要做三件事：

### 1. 选择要构建的 Python 版本

它会根据 Ubuntu 发行版代号选择要尝试构建的 Python 版本，例如：

- `noble` -> Python 3.12
- `jammy` -> Python 3.10
- `focal` -> Python 3.8

如果不是这些发行版，则尝试旧版本列表。

这部分逻辑在 [`python/CMakeLists.txt`](./CMakeLists.txt#L7) 开始。

### 2. 为每个 Python 版本进入 `bindings/`

它会遍历 `PYTHON_BINDING_VERSIONS`，然后对每个版本执行：

```cmake
add_subdirectory(bindings bindings_python_${PYTHON_BINDING_VERSION})
```

这一步的意思不是“直接构建 Python 模块”，而是把真正的工作交给
[`python/bindings/CMakeLists.txt`](./bindings/CMakeLists.txt)。

所以 `python/CMakeLists.txt` 更像一个版本分发器或调度器。

### 3. 复制和安装 Python 示例

除了绑定构建，它还会：

- 把 `python/examples/*.py` 复制到运行输出目录
- 在 `make install` 时把这些 Python 示例安装到 `bin`
- 为旧示例文件名保留一层兼容复制逻辑

所以这个文件除了“组织 Python 绑定构建”，还兼顾“分发 Python 示例脚本”。

## `python/bindings/CMakeLists.txt` 为什么也重要

如果只看 [`python/CMakeLists.txt`](./CMakeLists.txt)，容易误以为它已经负责了
Python 绑定的实际编译。但真正把 Python 模块编译出来的，是
[`python/bindings/CMakeLists.txt`](./bindings/CMakeLists.txt)。

这个文件才真正负责：

- 查找 Python 解释器、开发头文件、NumPy
- 处理不同 CMake / Python 版本的兼容逻辑
- 编译 Python 扩展模块
- 把输出模块命名为 `jetson_inference_python`
- 链接 `jetson-inference` 和 `jetson-utils-python-*`
- 把 `Jetson`、`jetson`、`jetson_inference` 这些纯 Python 包目录安装到
  Python 的 `site-packages` / `dist-packages`

所以职责划分是：

- `python/CMakeLists.txt` 决定“构建哪些 Python 版本”
- `python/bindings/CMakeLists.txt` 决定“每个 Python 版本具体怎么构建”

## 它和根目录 `CMakeLists.txt` 的核心区别

最核心的区别不是“都叫 CMakeLists”，而是它们的作用域完全不同。

### 1. 作用范围不同

- 根目录 `CMakeLists.txt` 面向整个项目
- `python/CMakeLists.txt` 只面向 Python 子系统

### 2. 目标层级不同

- 根目录负责核心 C++/CUDA 库、全局依赖、全局输出路径
- `python/CMakeLists.txt` 负责 Python 版本调度和 Python 示例安装

### 3. 是否是入口文件不同

- 根目录 `CMakeLists.txt` 是整个工程的主入口
- `python/CMakeLists.txt` 是子目录入口，必须由顶层调用

### 4. 产物不同

- 根目录主要产出 `jetson-inference` 共享库和全项目安装内容
- `python/CMakeLists.txt` 本身主要产出“子构建任务”和示例安装逻辑
- 真正的 Python 扩展产物来自 `python/bindings/CMakeLists.txt`

### 5. 依赖关系不同

- Python 绑定依赖顶层已经配置好的核心库和全局环境
- 顶层不依赖 Python 子目录才能完成核心库构建

换句话说：

根目录 `CMakeLists.txt` 即使不考虑 Python，仍然是项目主干；
而 `python/CMakeLists.txt` 离开顶层上下文就不完整。

## 一个更直观的理解方式

可以把这三个文件想象成这样：

- 根目录 `CMakeLists.txt`：项目总导演
- `python/CMakeLists.txt`：Python 部分的制片统筹
- `python/bindings/CMakeLists.txt`：真正动手拍 Python 扩展的执行层

如果只关心“为什么 Python 能 `import jetson_inference`”，真正相关的关键文件是：

1. [`CMakeLists.txt`](../CMakeLists.txt)
   先把核心库和全局环境准备好。
2. [`python/CMakeLists.txt`](./CMakeLists.txt)
   决定要为哪些 Python 版本生成绑定。
3. [`python/bindings/CMakeLists.txt`](./bindings/CMakeLists.txt)
   真正把扩展模块和 Python 包安装进去。

## 简短总结

一句话概括：

- 根目录 `CMakeLists.txt` 管整个项目
- `python/CMakeLists.txt` 管 Python 子系统的调度和示例安装
- `python/bindings/CMakeLists.txt` 管 Python 扩展模块的实际构建与安装

所以 `python/CMakeLists.txt` 和顶层 `CMakeLists.txt` 不是“重复关系”，
而是“总控”和“子模块控制”的关系。

## 这几个 `CMakeLists.txt` 有构建顺序吗？

有顺序，但要分成两层理解：

1. CMake 读取脚本时的顺序
2. 实际编译 target 时的顺序

这两种顺序不是一回事。

### 1. 配置阶段有“读取顺序”

在 CMake 配置阶段，CMake 会按脚本执行流程读取这些 `CMakeLists.txt`。

在这个仓库里，大致顺序是：

1. 先读取根目录 [`CMakeLists.txt`](../CMakeLists.txt)
2. 根目录执行到 `add_subdirectory(python)` 后，进入 [`python/CMakeLists.txt`](./CMakeLists.txt)
3. [`python/CMakeLists.txt`](./CMakeLists.txt) 再根据 `PYTHON_BINDING_VERSIONS`
   循环进入 [`python/bindings/CMakeLists.txt`](./bindings/CMakeLists.txt)

所以从“CMake 正在读哪些脚本”的角度，确实有先后顺序。

### 2. 编译阶段有“依赖顺序”

真正开始构建时，顺序不是按文件名逐个执行，而是按 target 依赖关系决定。

例如这个项目里：

- 顶层先定义核心共享库 `jetson-inference`
- Python 绑定模块在 `python/bindings/CMakeLists.txt` 里会链接这个核心库

这就意味着：

- 核心库 `jetson-inference` 必须先准备好
- Python 扩展模块才能继续链接和生成

但这个先后关系由 CMake 和底层构建系统自动处理，不需要你手工去控制。

## 是不是必须一个一个来？

不是。

你不需要手动分别去执行：

- 根目录 `CMakeLists.txt`
- `python/CMakeLists.txt`
- `python/bindings/CMakeLists.txt`

这些文件不是给你单独逐个运行的，而是由顶层 CMake 自动串起来的。

正常情况下，你只需要在项目根目录做一次配置，再做一次构建，例如：

```bash
cmake -S . -B build
cmake --build build -j
```

然后 CMake 会自动完成下面这些事情：

1. 从顶层入口开始读取
2. 进入 `python/` 子目录
3. 再进入 `python/bindings/`
4. 根据 target 依赖自动安排编译顺序

所以正确理解应该是：

- `CMakeLists.txt` 之间有解析顺序
- target 之间有依赖顺序
- 但用户通常不需要手工一个一个执行这些 `CMakeLists.txt`

## 一句话总结这个问题

这几个 `CMakeLists.txt` 有顺序，但这个顺序主要是给 CMake 用的，不是要求你手工逐个运行。

## `python/bindings` 和 `c/` 里的文件有什么区别？

这两个目录看起来都会出现类似名字的文件，比如：

- `c/detectNet.cpp`
- `python/bindings/PyDetectNet.cpp`

但它们的职责完全不同。

## 先说结论

可以把它们理解成两层：

- `c/`：真正干活的核心推理实现层
- `python/bindings/`：把核心能力包装成 Python 可调用接口的适配层

也就是说：

- `c/` 解决“功能本身怎么实现”
- `python/bindings/` 解决“Python 怎么调用这些功能”

## `c/` 目录里的文件是核心实现

[`c/`](../c) 目录里的 `.cpp`、`.cu`、`.h` 文件，主要是项目真正的核心推理逻辑。

这里面实现的是：

- TensorRT 网络加载
- CUDA 预处理 / 后处理
- 检测、分类、分割、姿态估计、深度估计等核心算法
- 模型推理流程
- GPU 内存和 CUDA stream 管理
- 插件、追踪、实验功能等底层能力

例如：

- [`c/imageNet.h`](../c/imageNet.h) 定义了 `imageNet` 类
- [`c/detectNet.h`](../c/detectNet.h) 定义了 `detectNet` 类
- [`c/tensorNet.cpp`](../c/tensorNet.cpp) 负责很多 TensorRT / CUDA 相关基础逻辑
- [`c/segNet.cu`](../c/segNet.cu) 这类 `.cu` 文件里还有 CUDA kernel 或 GPU 后处理

这些文件本质上是“原生 C++/CUDA 库代码”。
即使完全不考虑 Python，它们仍然是整个项目的核心。

## `python/bindings/` 目录里的文件是 Python 包装层

[`python/bindings/`](./bindings) 里的文件不是重新实现一套推理逻辑，
而是把 `c/` 里的 C++ 类和函数暴露给 Python。

它们主要做的是：

- 包含 Python C API，例如 `Python.h`
- 解析 Python 传进来的参数
- 把 Python 对象转换成 C++ 参数
- 调用底层 `c/` 里的类和方法
- 再把返回结果包装成 Python 对象返回
- 把类注册到 Python 模块里，例如 `imageNet`、`detectNet`

例如：

- [`python/bindings/PyInference.h`](./bindings/PyInference.h) 引入了 `Python.h`
- [`python/bindings/PyInference.cpp`](./bindings/PyInference.cpp) 定义模块入口 `PyInit_jetson_inference_python`
- [`python/bindings/PyDetectNet.cpp`](./bindings/PyDetectNet.cpp) 负责把 `detectNet` 包装成 Python 里的 `detectNet`
- [`python/bindings/PyImageNet.cpp`](./bindings/PyImageNet.cpp) 负责把 `imageNet` 包装成 Python 里的 `imageNet`

所以这些文件更像“翻译层”或“胶水代码”。

## 一个最直观的对应关系

你可以把它们理解成这种一一对应关系：

- `c/imageNet.*` -> 真正实现图像分类
- `python/bindings/PyImageNet.*` -> 让 Python 能调用图像分类

- `c/detectNet.*` -> 真正实现目标检测
- `python/bindings/PyDetectNet.*` -> 让 Python 能调用目标检测

- `c/segNet.*` -> 真正实现语义分割
- `python/bindings/PySegNet.*` -> 让 Python 能调用语义分割

- `c/poseNet.*` -> 真正实现姿态估计
- `python/bindings/PyPoseNet.*` -> 让 Python 能调用姿态估计

## 两边代码风格为什么看起来差很多？

因为它们解决的问题不一样。

`c/` 里的代码更像：

- 类定义
- 推理流程
- CUDA / TensorRT 调用
- 算法和性能实现

而 `python/bindings/` 里的代码更像：

- `PyObject`
- `PyArg_ParseTupleAndKeywords`
- `PyModule_AddObject`
- `PyTypeObject`
- Python list / tuple / dict 的创建和转换

这说明：

- `c/` 更关注算法和底层执行
- `python/bindings/` 更关注语言边界转换

## 它们在构建产物里也不一样

`c/` 目录的源码主要会被编译进核心共享库：

- `jetson-inference`

而 `python/bindings/` 目录的源码会被编译成 Python 扩展模块：

- `jetson_inference_python`

然后 Python 再通过 `jetson_inference/__init__.py` 之类的包入口把它导入出来。

所以从产物角度也能区分：

- `c/` -> 核心本体
- `python/bindings/` -> Python 入口外壳

## 关系可以类比成什么？

如果把整个项目想成一家工厂：

- `c/` 是真正生产机器和流水线的地方
- `python/bindings/` 是给 Python 用户装上的操作面板和接口说明

操作面板本身不负责制造产品，但没有它，Python 用户就不方便直接操控底层机器。

## 一句话总结这个问题

`c/` 是核心功能实现，`python/bindings/` 是把这些核心功能暴露给 Python 的绑定适配层；前者是“能力本体”，后者是“Python 调用接口”。

## 这里为什么不用 `pybind11`？现在这个就是 `pybind11` 吗？

不是。

这个仓库当前这套绑定代码不是 `pybind11`，而是手写的 Python C API 绑定。

从代码上可以直接看出来：

- [`python/bindings/PyInference.h`](./bindings/PyInference.h) 里包含了 `Python.h`
- [`python/bindings/PyInference.cpp`](./bindings/PyInference.cpp) 里定义了模块入口 `PyInit_jetson_inference_python`
- 各个绑定文件里大量使用了：
  - `PyArg_ParseTupleAndKeywords`
  - `PyTypeObject`
  - `PyModule_AddObject`
  - `PyObject`

如果是 `pybind11`，你通常会看到这类写法：

- `#include <pybind11/pybind11.h>`
- `PYBIND11_MODULE(...)`
- `py::class_<...>`

这个仓库里没有走那条路线。

## 那为什么不直接用 `pybind11`？

从工程角度说，当然可以用 `pybind11` 重写一套绑定，但这不代表现有手写方案就是错的。

这个项目选择手写 Python C API，常见原因通常有这些：

- 项目历史比较早，最初就不是按 `pybind11` 体系写的
- 需要精细控制 Python 2 / Python 3 兼容逻辑
- 需要显式控制模块初始化、对象生命周期、错误处理
- 需要和自定义 CUDA 内存封装、capsule、底层 C 结构体紧密对接
- 已有代码已经稳定可用，迁移到 `pybind11` 成本不小

这个仓库里其实能看到明显的历史痕迹：

- [`python/bindings/PyInference.cpp`](./bindings/PyInference.cpp) 里同时保留了
  Python 3 的 `PyInit_jetson_inference_python`
- 也保留了旧的 Python 2 初始化逻辑 `initjetson_inference_python`

这类代码风格本身就很像“历史上一直是手写 CPython API”。

## 用了 `pybind11` 就不用自己写类型转换了吗？

不是“完全不用”，而是“很多常见转换由框架帮你自动做了”。

这是关键区别：

- 手写 Python C API：你自己显式写转换
- `pybind11`：很多基础类型转换由库帮你封装

例如普通场景下：

- Python `int` <-> C++ `int`
- Python `float` <-> C++ `float`
- Python `str` <-> `std::string`
- Python `list` <-> `std::vector`

这些在 `pybind11` 里通常更省事。

但是只要涉及下面这些内容，类型转换仍然绕不过去：

- 自定义 C++ 类
- 自定义结构体
- GPU 指针
- CUDA 图像内存
- 零拷贝共享内存
- Python 和 C++ 之间谁拥有对象生命周期
- 返回值要不要转成 Python tuple / list / class

所以，`pybind11` 不是“没有类型转换”，而是“帮你封装了一大部分常见类型转换”。

## 为什么这里面还有这么多类型转换？

因为 Python 和 C++ 本来就是两套不同的对象系统。

比如在这个仓库里，Python 传进来的很多图像不是普通 CPU 数组，而是 CUDA 图像 capsule。
绑定层必须把这个 Python 对象重新解释成底层可用的数据结构，然后再传给 `c/` 里的核心实现。

代码里能看到很多这种情况，例如：

- [`python/bindings/PyDetectNet.cpp`](./bindings/PyDetectNet.cpp) 会先解析 Python 参数
- 然后通过 `PyCUDA_GetImage(...)` 从 capsule 中取出底层图像指针
- 再调用底层 `detectNet` 能接受的参数格式

这一步本质上就是类型转换，而且是“跨语言边界的类型转换”。

## 为什么 `pybind11` 也不能完全替代这些转换？

因为像这个项目这种场景，不只是简单的 `int` 和 `string`。

它还涉及：

- GPU 内存地址
- 图像宽高和像素格式
- CUDA capsule
- Python 对象和 C++ 对象之间的生命周期同步
- 检测框、姿态点、深度图这类自定义结果对象

这些东西即使用 `pybind11`，你通常也还是要自己定义：

- 怎么从 Python 对象里取到底层句柄
- 怎么检查这个对象是不是合法的 GPU 图像
- 怎么把 C++ 结果转回 Python 里的类、tuple、list

也就是说，`pybind11` 可以减少样板代码，但不能让“边界适配”这件事消失。

## 那这里的“类型转换”具体在转什么？

大致有几类：

- Python 参数 -> C/C++ 基础类型
  例如 `int`、`float`、`char*`
- Python 对象 -> 底层 CUDA 图像 / 内存句柄
  例如 capsule -> `PyCudaImage*` 或底层图像指针
- C++ 推理结果 -> Python 对象
  例如检测框转成 Python `Detection` 对象
- C++ 容器 / 结构体 -> Python list / tuple / 属性对象

例如在 [`python/bindings/PyDetectNet.cpp`](./bindings/PyDetectNet.cpp) 里：

- `PyArg_ParseTupleAndKeywords(...)` 负责把 Python 调用参数读出来
- `PyObject_New(...)` 用来创建 Python 侧的检测结果对象
- `PyList_New(...)` / `PyTuple_Pack(...)` 用来把 C++ 结果组装回 Python

这就是为什么你会看到很多“看起来像手工搬运数据”的代码。

## 一句话总结这个问题

这套代码不是 `pybind11`，而是手写的 Python C API 绑定；即使用 `pybind11`，跨 Python/C++/CUDA 边界的类型转换也不会消失，只是很多基础转换会被库自动封装掉。
