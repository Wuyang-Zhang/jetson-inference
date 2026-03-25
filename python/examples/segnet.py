#!/usr/bin/env python3
#
# Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#

import sys
import argparse

from jetson_inference import segNet
from jetson_utils import videoSource, videoOutput, cudaOverlay, cudaDeviceSynchronize, Log

from segnet_utils import *

# 这个示例脚本演示的是“语义分割”的标准流程：
# 1. 从摄像头/视频中取一帧图像
# 2. 送入 segNet 做每像素分类
# 3. 把分割结果生成为 overlay / mask
# 4. 再输出到窗口或文件
#
# 这里先解析命令行参数，是为了把“输入源、输出源、模型、可视化方式”都保持成可配置的，
# 这样同一份脚本既能跑摄像头，也能跑视频文件，还能切不同分割模型。
parser = argparse.ArgumentParser(description="Segment a live camera stream using an semantic segmentation DNN.", 
                                 formatter_class=argparse.RawTextHelpFormatter, 
                                 epilog=segNet.Usage() + videoSource.Usage() + videoOutput.Usage() + Log.Usage())

parser.add_argument("input", type=str, default="", nargs='?', help="URI of the input stream")
parser.add_argument("output", type=str, default="", nargs='?', help="URI of the output stream")
parser.add_argument("--network", type=str, default="fcn-resnet18-voc", help="pre-trained model to load, see below for options")
parser.add_argument("--filter-mode", type=str, default="linear", choices=["point", "linear"], help="filtering mode used during visualization, options are:\n  'point' or 'linear' (default: 'linear')")
parser.add_argument("--visualize", type=str, default="overlay,mask", help="Visualization options (can be 'overlay' 'mask' 'overlay,mask'")
parser.add_argument("--ignore-class", type=str, default="void", help="optional name of class to ignore in the visualization results (default: 'void')")
parser.add_argument("--alpha", type=float, default=150.0, help="alpha blending value to use during overlay, between 0.0 and 255.0 (default: 150.0)")
parser.add_argument("--stats", action="store_true", help="compute statistics about segmentation mask class output")

try:
    args = parser.parse_known_args()[0]
except:
    print("")
    parser.print_help()
    sys.exit(0)

# 加载分割网络。
# 这里传的是网络名 + sys.argv，底层会继续解析是否用了 --model/--labels/--input_blob/--output_blob
# 这类参数，所以示例脚本本身可以保持简洁，而把模型装载细节交给底层封装处理。
net = segNet(args.network, sys.argv)

# note: to hard-code the paths to load a model, the following API can be used:
#
# net = segNet(model="model/fcn_resnet18.onnx", labels="model/labels.txt", colors="model/colors.txt",
#              input_blob="input_0", output_blob="output_0")

# 设置 overlay 的透明度。
# 分割结果常常不是直接单独显示，而是叠加在原图上，这样更容易同时看清原始画面和分割区域。
net.SetOverlayAlpha(args.alpha)

# 创建输出端。它既可以是显示窗口，也可以是文件、RTSP、图像序列等 URI。
output = videoOutput(args.output, argv=sys.argv)

# segmentationBuffers 是这个脚本的辅助缓冲区管理器。
# 它负责按当前输入尺寸准备 overlay/mask/composite 这些 CUDA 缓冲区，
# 这样主循环里只关心“什么时候推理、什么时候可视化”，不用反复手写分配逻辑。
buffers = segmentationBuffers(net, args)

# 创建输入端。和 output 一样，统一走 URI 抽象，便于在摄像头/视频/图片之间切换。
input = videoSource(args.input, argv=sys.argv)

# 主循环：不断取帧 -> 推理 -> 可视化 -> 输出。
# 这是实时视觉应用里最典型的一条流水线。
while True:
    # 采集一帧输入图像。
    # Capture() 返回的是位于 CUDA 内存中的图像，这样后面的推理和叠加可以直接在 GPU 上完成，
    # 减少 CPU/GPU 来回拷贝。
    img_input = input.Capture()

    # None 通常表示当前这次采集超时，不一定是流结束，所以这里直接继续下一轮。
    if img_input is None: # timeout
        continue
        
    # 根据当前输入尺寸分配/更新可视化缓冲区。
    # 这么做是因为输入分辨率可能变化，或者第一次进入循环时还不知道图像尺寸。
    buffers.Alloc(img_input.shape, img_input.format)

    # 执行分割网络推理。
    # 对 segNet 来说，Process() 的语义是“生成每个像素的类别结果”，
    # 和分类网络 imageNet 的 Classify() 不同。
    net.Process(img_input, ignore_class=args.ignore_class)

    # 生成 overlay：把分割颜色结果叠加到原图上。
    # 这种显示方式适合观察“模型把原图中的哪些区域分到了哪些类别”。
    if buffers.overlay:
        net.Overlay(buffers.overlay, filter_mode=args.filter_mode)

    # 生成 mask：输出纯分割结果图，不保留原图细节。
    # 这种方式更适合单独检查分割边界和类别区域。
    if buffers.mask:
        net.Mask(buffers.mask, filter_mode=args.filter_mode)

    # 如果开启 composite，就把 overlay 和 mask 拼到一张大图里方便同时对比。
    if buffers.composite:
        cudaOverlay(buffers.overlay, buffers.composite, 0, 0)
        cudaOverlay(buffers.mask, buffers.composite, buffers.overlay.width, 0)

    # 输出最终画面。
    # buffers.output 会根据可视化选项自动指向 overlay / mask / composite 中合适的那个。
    output.Render(buffers.output)

    # 更新窗口标题栏，显示当前网络名和推理 FPS，便于实时观察性能。
    output.SetStatus("{:s} | Network {:.0f} FPS".format(args.network, net.GetNetworkFPS()))

    # 同步 GPU，确保这一帧相关的 CUDA 操作都执行完成。
    # 这样后面打印 profiler 时间和做统计时，拿到的是这一帧真实完成后的结果。
    cudaDeviceSynchronize()
    net.PrintProfilerTimes()

    # 可选统计：统计当前分割结果里各类别像素占比/面积等信息。
    # 这对调试模型输出和理解分割结果很有帮助，但会增加一些额外开销。
    if args.stats:
        buffers.ComputeStats()

    # 输入流或输出流结束时退出主循环。
    if not input.IsStreaming() or not output.IsStreaming():
        break
