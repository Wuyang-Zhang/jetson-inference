# Python 包目录说明

这个目录保存的是最终会被安装到 Python `site-packages` / `dist-packages`
中的包文件。

## 为什么会有 `python/python`？

外层 [`python`](../) 目录是这个仓库里所有 Python 相关内容的工作区，
里面包括：

- `bindings/`：C++ 到 Python 的绑定构建脚本和扩展模块构建逻辑
- `examples/`：Python 示例程序
- `www/`：Web / Demo 相关代码
- `python/`：会被复制到最终 Python 安装目录中的纯 Python 包目录

也就是说：

- 外层 `python/` = 仓库里所有 Python 相关源码的总目录
- 内层 `python/python/` = 真正作为 Python 包安装出去的目录根

这和 [`python/bindings/CMakeLists.txt`](../bindings/CMakeLists.txt) 里的安装逻辑一致。
构建时会把下面这些包目录安装到 Python 的包路径中：

- `../python/Jetson`
- `../python/jetson`
- `../python/jetson_inference`

## `jetson`、`jetson_inference`、`Jetson/Inference` 有什么区别？

`jetson_inference` 是现在正式使用的包名，也是推荐的导入方式。
新代码应该写成：

```python
from jetson_inference import detectNet, imageNet
```

它的初始化文件会导入底层编译出的扩展模块 `jetson_inference_python`，
并把实际的 Python API 暴露出来。

另外两个名字主要是为了兼容旧代码：

- `jetson.inference`
- `Jetson.Inference`

它们本质上仍然指向同一个底层扩展模块，但已经属于兼容层。
旧路径的初始化文件里还会打印警告，提示用户改用
`import jetson_inference`。

所以实际可以这样理解：

- `jetson_inference`：当前正式包名
- `jetson` / `Jetson`：为了兼容旧导入方式而保留的命名空间

## 为什么还会有大小写不同的 `Jetson` 和 `jetson`？

这是历史兼容的一部分。

在 Linux 上，文件路径大小写敏感，所以 `Jetson` 和 `jetson`
可以被当成两个不同的包路径。

在 Windows 上，文件系统通常对大小写不敏感，所以你浏览目录时会觉得
这两个名字看起来几乎重复，容易让结构显得更乱。

但它们的存在目的其实很简单：保留老代码的导入方式，同时把新代码统一到
`jetson_inference`。
