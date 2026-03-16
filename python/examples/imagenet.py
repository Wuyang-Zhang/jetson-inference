#!/usr/bin/env python3
#
# Copyright (c) 2020, NVIDIA CORPORATION. All rights reserved.
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
# 程序结构：
# main
#  ├─ argparse 解析参数
#  ├─ imageNet() 载入模型
#  ├─ videoSource() 读取视频
#  ├─ videoOutput() 显示视频
#  └─ while True
#         ├─ Capture() 获取图像
#         ├─ Classify() 推理
#         ├─ OverlayText() 显示结果
#         ├─ Render() 输出画面


import sys
import argparse

from jetson_inference import imageNet  # 核心推理库 （加载模型 运行TensorRT 输出分类 底层其实是TensorRT engine）
from jetson_utils import videoSource, videoOutput, cudaFont, Log  # Jetson GPU工具库
# | 类           | 功能       |
# | ----------- | -------- |
# | videoSource | 摄像头/视频输入 |
# | videoOutput | 显示输出     |
# | cudaFont    | GPU文字绘制  |


# parse the command line
parser = argparse.ArgumentParser(description="Classify a live camera stream using an image recognition DNN.", 
                                 formatter_class=argparse.RawTextHelpFormatter, 
                                 epilog=imageNet.Usage() + videoSource.Usage() + videoOutput.Usage() + Log.Usage())

parser.add_argument("input", type=str, default="", nargs='?', 
                    help="URI of the input streamL : /dev/video0 ; file.mp4 ; rtsp://")
parser.add_argument("output", type=str, default="", nargs='?', 
                    help="URI of the output stream :display ; file.mp4 ; rtsp")
parser.add_argument("--network", type=str, default="googlenet", 
                    help="pre-trained model to load (see below for options) 这些模型 已经预编译为TensorRT engine。")
parser.add_argument("--topK", type=int, default=1, 
                    help="show the topK number of class predictions (default: 1)")

try:
	args = parser.parse_known_args()[0]
except:
	print("")
	parser.print_help()
	sys.exit(0)


# load the recognition network 非常关键: 1 加载ONNX模型 2 TensorRT build engine  3 创建CUDA buffer  4 创建执行context
# 内部流程
net = imageNet(args.network, sys.argv)

# note: to hard-code the paths to load a model, the following API can be used:
#
# net = imageNet(model="model/resnet18.onnx", labels="model/labels.txt", 
#                 input_blob="input_0", output_blob="output_0")

# create video sources & outputs
input = videoSource(args.input, argv=sys.argv)  # GStreamer pipeline
output = videoOutput(args.output, argv=sys.argv) # 会打开 Jetson 显示窗口。
font = cudaFont()  # GPU绘制文字  不需要CPU copy

# process frames until EOS or the user exits
while True:
    # capture the next image
    img = input.Capture()  # 返回： CUDA image  不是 numpy  实际上是：CUDA memory pointer 避免CPU拷贝

    if img is None: # timeout
        continue  

    # classify the image and get the topK predictions
    # if you only want the top class, you can simply run:
    #   class_id, confidence = net.Classify(img)
    predictions = net.Classify(img, topK=args.topK)

    # draw predicted class labels
    for n, (classID, confidence) in enumerate(predictions):
        classLabel = net.GetClassLabel(classID)
        confidence *= 100.0

        print(f"imagenet:  {confidence:05.2f}% class #{classID} ({classLabel})")

        font.OverlayText(img, text=f"{confidence:05.2f}% {classLabel}", 
                         x=5, y=5 + n * (font.GetSize() + 5),
                         color=font.White, background=font.Gray40)
                         
    # render the image
    output.Render(img)

    # update the title bar
    output.SetStatus("{:s} | Network {:.0f} FPS".format(net.GetNetworkName(), net.GetNetworkFPS()))

    # print out performance info
    net.PrintProfilerTimes()  # 输出：PreProces 、Inferenc、PostProcess 用于分析：推理瓶颈

    # exit on input/output EOS
    if not input.IsStreaming() or not output.IsStreaming():
        break
