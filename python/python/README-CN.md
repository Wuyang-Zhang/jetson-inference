# Python 包目录说明

这个目录保存的是最终会安装到 Python `site-packages` / `dist-packages` 里的包文件。

## 为什么会有 `python/python`

外层 [`python`](g:/jetson-inference/python) 是这个仓库里所有 Python 相关内容的工作目录，里面包含：

- `bindings/`
  Python 绑定的 C++ 构建脚本和扩展模块构建逻辑

- `examples/`
  Python 示例程序

- `www/`
  Web / Demo 相关代码

- `python/`
  真正要被安装到 Python 包路径里的纯 Python 包目录

所以可以把它理解成：

- 外层 `python/` = 这个仓库里所有 Python 相关源码的总目录
- 内层 `python/python/` = 最终安装到 Python 里的包根目录

这和 [`python/bindings/CMakeLists.txt`](g:/jetson-inference/python/bindings/CMakeLists.txt) 的安装逻辑是一致的。它会把这些目录安装到 Python 包路径中：

- `../python/Jetson`
- `../python/jetson`
- `../python/jetson_inference`

## `jetson`、`jetson_inference`、`Jetson/Inference` 有什么区别

`jetson_inference` 是当前正式包名，也是推荐使用的导入方式。

新代码应该使用：

```python
from jetson_inference import detectNet, imageNet
```

另外两个名称是为了兼容旧代码保留下来的旧路径：

- `jetson.inference`
- `Jetson.Inference`

实际关系是：

- `jetson_inference`
  当前正式公开包名

- `jetson` / `Jetson`
  历史兼容命名空间

所以平时写代码时，直接统一用 `jetson_inference` 就行。

## 为什么会同时有 `Jetson` 和 `jetson`

这是为了兼容旧项目。

在 Linux 上，路径大小写是区分的，所以：

- `Jetson`
- `jetson`

会被当成两个不同目录。

在 Windows 上通常不区分大小写，所以你浏览仓库时会感觉它们几乎重复。

但这个布局本质上不是为了制造两套 API，而是为了兼容老的导入方式，同时把新代码统一引导到 `jetson_inference`。

## `python/python/jetson_inference` 最后形成的是什么

[`python/python/jetson_inference`](g:/jetson-inference/python/python/jetson_inference) 本身最后形成的是一个 **Python 包入口壳**，不是核心实现本体。

这个目录里真正的文件只有：

- [`python/python/jetson_inference/__init__.py`](g:/jetson-inference/python/python/jetson_inference/__init__.py)

这个 `__init__.py` 做的事情很简单：

1. 先导入 `jetson_utils`
2. 再从 `jetson_inference_python` 导入真正的绑定对象

也就是说：

- `jetson_inference`
  是给 Python 用户导入用的正式包名

- `jetson_inference_python`
  是真正的底层编译扩展模块

- `__init__.py`
  只是把这个扩展模块包装成更自然的 Python 包入口

所以你平时写：

```python
import jetson_inference
```

表面上导入的是 `jetson_inference` 包，  
但实际功能最终来自 `jetson_inference_python` 这个编译模块。

## `jetson_inference_python` 模块里面包含什么

`jetson_inference_python` 不是普通的 Python 脚本模块，而是一个 **C++ 编译扩展模块**。

模块入口在：

- [`python/bindings/PyInference.cpp`](g:/jetson-inference/python/bindings/PyInference.cpp)

它本身几乎没有普通顶层函数，真正做的事情是注册一组 Python 类型。

主要包含这些网络类：

- `tensorNet`
- `imageNet`
- `depthNet`
- `detectNet`
- `poseNet`
- `segNet`
- `actionNet`
- `backgroundNet`

也就是说，`jetson_inference_python` 本质上是：

- 把底层 C++ 网络类注册成 Python 里的类
- 再让 `jetson_inference/__init__.py` 把它们导出给用户

例如：

- [`python/bindings/PyImageNet.cpp`](g:/jetson-inference/python/bindings/PyImageNet.cpp)
  提供 `imageNet`

- [`python/bindings/PyDetectNet.cpp`](g:/jetson-inference/python/bindings/PyDetectNet.cpp)
  提供 `detectNet`

- [`python/bindings/PySegNet.cpp`](g:/jetson-inference/python/bindings/PySegNet.cpp)
  提供 `segNet`

除了网络类本身，它里面还包含一些结果对象类型。

例如：

- `detectNet.Detection`
- `poseNet.ObjectPose`
- `poseNet.ObjectPose.Keypoint`

## Python 运行时到底依赖哪些 `.so`

这个问题最容易混淆，所以单独总结。

### 1. `libjetson-inference.so`

这是底层核心共享库。

它里面是真正的 C++ 推理实现，也就是：

- `c/imageNet.cpp`
- `c/detectNet.cpp`
- `c/segNet.cpp`
- `c/actionNet.cpp`
- `c/tensorNet.cpp`

这些核心网络能力最终都会被编到这个底层库里。

所以可以把它理解成：

```text
libjetson-inference.so = 功能本体
```

### 2. `jetson_inference_python.so`

这是 Python 扩展模块。

它不会重新实现一套网络，而是把底层 `libjetson-inference.so` 里的能力包装成 Python 可以调用的接口。

所以可以把它理解成：

```text
jetson_inference_python.so = Python 适配层 + 底层库调用入口
```

### 3. `jetson_inference/__init__.py`

这是 Python 包壳。

它的作用不是做推理，而是让你可以自然地写：

```python
import jetson_inference
```

而不需要直接去导入底层扩展模块名字。

## 它们三者之间的关系

最直观可以画成这样：

```text
import jetson_inference
        ↓
python/python/jetson_inference/__init__.py
        ↓
jetson_inference_python.so
        ↓
libjetson-inference.so
        ↓
c/imageNet.cpp / c/detectNet.cpp / c/segNet.cpp / ...
```

所以你前面问的那句话，可以总结成：

- `libjetson-inference.so`
  是底层核心库

- `jetson_inference_python.so`
  是让 Python 能调用这套底层能力的扩展模块

- `jetson_inference`
  是最终给 Python 用户使用的正式包名入口

## 最后一句总结

如果你只记一句话，就记这个：

**`jetson_inference` 是 Python 包名，`jetson_inference_python` 是 Python 扩展模块，`libjetson-inference.so` 是底层核心实现。**
