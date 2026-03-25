## 函数调用

**在 CUDA + C++ 项目里这是非常常见的做法** 。但这里有几个关键点你必须搞清楚，否则很容易踩坑。

 **`.cpp` 可以调用 `.cu`，但前提是：通过函数接口调用，而不是直接“include调用”。**

本质原理：`.cu` 文件其实是： 被 **NVCC 编译器**处理的 C++ 文件（包含 GPU 代码）；

而 `.cpp`： 被 **g++ / clang++** 编译

所以问题本质是： **不同编译器如何协同工作 + 如何链接**

正确调用方式（标准工程做法）

Step 1：在 `.cu` 中写函数（提供接口）

```cpp
// kernel.cu
#include <cuda_runtime.h>
#include <stdio.h>

__global__ void myKernel() {
    printf("Hello from GPU!\n");
}

// 提供给 cpp 调用的接口（关键！）
extern "C" void launchKernel() {
    myKernel<<<1,1>>>();
    cudaDeviceSynchronize();
}
```

为什么要 `extern "C"`？避免 C++  **名字重整（name mangling）** ，否则 `.cpp` 找不到符号。

Step 2：在 `.cpp` 中声明并调用

```cpp
// main.cpp
#include <iostream>

// 声明接口（不用 include .cu）
extern "C" void launchKernel();

int main() {
    launchKernel();
    return 0;
}
```

Step 3：编译方式（核心）

```bash
nvcc -c kernel.cu -o kernel.o
g++ main.cpp kernel.o -lcudart -o app
```

或者直接：

```bash
nvcc main.cpp kernel.cu -o app
```

常见错误

1. 在 `.cpp` 里直接 `#include "xxx.cu"`

 **错误！非常不推荐**

原因：

* `.cu` 需要 nvcc 编译
* `.cpp` 用 g++ 编译 → 不认识 CUDA 语法（如 `__global__`）

2. `.cpp` 直接调用 kernel

```cpp
myKernel<<<1,1>>>();  // ❌ 错
```

 kernel launch 语法只在 `.cu` 里合法

3. 没加 `extern "C"`

结果：

```
undefined reference to `launchKernel()`
```

 因为符号名被 C++ 改写了

你可以把它理解为：

```
.cpp（CPU逻辑）
    ↓ 调用
接口函数（普通C函数）
    ↓
.cu（GPU执行）
    ↓
kernel<<<>>>（GPU执行）
```

CMake里怎么写

```cmake
enable_language(CUDA)

add_executable(app main.cpp kernel.cu)

target_link_libraries(app cuda)
```

关键点：

* `.cu` 会自动用 nvcc
* `.cpp` 用 g++
* CMake 帮你做混合编译

你现在这个问题，本质已经触及到：

## “异构编程模型”

* CPU代码（.cpp）
* GPU代码（.cu）
* 两者通过接口解耦

最后给你一个判断标准

 如果你问：“这个函数里有没有 `<<< >>>` 或 `__global__`？”

✔ 有 → 必须在 `.cu`
✔ 没有 → 可以在 `.cpp`

---

## [segNet](g:/jetson-inference/c/segNet.h)

它们是同一个模块的三层分工，不是三个互相独立的文件。

- [c/segNet.h](g:/jetson-inference/c/segNet.h)
  作用是“对外说明书”。
  这里定义了 `segNet` 类本身、公开接口、枚举和成员函数声明，比如 `Process()`、`Mask()`、`Overlay()`、`FilterMode`、`VisualizationFlags`。别的文件要用 `segNet`，先看这个头文件。
- [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp)
  作用是“主逻辑实现”。
  这里实现了 `segNet` 的大部分 C++ 流程，比如：

  - `Create()` 怎么加载模型
  - `Process()` 怎么做输入预处理和推理
  - `classify()` 怎么整理分割结果
  - `Mask()` / `Overlay()` 什么时候调用底层 CUDA 可视化
  - 类别颜色、标签、alpha 这些管理逻辑
- [c/segNet.cu](g:/jetson-inference/c/segNet.cu)
  作用是“GPU 像素级计算实现”。
  这里放的是 CUDA kernel，比如 [gpuSegOverlay](g:/jetson-inference/c/segNet.cu#L30) 和 [cudaSegOverlay](g:/jetson-inference/c/segNet.cu#L159)。这部分专门负责把分割结果按像素生成 mask/overlay，因为这类操作数据量大，放 GPU 上做更合适。

它们的关系可以直接理解成：

```text
segNet.h
= 定义接口

segNet.cpp
= 用这些接口组织分割网络流程

segNet.cu
= 给 segNet.cpp 提供 GPU 加速的底层图像处理
```

实际调用链大概是：

```text
Python/C++ 示例
  ↓
segNet::Process()          在 segNet.cpp
  ↓
TensorRT 推理 + 生成分割类别结果

segNet::Overlay()/Mask()   在 segNet.cpp
  ↓
cudaSegOverlay()           在 segNet.cu
  ↓
gpuSegOverlay<<<...>>>()   在 segNet.cu
```

为什么要拆成这三个文件：

- `.h` 让接口和实现分开，外部更容易调用
- `.cpp` 放普通 C++ 逻辑，结构清楚
- `.cu` 放 CUDA kernel，因为它要用 `__global__` 这类只能由 NVCC 编译的代码

**`segNet.h` 是接口，`segNet.cpp` 是分割流程控制，`segNet.cu` 是分割结果可视化的 GPU 加速实现。**

### `segNet::Process()`

 `segNet::Process()` 这条线其实很清楚。它做的不是“直接把结果画出来”，而是先把一帧图像变成**分割结果的内部表示**，后面的 `Overlay()` 和 `Mask()` 再拿这个结果去可视化。

先看接口定义：[c/segNet.h](g:/jetson-inference/c/segNet.h#L175)、[c/segNet.h](g:/jetson-inference/c/segNet.h#L185)。这里已经写得很明确：`Process()` 负责 inferencing，结果随后可以用 `Overlay()` / `Mask()` 显示。

`Process()` 在 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L500) 里的执行顺序基本是这样：

1. 参数检查它先检查输入图像指针、宽高是不是合法，再检查格式是不是 RGB/RGBA 这类支持的格式。见 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L502) 到 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L517)。这么做是为了尽早拦住错误输入，不让后面的 CUDA/TensorRT 在错误数据上继续跑。
2. 预处理这一步把原始图像变成网络真正要吃的输入 tensor。见 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L520) 到 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L548)。它会按模型类型走两条路：

   - ONNX 模型：走 `cudaTensorNormMeanRGB()`，也就是 resize + RGB 排列 + 归一化/均值方差处理，见 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L522) 到 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L534)
   - 其他模型：走 `cudaTensorMeanBGR()`，见 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L536) 到 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L545)

   这里就是你前面关心的“输入输出都不一样”会卡住的地方之一。模型不同时，预处理假设也可能不同。
3. 跑 TensorRT 推理预处理后的 tensor 放进网络，调用 [ProcessNetwork()](g:/jetson-inference/c/segNet.cpp#L552)。这一步的结果还不是彩色 mask，而是网络输出的原始分数图，也就是每个位置对每个类别的 score/logit。
4. 后处理 `classify()`在 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L558) 调用 [classify()](g:/jetson-inference/c/segNet.cpp#L575)。这一步很关键，它做的是：

   - 读取网络输出 `mOutputs[0].CPU`，见 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L577)
   - 取出输出张量宽高和类别数，见 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L580) 到 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L582)
   - 如果设置了 `ignore_class`，先找到要忽略的类别 ID，见 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L590) 到 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L591)
   - 对输出图上的每个位置，遍历所有类别，取最大分数那个类别，见 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L597) 到 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L623)

   这一步本质上就是在做 **argmax**：“这个像素/网格点最终属于哪个类别？”

   得到的结果被存进 `mClassMap`。这就是分割网络真正的“类别图”。
5. 缓存本次输入信息
   最后把这次输入图像和尺寸缓存起来，见 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L564) 到 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L568)。
   这样后面的 `Overlay()` 才知道要往哪张原图上叠加。

所以一句话总结：

**`Process()` = 输入检查 + 预处理 + TensorRT 推理 + argmax 得到 `mClassMap`。**
它的输出不是最终彩色图，而是内部的分割类别结果。

然后后面两条路才是显示：

- [Mask()](g:/jetson-inference/c/segNet.h#L201) / [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L688)把 `mClassMap` 变成纯分割结果图
- [Overlay()](g:/jetson-inference/c/segNet.h#L229) / [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L714)
  把分割颜色叠加到原图上

而真正逐像素绘制 overlay/mask 的 GPU kernel 在 [c/segNet.cu](g:/jetson-inference/c/segNet.cu#L30) 和 [c/segNet.cu](g:/jetson-inference/c/segNet.cu#L159)。

### `Overlay()`

`Overlay()` 这一层的作用，就是把 `Process()` 得到的 `mClassMap` 和原始输入图像组合起来，生成你最终看到的彩色叠加图。

调用链大概是这样：

```text
segNet::Process()
  ↓
得到 mClassMap（每个位置的类别ID）

segNet::Overlay()
  ↓
根据 filter 选择 point / linear
  ↓
overlayPoint() / overlayLinear()
  ↓
cudaSegOverlay()
  ↓
gpuSegOverlay<<<...>>>()
  ↓
输出最终 overlay 图像
```

具体看 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L714)：

1. `Overlay()` 先检查输出参数是否合法例如输出图像指针、宽高是否有效。还会检查你是不是已经先调用过 `Process()`，因为没有前面的分割结果就没法 overlay。见 [c/segNet.cpp](g:/jetson-inference/c/segNet.cpp#L724)。
2. `Overlay()` 根据滤波模式决定走哪条路这里的 `filter` 来自 `FilterMode`：

   - `FILTER_POINT`
   - `FILTER_LINEAR`

   然后它会调用：

   - [overlayPoint()](g:/jetson-inference/c/segNet.cpp#L748)
   - 或 [overlayLinear()](g:/jetson-inference/c/segNet.cpp#L828)
3. `overlayPoint()` / `overlayLinear()` 再调用 CUDA 实现它们最后都会调 [cudaSegOverlay()](g:/jetson-inference/c/segNet.cpp#L741)，真正定义在 [c/segNet.cu](g:/jetson-inference/c/segNet.cu#L159)。
4. `cudaSegOverlay()` 负责准备 kernel 启动在 [c/segNet.cu](g:/jetson-inference/c/segNet.cu#L159) 这里，它会：

   - 检查输入输出参数
   - 判断图像格式
   - 根据格式选择合适的 kernel 模板
   - 启动 [gpuSegOverlay](g:/jetson-inference/c/segNet.cu#L30)
5. `gpuSegOverlay<<<...>>>` 在 GPU 上逐像素生成结果它每个线程处理一个输出像素，大致做这些事：

   - 根据输出坐标找到对应的分割类别
   - 取这个类别对应的颜色
   - 如果是 `mask_only`，直接输出分割颜色
   - 如果是 overlay，就把类别颜色和原图像按 alpha 混合

这就是为什么 `.cu` 单独存在：
因为这种“每个像素都做一次映射/混合”的工作量很大，适合写成 CUDA kernel 在 GPU 上并行跑。

`point` 和 `linear` 的区别可以简单理解成：

- `point`
  - 直接按最近邻取分类结果
  - 边界更硬
  - 更快
- `linear`
  - 做线性插值
  - 放大后边缘更平滑
  - 可视化通常更好看

**`Process()` 负责算出“每个位置属于什么类别”，`Overlay()` 负责把这个类别图重新映射并叠加回原图，真正逐像素干活的是 `segNet.cu` 里的 CUDA kernel。**
