<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">

# 从零配置到可运行项目

这份文档只解决一个目标：

把 `jetson-inference` 从源码配置到“可以真正跑起来一个示例”。

它不是源码结构分析文档，也不是训练文档，而是偏实操的安装/构建/验证指南。

## 适用范围

这份文档优先面向下面这种环境：

- NVIDIA Jetson 设备
- 已经刷好 JetPack
- 系统是 Ubuntu / Linux
- 希望从源码构建并运行 C++ / Python 示例

最推荐的环境是：

- Jetson Nano / Xavier / Orin
- JetPack 已正常安装
- 在设备本机终端里操作

## 先说一个很重要的前提

这个项目的主流程不是给 Windows 原生环境准备的。

原因很直接：

- 顶层构建脚本会调用 `apt-get`
- 会用到 `lsb_release`
- 会跑 `sh ../CMakePreBuild.sh`
- 会依赖 JetPack / CUDA / TensorRT / GStreamer 这些 Linux/Jetson 体系

所以最稳妥的做法是：

- 在 Jetson 的 Ubuntu 系统里构建

如果你现在是在 Windows 里浏览源码，没有问题；
但真正要把项目配到能运行，建议切到 Jetson 本机 Ubuntu 环境去做。

## 你最终会完成什么

完成本文后，你应该能做到：

1. 成功编译 `jetson-inference`
2. 安装 C++ 库和 Python 包
3. 下载预训练模型
4. 运行至少一个 C++ 示例
5. 运行至少一个 Python 示例

## 推荐的最短成功路径

如果你只想先跑通，推荐走这条路径：

1. 确认 Jetson 已安装 JetPack
2. 安装最基本构建工具
3. 拉取仓库和子模块
4. 配置 CMake
5. 编译并安装
6. 下载模型
7. 运行 `imagenet` 示例验证

## 第 1 步：确认系统前提

先确认你是在 Jetson 的 Ubuntu 系统中。

至少要具备：

- JetPack
- CUDA
- TensorRT
- 网络可用
- `sudo` 权限

如果 JetPack 没装好，先不要继续构建这个仓库。

## 第 2 步：安装基础工具

在 Jetson 终端里执行：

```bash
sudo apt-get update
sudo apt-get install -y git cmake libpython3-dev python3-numpy
```

这一步是最小基础依赖。

注意：

- Python 绑定构建依赖 `libpython3-dev`
- Python 示例和绑定常用 `python3-numpy`

## 第 3 步：拉取仓库

推荐直接带子模块一起拉：

```bash
git clone --recursive https://github.com/dusty-nv/jetson-inference
cd jetson-inference
```

克隆一个仓库 + 把它依赖的其他仓库也一起下载下来

有些项目不是单一仓库，而是：

```
jetson-inference
├── 主代码
├── jetson-utils（另一个仓库）
├── third-party libs
```

这些“嵌套仓库”就是：submodule（子模块）

如果你已经克隆过，但没带子模块，就执行：

```bash
git submodule update --init
```

这个步骤不能省。
顶层 [CMakeLists.txt](../CMakeLists.txt) 会检查 `utils/.git`，缺子模块会直接报错退出。

不加 `--recursive`

```
jetson-inference/
├── CMakeLists.txt
├── python/
├── jetson-utils/   空目录（没内容！）
```

加了 `--recursive`

```
jetson-inference/
├── CMakeLists.txt
├── python/
├── jetson-utils/   已下载完整代码
```

这个项目结构是：NVIDIA官方示例项目，依赖：jetson-utils（图像处理、CUDA工具），其他第三方组件；

底层原理：Git 里面有一个文件 `.gitmodules`，内容类似：

```
[submodule "jetson-utils"]
    path = jetson-utils
    url = https://github.com/dusty-nv/jetson-utils
```

意思是：“这个目录其实是另一个 Git 仓库”

`--recursive` 等价于：

```bash
git clone xxx
cd repo
git submodule update --init --recursive
```

本质：自动把所有子仓库 clone 下来

jetson-inference：一个文件夹 + 多个子工程（Git仓库）,多仓库工程结构

```
git submodule update --init --recursive
```

等价于：

```
# 初始化子模块（让 Git 知道这些子仓库）
git submodule init

# 下载子模块代码
git submodule update

# 递归处理子模块的子模块
git submodule update --recursive
```

## 第 4 步：创建构建目录

```bash
mkdir build
cd build
```

不要在源码根目录直接 `make`，这个项目按 out-of-source build 来组织更稳妥。

## 第 5 步：运行 CMake 配置

最常见做法：

```bash
cmake ../
```

如果你想避免交互式提示，更建议用：

```bash
cmake -DBUILD_INTERACTIVE=NO ../
```

### 这里会发生什么

第一次配置时，顶层 [CMakeLists.txt](../CMakeLists.txt) 会触发
[CMakePreBuild.sh](../CMakePreBuild.sh)。

这个脚本会尝试安装一些额外依赖，例如：

- GStreamer 相关包
- Qt 基础包
- Python 打包相关工具
- Cython

并且在非容器环境下，它还会调用 `install-pytorch.sh`。

所以如果你看到 CMake 阶段在装包，这是正常的。

### `BUILD_INTERACTIVE=NO` 有什么用

这个参数主要用来减少交互。

对当前仓库来说，它最直接的作用是：

- 让 PyTorch 安装脚本跳过交互式安装

如果你当前目标只是“先把推理示例跑起来”，完全可以先用它。

## 第 6 步：编译

```bash
make -j$(nproc)
```

如果你想保守一点，也可以直接：

```bash
make
```

编译完成后，主要产物会出现在类似这样的目录中：

```text
build/aarch64/bin
build/aarch64/lib
build/aarch64/include
```

其中：

- `bin/` 放示例程序
- `lib/` 放库文件
- `include/` 放导出的头文件

## 第 7 步：安装到系统

```bash
sudo make install
sudo ldconfig
```

这一步非常重要。

它会把项目安装到系统路径，典型包括：

- `/usr/local/lib`
- `/usr/local/include`
- Python 的 `dist-packages`

如果你跳过这一步，经常会出现：

- Python `import jetson_inference` 失败
- 系统找不到共享库

## 第 8 步：下载模型

当前仓库里，模型下载不要假设一定会在构建时自动完成。

为了最稳妥，建议手动执行一次：

```bash
cd ../tools
./download-models.sh
cd ../build
```

如果你不下载模型，很多示例虽然编译通过，但运行时会缺少网络模型文件。

## 第 9 步：先做最小验证

建议先验证两件事：

1. Python 包是否安装成功
2. C++ 示例是否真的能跑

### 9.1 验证 Python 包

执行：

```bash
python3 -c "import jetson_inference, jetson_utils; print('python bindings ok')"
```

如果这条命令成功，说明：

- Python 包已经安装进系统
- 动态链接至少基本正常

### 9.2 验证 C++ 示例存在

执行：

```bash
cd aarch64/bin
ls
```

你应该能看到类似：

- `imagenet`
- `detectnet`
- `segnet`
- `posenet`

## 第 10 步：运行第一个 C++ 示例

推荐先跑最简单的 `imagenet`。

在 `build/aarch64/bin` 目录下执行：

```bash
./imagenet images/orange_0.jpg output.jpg
```

这条命令的意思是：

- 输入一张测试图片
- 输出一张带结果的图片

如果你的环境有显示器，也可以尝试视频/摄像头输入，但第一步不建议一上来就走摄像头链路。

先把静态图片跑通，排错成本最低。

## 第 11 步：运行第一个 Python 示例

回到仓库根目录后，可以执行：

```bash
python3 python/examples/imagenet.py images/orange_0.jpg output_py.jpg
```

如果这条命令运行成功，说明：

- Python 绑定可用
- Python 示例能调用到底层 TensorRT / CUDA 路径

## 一套可以直接照抄的最短命令

如果你想直接复制执行，下面这套是最接近“从零到能跑”的最短路径：

```bash
sudo apt-get update
sudo apt-get install -y git cmake libpython3-dev python3-numpy

git clone --recursive https://github.com/dusty-nv/jetson-inference
cd jetson-inference

mkdir build
cd build

cmake -DBUILD_INTERACTIVE=NO ../
make -j$(nproc)
sudo make install
sudo ldconfig

cd ../tools
./download-models.sh

cd ../build/aarch64/bin
./imagenet images/orange_0.jpg output.jpg

cd ../../..
python3 python/examples/imagenet.py images/orange_0.jpg output_py.jpg
```

## 如果你只想先做推理，不想装 PyTorch

可以。

这个项目的“运行推理示例”和“在 Jetson 上训练模型”不是一回事。

如果你当前只想：

- 编译项目
- 跑 `imagenet` / `detectnet` / `segnet`

那你可以先不折腾 PyTorch 训练环境。

也就是说：

- 推理示例是第一优先级
- PyTorch 训练是后续可选项

## 常见问题

### 1. `cmake` 一开始就报缺少 submodule

说明你没把子模块准备好。

执行：

```bash
git submodule update --init
```

如果仓库是新克隆的，更推荐一开始就用：

```bash
git clone --recursive https://github.com/dusty-nv/jetson-inference
```

### 2. `import jetson_inference` 失败

优先检查你是不是漏了：

```bash
sudo make install
sudo ldconfig
```

其次检查是不是 Python 版本环境不一致。

### 3. 编译过了，但运行示例时报模型不存在

通常就是还没下载模型。

执行：

```bash
cd tools
./download-models.sh
```

### 4. 在 Windows PowerShell 里直接构建失败

这不是推荐路径。

这个项目主流程依赖 Jetson / Ubuntu / apt / CUDA / TensorRT / GStreamer。
请切回 Jetson 的 Ubuntu 环境构建。

### 5. 一定要手工一个一个执行多个 `CMakeLists.txt` 吗？

不用。

你只需要从项目根目录执行一次：

```bash
cmake -S . -B build
cmake --build build -j
```

或者进入 `build/` 后：

```bash
cmake ../
make -j$(nproc)
```

CMake 会自动按顺序进入：

- 顶层 `CMakeLists.txt`
- `python/CMakeLists.txt`
- `python/bindings/CMakeLists.txt`

你不需要逐个手动执行它们。

## 推荐你接下来怎么做

如果你的目标是“先确认项目活了”，建议按这个顺序：

1. 先跑 C++ `imagenet`
2. 再跑 Python `imagenet.py`
3. 再切到 `detectnet`
4. 最后再考虑摄像头、RTSP、训练、WebRTC

这样排错路径最短。

## 一句话总结

最稳妥的配置方式是在 Jetson 的 Ubuntu 系统里，从源码根目录完成：

- 安装基础依赖
- 拉取子模块
- `cmake`
- `make`
- `sudo make install`
- 手动下载模型
- 先用 `imagenet` 做最小运行验证
