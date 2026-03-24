<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">

# x86 Ubuntu 从零配置到可运行

这份文档专门写给：

- `x86_64 Ubuntu`
- 有 NVIDIA 独立 GPU
- 想把 `jetson-inference` 配到“可以跑起来”

先说结论：

- 在 `x86 Ubuntu` 上，这个项目不是主线使用场景
- 但仓库明确考虑了 `x86_64`
- 真正最稳的做法是走 **Docker**
- 原生本机构建也可以尝试，但坑会比 Jetson 多，尤其是 Python 安装路径

## 这条路线适不适合你

适合：

- 你有 NVIDIA 独显
- 你已经在 Ubuntu 上装好了 NVIDIA 驱动
- 你希望在 PC 上跑示例、做开发、做训练
- 你能接受它和 Jetson 路线并不完全一样

不太适合：

- 没有 NVIDIA GPU
- 想完全照搬 Jetson 文档一步不改
- 想让所有 Jetson 专属硬件链路都完全等价

## 这条路线和 Jetson 路线的主要差别

这个项目主体文档是围绕 Jetson / `aarch64` 写的。

但仓库里确实有 `x86_64` 分支逻辑，例如：

- [`CMakePreBuild.sh`](../CMakePreBuild.sh) 会单独判断 `x86_64`
- [`python/bindings/CMakeLists.txt`](../python/bindings/CMakeLists.txt) 在 `x86_64` 下把 Python 包安装到 `/opt/conda/.../site-packages`
- [`docker/run.sh`](../docker/run.sh) 对 `x86_64` 有专门的 `--gpus all` 路线
- [`docs/aux-docker-CN.md`](./aux-docker-CN.md) 也明确写了容器可运行在 `x86_64` + NVIDIA GPU 上

所以正确理解是：

- `x86 Ubuntu` 不是这个仓库的主叙事路线
- 但它不是完全不支持
- 而且仓库作者明显更偏向你在 `x86` 上走 Docker

## 推荐路线

推荐顺序如下：

1. Docker 路线
2. 原生本机构建路线

如果你只是想尽快跑起来，优先用 Docker。

## 路线 A：Docker 方案，推荐

这是 `x86 Ubuntu` 下最稳妥的路线。

### 你需要先准备什么

至少要有：

- Ubuntu
- NVIDIA 驱动
- Docker
- NVIDIA Container Toolkit

如果没有 NVIDIA Container Toolkit，容器拿不到 GPU，这条路线基本就跑不起来。

## A1. 安装 Docker 和 NVIDIA Container Toolkit

这一步仓库没有完整替你做，需要你自己先准备好。

关键目标是做到：

- `docker` 可用
- 容器可使用 GPU

完成后你至少应能在宿主机确认：

```bash
docker --version
```

## A2. 拉取仓库

```bash
git clone --recursive https://github.com/dusty-nv/jetson-inference
cd jetson-inference
```

如果你已经克隆过但没拉子模块：

```bash
git submodule update --init
```

## A3. 构建 x86 容器

仓库已经提供了脚本：

```bash
docker/build.sh
```

这个脚本在 `x86_64` 下会走：

- [`docker/build.sh`](../docker/build.sh)
- 基础镜像为 `nvcr.io/nvidia/pytorch:...-py3`

这说明作者本来就把 x86 路线和 NVIDIA PyTorch 容器体系绑在一起考虑。

## A4. 运行容器

```bash
docker/run.sh
```

在 `x86_64` 下，这个脚本会自动用：

- `docker run --gpus all`
- `--network=host`
- 较大的共享内存和 ulimit 设置

也就是仓库已经把 x86 的 GPU 容器启动参数准备好了，见
[`docker/run.sh`](../docker/run.sh)。

## A5. 在容器中验证

进入容器后，先做最小验证。

### 验证 Python 绑定

```bash
python3 -c "import jetson_inference, jetson_utils; print('python bindings ok')"
```

### 验证 C++ 示例

```bash
cd /jetson-inference/build/$(uname -m)/bin
ls
```

你应该能看到类似：

- `imagenet`
- `detectnet`
- `segnet`

### 跑一个最小示例

```bash
./imagenet images/orange_0.jpg output.jpg
```

或者 Python：

```bash
cd /jetson-inference
python3 python/examples/imagenet.py images/orange_0.jpg output_py.jpg
```

## Docker 路线的优点

- 最接近仓库作者在 `x86_64` 下预期的使用方式
- Python 包路径、依赖、容器环境更统一
- 少踩本机 Python / TensorRT / GStreamer 混装问题

## Docker 路线的缺点

- 前提是你已经把 Docker GPU 环境配好
- 摄像头 / 显示器 / V4L2 之类设备映射仍可能需要额外处理

## 路线 B：原生本机构建，进阶

这条路线能试，但你要清楚：

- 它比 Jetson 原生构建更容易踩坑
- 最大坑之一就是 Python 绑定安装路径

如果你只是想快速跑起来，优先还是 Docker。

## B1. 你需要的前提

至少准备好：

- Ubuntu
- NVIDIA 驱动
- CUDA
- TensorRT
- GStreamer 开发包
- Python 开发环境

你不能只装一个 `cmake` 就指望整个项目直接跑。

## B2. 安装基础依赖

先装最基本的工具：

```bash
sudo apt-get update
sudo apt-get install -y \
    git cmake \
    libpython3-dev python3-numpy \
    python3-pip python3-packaging \
    libglew-dev glew-utils \
    gstreamer1.0-libav \
    gstreamer1.0-nice \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    libgstreamer-plugins-good1.0-dev \
    libgstreamer-plugins-bad1.0-dev \
    libgstrtspserver-1.0-dev \
    libglib2.0-dev \
    libsoup2.4-dev \
    libjson-glib-dev \
    qtbase5-dev \
    avahi-utils
```

再装 Cython：

```bash
pip3 install --upgrade Cython
```

### 为什么这里的包这么多

因为 [`CMakePreBuild.sh`](../CMakePreBuild.sh) 本来就会装这些。
我这里是把它显式写出来，方便你提前知道需要什么。

## B3. 拉取仓库

```bash
git clone --recursive https://github.com/dusty-nv/jetson-inference
cd jetson-inference
```

如果没带子模块：

```bash
git submodule update --init
```

## B4. 创建构建目录

```bash
mkdir build
cd build
```

## B5. 运行 CMake

原生路线推荐先这样：

```bash
cmake -DBUILD_INTERACTIVE=NO ../
```

如果你想让脚本自己交互式装 PyTorch，可以去掉 `-DBUILD_INTERACTIVE=NO`，
但对“先跑通推理”这件事没有必要。

## B6. 编译和安装

```bash
make -j$(nproc)
sudo make install
sudo ldconfig
```

## B7. 下载模型

```bash
cd ../tools
./download-models.sh
cd ../build
```

## B8. 先验证 C++ 示例

在 `build/$(uname -m)/bin` 目录下执行：

```bash
./imagenet images/orange_0.jpg output.jpg
```

如果这一步成功，说明：

- 核心库
- CUDA / TensorRT
- 至少一部分运行链路

已经工作了。

## B9. 原生路线最大的坑：Python 包安装位置

这是你必须注意的点。

[`python/bindings/CMakeLists.txt`](../python/bindings/CMakeLists.txt) 在 `x86_64` 下默认写的是：

- Python 安装目录 -> `/opt/conda/lib/pythonX.Y/site-packages`

这意味着：

- 如果你在普通系统 Python 里运行
- 而不是在 `/opt/conda` 对应的 Python 环境里运行

那么即使你已经 `sudo make install`，也可能直接出现：

```python
import jetson_inference
```

失败。

## B10. 原生路线怎么处理 Python 问题

### 方案 1：用 conda 环境，最贴近仓库预期

如果你的 Python 本来就来自 `/opt/conda`，那这条路径最顺。

### 方案 2：临时加 `PYTHONPATH`

如果绑定确实被装到了 `/opt/conda/lib/pythonX.Y/site-packages`，你可以临时：

```bash
export PYTHONPATH=/opt/conda/lib/python3.X/site-packages:$PYTHONPATH
python3 -c "import jetson_inference, jetson_utils; print('python bindings ok')"
```

这里的 `3.X` 需要换成实际版本。

### 方案 3：自己改安装路径，最彻底

如果你明确要走原生系统 Python，而不是 conda，
最彻底的做法是改
[`python/bindings/CMakeLists.txt`](../python/bindings/CMakeLists.txt)
里 `x86_64` 的 `PYTHON_BINDING_INSTALL_DIR`。

但这已经属于“修改项目构建逻辑”了，不是零改动方案。

## B11. Python 最小验证

如果你已经解决了 Python 路径问题，再执行：

```bash
python3 -c "import jetson_inference, jetson_utils; print('python bindings ok')"
```

然后跑 Python 示例：

```bash
cd ..
python3 python/examples/imagenet.py images/orange_0.jpg output_py.jpg
```

## x86 Ubuntu 推荐你怎么选

### 如果你的目标是尽快跑通

选 Docker：

1. 配好 Docker + NVIDIA Container Toolkit
2. `docker/build.sh`
3. `docker/run.sh`
4. 在容器里跑 `imagenet`

### 如果你的目标是本机深度开发

可以试原生路线，但你要接受：

- Python 安装路径需要你自己处理
- 某些视频硬件路径不一定和 Jetson 一样
- 某些 Jetson 专属能力不会等价

## 一套最实用的 x86 命令建议

### Docker 路线

```bash
git clone --recursive https://github.com/dusty-nv/jetson-inference
cd jetson-inference

docker/build.sh
docker/run.sh
```

进入容器后：

```bash
python3 -c "import jetson_inference, jetson_utils; print('python bindings ok')"
cd /jetson-inference/build/$(uname -m)/bin
./imagenet images/orange_0.jpg output.jpg
```

### 原生路线

```bash
sudo apt-get update
sudo apt-get install -y \
    git cmake \
    libpython3-dev python3-numpy \
    python3-pip python3-packaging \
    libglew-dev glew-utils \
    gstreamer1.0-libav \
    gstreamer1.0-nice \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    libgstreamer-plugins-good1.0-dev \
    libgstreamer-plugins-bad1.0-dev \
    libgstrtspserver-1.0-dev \
    libglib2.0-dev \
    libsoup2.4-dev \
    libjson-glib-dev \
    qtbase5-dev \
    avahi-utils

pip3 install --upgrade Cython

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

cd ../build/$(uname -m)/bin
./imagenet images/orange_0.jpg output.jpg
```

## 一句话总结

`x86 Ubuntu` 能做，但这个仓库在 `x86` 下最稳的方式是 **Docker**；如果坚持原生构建，最大难点通常不是编译本身，而是 **CUDA/TensorRT 环境完整性** 和 **Python 绑定安装路径**。
