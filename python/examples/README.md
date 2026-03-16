<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg">

# Inference Samples (Python)

This directory contains Python examples of using jetson-inference for image recognition, object detection, semantic segmentation, and other DNNs for computer vision.  Refer to the [tutorial steps](../../README.md#hello-ai-world-inference-only) for information about running these examples.

For examples of working with numpy arrays, see [`cuda-from-numpy.py`](https://github.com/dusty-nv/jetson-utils/blob/master/python/examples/cuda-from-numpy.py) and [`cuda-to-numpy.py`](https://github.com/dusty-nv/jetson-utils/blob/master/python/examples/cuda-to-numpy.py)

> **note**:  for C++ samples, see the [**`jetson-inference/examples/`**](../../examples).



# 推理示例（Python）

本目录包含使用 **jetson-inference** 的 **Python 示例程序**，用于实现以下计算机视觉任务：

- 图像分类（image recognition）
- 目标检测（object detection）
- 语义分割（semantic segmentation）
- 以及其他基于深度神经网络（DNN）的计算机视觉任务

有关如何运行这些示例的说明，请参考教程步骤：
 **[Hello AI World - 仅推理部分](../../README.md#hello-ai-world-inference-only)**。

如果需要查看 **如何与 NumPy 数组进行交互的示例**，可以参考以下代码：

- [`cuda-from-numpy.py`](https://github.com/dusty-nv/jetson-utils/blob/master/python/examples/cuda-from-numpy.py)
- [`cuda-to-numpy.py`](https://github.com/dusty-nv/jetson-utils/blob/master/python/examples/cuda-to-numpy.py)

这些示例展示了 **NumPy 数组与 CUDA GPU 内存之间的数据转换方法**。

> **注意：**
>  如果需要查看 **C++ 示例程序**，请参考目录：
>  **`jetson-inference/examples/`**