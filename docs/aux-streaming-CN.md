<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="../README-CN.md#appendix">返回</a> | <a href="aux-image-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>附录</sup></p>


# 相机流媒体和多媒体


该项目支持通过各种接口和协议捕获和流式传输视频源和静态图像，包括: 


* [MIPI CSI 相机](#mipi-csi-cameras)

* [V4L2 相机](#v4l2-cameras)

* [WebRTC](#webrtc)

* [RTP](#rtp) / [RTSP](#rtsp)

* [视频](#video-files) 和 [图片](#image-files)

* [图像序列](#image-files)

* [OpenGL 窗口](#output-streams)


流通过资源 URI 进行标识，并通过 [`videoSource`](#source-code) 和 [`videoOutput`](#source-code) API 获取。  这些设置可以通过命令行参数或在应用程序的[源代码](#source-code)中进行配置。  下表显示了支持的输入/输出协议和 URI 格式: 


### 输入流


|                                      |协议|资源 URI |笔记|
|--------------------------------------|--------------|--------------------------------------------|------------------------------------------------------------------------|
| [MIPI CSI 摄像头](#mipi-csi-cameras) | `csi://` | `csi://0` | CSI 摄像机 0（用其他摄像机编号替换 `0`）|
| [V4L2 相机](#v4l2-cameras) | `v4l2://` | `v4l2:///dev/video0` | V4L2 设备 0（用其他摄像机编号替换 `0`）|
| [WebRTC 流](#webrtc) | `webrtc://` | `webrtc://@:8554/my_input` |从浏览器网络摄像头到本地主机，端口 8554（需要 HTTPS/SSL）|
| [RTP 流](#rtp) | `rtp://` | `rtp://@:1234` |本地主机，端口 1234（需要额外配置）|
| [RTSP 流](#rtsp) | `rtsp://` | `rtsp://<remote-ip>:1234` |将 `<remote-ip>` 替换为远程主机的 IP 或主机名 |
| [视频文件](#video-files) | `file://` | `file://my_video.mp4` |支持加载 MP4、MKV、AVI、FLV（请参阅下面的编解码器）|
| [图像文件](#image-files) | `file://` | `file://my_image.jpg` |支持加载JPG、PNG、TGA、BMP、GIF等。           |
| [图像序列](#image-files) | `file://` | `file://my_directory/` |按字母数字顺序搜索图像 |


* 支持的解码器编解码器: H.264、H.265、VP8、VP9、MPEG-2、MPEG-4、MJPEG

* 可以从 URI 中省略 `file://`、`v4l2://` 和 `csi://` 协议前缀作为简写


### 输出流


|                                  |协议|资源 URI |笔记|
|----------------------------------|--------------|--------------------------------------------------------|------------------------------------------------------------------------|
| [WebRTC 流](#webrtc) | `webrtc://` | `webrtc://@:8554/my_output` |发送到浏览器，端口 8554，流名称 `"my_output"` |
| [RTP 流](#rtp) | `rtp://` | `rtp://<remote-ip>:1234` |将 `<remote-ip>` 替换为远程主机的 IP 或主机名 |
| [RTSP 流](#rtsp) | `rtsp://` | `rtsp://@:1234/my_output` |联系方式: `rtsp://<jetson-ip>:1234/my_output` |
| [视频文件](#video-files) | `file://` | `file://my_video.mp4` |支持保存 MP4、MKV、AVI、FLV（请参阅下面的编解码器）|
| [图像文件](#image-files) | `file://` | `file://my_image.jpg` |支持保存JPG、PNG、TGA、BMP |
| [图像序列](#image-files) | `file://` | `file://image_%i.jpg` | `%i` 被序列 | 中的图像编号替换
| [OpenGL 窗口](#output-streams) | `display://` | `display://0` |在屏幕 0 上创建 GUI 窗口 |


* 支持的编码器编解码器: H.264、H.265、VP8、VP9、MJPEG

* 可以从 URI 中省略 `file://` 协议前缀作为简写

* 默认情况下，除非指定 `--headless` ，否则将创建 OpenGL 显示窗口


## 命令行参数


jetson-inference 中的每个示例 C++ 和 Python 程序都接受相同的一组命令行参数来指定流 URI 和其他选项。因此，这些选项可以用于任何示例（例如 [`imagenet`](../examples/imagenet/imagenet.cpp)/[`imagenet.py`](../examples/python/imagenet.py)、[`detectnet`](../examples/detectnet/detectnet.cpp)/[`detectnet.py`](../examples/python/detectnet.py)、[`segnet`](../examples/segnet/segnet.cpp)/[`segnet.py`](../examples/python/segnet.py)、 [`video-viewer`](https://github.com/dusty-nv/jetson-utils/tree/master/video/video-viewer/video-viewer.cpp)/[`video-viewer.py`](https://github.com/dusty-nv/jetson-utils/tree/master/python/examples/video-viewer.py) 等）。  这些命令行参数通常采用以下形式: 


```bash
$ imagenet [options] input_URI [output_URI]  # output URI is optional (default is display://0)
```


其中输入和输出 URI 由两个位置参数指定。  例如: 


```bash
$ imagenet input.jpg output.jpg              # classify input.jpg, save as output.jpg
```


如上所述，这里可以替换 jetson-inference 中的任何示例，因为它们使用相同的命令行解析。  以下是运行每个程序时可以指定的附加流选项: 


#### 输入选项


```
    input                resource URI of the input stream, for example:
                             * /dev/video0               (V4L2 camera #0)
                             * csi://0                   (MIPI CSI camera #0)
                             * rtp://@:1234              (RTP stream)
                             * rtsp://user:pass@ip:1234  (RTSP stream)
                             * webrtc://@:1234/my_stream (WebRTC stream)
                             * file://my_image.jpg       (image file)
                             * file://my_video.mp4       (video file)
                             * file://my_directory/      (directory of images)
  --input-width=WIDTH    explicitly request a width of the stream (optional)
  --input-height=HEIGHT  explicitly request a height of the stream (optional)
  --input-rate=RATE      explicitly request a framerate of the stream (optional)
  --input-save=FILE      path to video file for saving the input stream to disk
  --input-codec=CODEC    RTP requires the codec to be set, one of these:
                             * h264, h265
                             * vp8, vp9
                             * mpeg2, mpeg4
                             * mjpeg
  --input-decoder=TYPE   the decoder engine to use, one of these:
                             * cpu
                             * omx  (aarch64/JetPack4 only)
                             * v4l2 (aarch64/JetPack5 only)
  --input-flip=FLIP      flip method to apply to input:
                             * none (default)
                             * counterclockwise
                             * rotate-180
                             * clockwise
                             * horizontal
                             * vertical
                             * upper-right-diagonal
                             * upper-left-diagonal
  --input-loop=LOOP      for file-based inputs, the number of loops to run:
                             * -1 = loop forever
                             *  0 = don't loop (default)
                             * >0 = set number of loops
```


#### 输出选项


```
    output               resource URI of the output stream, for example:
                             * file://my_image.jpg       (image file)
                             * file://my_video.mp4       (video file)
                             * file://my_directory/      (directory of images)
                             * rtp://<remote-ip>:1234    (RTP stream)
                             * rtsp://@:8554/my_stream   (RTSP stream)
                             * webrtc://@:1234/my_stream (WebRTC stream)
                             * display://0               (OpenGL window)
  --output-codec=CODEC   desired codec for compressed output streams:
                            * h264 (default), h265
                            * vp8, vp9
                            * mpeg2, mpeg4
                            * mjpeg
  --output-encoder=TYPE  the encoder engine to use, one of these:
                            * cpu
                            * omx  (aarch64/JetPack4 only)
                            * v4l2 (aarch64/JetPack5 only)
  --output-save=FILE     path to a video file for saving the compressed stream
                         to disk, in addition to the primary output above
  --bitrate=BITRATE      desired target VBR bitrate for compressed streams,
                         in bits per second. The default is 4000000 (4 Mbps)
  --headless             don't create a default OpenGL GUI window
```


以下是在各种类型的流上启动 `video-viewer` 工具的示例命令。  您可以用其他程序替换这些命令中的 `video-viewer` ，因为它们解析相同的参数。  在此页面的[源代码](#source-code)部分，您可以浏览`video-viewer`源代码的内容，以展示如何在您自己的应用程序中使用`videoSource`和`videoOutput` API。


## MIPI CSI 相机


MIPI CSI 相机是紧凑型传感器，可通过 Jetson 的硬件 CSI/ISP 接口直接获取。  支持的 CSI 摄像机包括: 


* [Raspberry Pi 相机模块 v2](https://www.raspberrypi.org/products/camera-module-v2/) (IMX219) 适用于 Jetson Nano 和 Jetson Xavier NX

* Jetson TX1/TX2 开发套件中的 OV5693 相机模块。

* 请参阅 [Jetson 合作伙伴支持的相机](https://developer.nvidia.com/embedded/jetson-partner-supported-cameras) 页面，了解生态系统支持的更多传感器。


以下是使用 MIPI CSI 相机启动的一些示例。  如果连接了多个 CSI 摄像机，请将摄像机编号替换为 0: 


```bash
$ video-viewer csi://0                        # MIPI CSI camera 0 (substitue other camera numbers)
$ video-viewer csi://0 output.mp4             # save output stream to MP4 file (H.264 by default)
$ video-viewer csi://0 rtp://<remote-ip>:1234 # broadcast output stream over RTP to <remote-ip>
```


默认情况下，CSI 摄像机将以 1280x720 分辨率创建。  要指定不同的分辨率，请使用 `--input-width` 和 `input-height` 选项。  请注意，指定的分辨率必须与相机支持的格式之一匹配。


```bash
$ video-viewer --input-width=1920 --input-height=1080 csi://0
```


## V4L2相机


USB 网络摄像头最常作为 V4L2 设备受到支持，例如 Logitech [C270](https://www.logitech.com/en-us/product/hd-webcam-c270) 或 [C920](https://www.logitech.com/en-us/product/hd-pro-webcam-c920)。


```bash
$ video-viewer v4l2:///dev/video0                 # /dev/video0 can be replaced with /dev/video1, ect.
$ video-viewer /dev/video0                        # dropping the v4l2:// protocol prefix is fine
$ video-viewer /dev/video0 output.mp4             # save output stream to MP4 file (H.264 by default)
$ video-viewer /dev/video0 rtp://<remote-ip>:1234 # broadcast output stream over RTP to <remote-ip>
```


> **注意: ** 如果您插入了 MIPI CSI 摄像头，它也会显示为 `/dev/video0`。  然后，如果您插入 USB 网络摄像头，它将显示为 `/dev/video1`，因此您需要在上面的命令中替换 `/dev/video1`。  此项目不支持通过 V4L2 使用 CSI 相机，因为通过 V4L2，它们使用原始拜耳而不使用 ISP（相反，使用 CSI 相机，如 [上面](#mipi-csi-cameras) 所示）。


#### V4L2 格式


默认情况下，将使用与所需分辨率最匹配的最高帧速率的相机格式创建 V4L2 相机（默认情况下，该分辨率为 1280x720）。  可以对具有最高帧速率的格式进行编码（例如使用 H.264 或 MJPEG），因为 USB 相机通常以较低帧速率传输未压缩的 YUV/RGB。  在这种情况下，将检测到该编解码器，并使用 Jetson 的硬件解码器自动对摄像机流进行解码，以获得最高帧速率。


如果您明确想要选择 V4L2 相机使用的格式，可以使用 `--input-width`、`--input-height` 和 `--input-codec` 选项来实现。  可能的解码器编解码器选项是 `--input-codec=h264, h265, vp8, vp9, mpeg2, mpeg4, mjpeg`


```bash
$ video-viewer --input-width=1920 --input-height=1080 --input-codec=h264 /dev/video0
```


当您在 V4L2 源上运行 jetson-inference 程序之一时，V4L2 相机支持的不同格式将记录到终端。  但是，您也可以使用 `v4l2-ctl` 命令列出这些支持的格式: 


```bash
$ sudo apt-get install v4l-utils
$ v4l2-ctl --device=/dev/video0 --list-formats-ext
```


## 网络RTC


该项目包括一个内置的 WebRTC 服务器（输入/输出），用于将视频流传输到客户端 Web 浏览器或从客户端 Web 浏览器传输视频。  当您的 Jetson 无头且未连接显示器时，您可以使用它方便地查看视频流，或者轻松构建在后端使用 Jetson 和边缘 AI 的交互式 Web 应用程序。  测试的浏览器包括 Chrome/Chromium、移动 Android 和移动 iOS (Safari)。


```bash
$ video-viewer /dev/video0 webrtc://@:8554/my_output               # send V4L2 webcam to browser
$ video-viewer webrtc://@:8554/my_input output.mp4                 # receive browser webcam (requires HTTPS/SSL) and save to MP4
$ video-viewer webrtc://@:8554/my_input webrtc://@:8554/my_output  # receieve + send (full-duplex loopback)
```


> **注意**: 接收浏览器网络摄像头需要启用 [HTTPS/SSL](webrtc-server-CN.md#enabling-https--ssl)


然后，您应该能够将浏览器导航到 `https://<JETSON-IP>:8554` 以查看该流。  Hello AI World 教程中有一个完整的部分专门介绍如何使用 WebRTC 以及使用各种 Web 应用程序框架构建应用程序: 


* [WebRTC 服务器](webrtc-server-CN.md)

* [WebAPP 框架](../README-CN.md#webapp-frameworks)


## 实时传输协议


RTP 网络流通过 UDP/IP 广播到特定主机或多播组。  接收 RTP 流时，必须指定编解码器 (`--input-codec`)，因为 RTP 无法动态查询它。  这将使用 RTP 作为来自另一台设备的输入: 


```bash
$ video-viewer --input-codec=h264 rtp://@:1234         # recieve on localhost port 1234
$ video-viewer --input-codec=h264 rtp://224.0.0.0:1234 # subscribe to multicast group
```


上面的命令指定 RTP 作为输入源，网络上的另一个远程主机正在将流传输到 Jetson。  但是，您也可以从 Jetson 输出 RTP 流并将其传输到网络上的另一台远程主机。


#### 传输RTP


要传输 RTP 输出流，请将目标 IP/端口指定为 `output_URI`。如果需要，您可以指定比特率（默认为 `--bitrate=4000000` 或 4Mbps）和/或输出编解码器（默认为 `--output-codec=h264`），可以是 `h264, h265, vp8, vp9, mjpeg`


```bash
$ video-viewer --bitrate=1000000 csi://0 rtp://<remote-ip>:1234         # transmit camera over RTP, encoded as H.264 @ 1Mbps 
$ video-viewer --output-codec=h265 my_video.mp4 rtp://<remote-ip>:1234  # transmit a video file over RTP, encoded as H.265
```


输出 RTP 时，您需要显式设置流发送到的远程主机（或多播组）的 IP 地址或主机名（如上所示 `<remote-ip>`）。  请参阅下文，了解有关从 PC 查看 RTP 流的一些提示。


#### 远程查看RTP


如果您的 Jetson 正在将 RTP 传输到另一台远程主机（例如 PC），则可以使用以下一些示例命令来查看流: 


* 使用 GStreamer: 

	* [安装 GStreamer](https://gstreamer.freedesktop.org/documentation/installing/index.html) 并运行此管道（将 `port=1234` 替换为您正在使用的端口）


```bash
	$ gst-launch-1.0 -v udpsrc port=1234 \
	caps = "application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96" ! \
	rtph264depay ! decodebin ! videoconvert ! autovideosink
	```


* 使用 VLC 播放器: 

	* 创建包含以下内容的 SDP 文件 (.sdp)（将 `1234` 替换为您正在使用的端口）


```
     c=IN IP4 127.0.0.1
     m=video 1234 RTP/AVP 96
     a=rtpmap:96 H264/90000
	```


	* 双击 SDP 文件在 VLC 中打开流

	* 您可能需要减少 VLC 中的 `File caching` 和 `Network caching` 设置，如[此处所示](https://www.howtogeek.com/howto/windows/fix-for-vlc-skipping-and-lagging-playing-high-def-video-files/)


* 如果您的远程主机是另一个 Jetson: 

	* 使用与[上面](#rtp)相同的 `video-viewer` 命令（将 `1234` 替换为您正在使用的端口）


```bash
	$ video-viewer --input-codec=h264 rtp://@:1234
     ```


## 实时传输协议


RTSP 网络流通过 UDP/IP 从远程主机订阅。  与 RTP 不同，RTSP 可以动态查询流属性（如分辨率和编解码器），因此不需要显式提供这些选项。


#### RTSP 输入


要连接到 RTSP 源，请提供提供该源的 RTSP 服务器的 IP 地址和 URL: 


```bash
$ video-viewer rtsp://<remote-ip>:1234 my_video.mp4      # subscribe to RTSP feed from <remote-ip>, port 1234 (and save it to file)
$ video-viewer rtsp://username:password@<remote-ip>:1234 # with authentication (replace username/password with credentials)
```


如果 RTSP 服务器启用了身份验证，您可能需要在 URL 中提供用户名/密码凭据。


#### RTSP 输出


jetson-utils 包含一个内置 RTSP 服务器，使用 GStreamer 将压缩的 RTSP 流发送到多个客户端: 


```bash
$ video-viewer /dev/video0 rtsp://@:1234/my_output                 # stream a V4L2 camera out over RTSP 
$ video-viewer rtsp://<remote-ip>:1234/input rtsp://@:1234/output  # subscribe to an RTSP feed, and relay it (loopback)
```


> **注意: ** 可以以与 WebRTC 相同的方式为 RTSP 输出启用 SSL 加密


然后，您应该能够从 URL `rtsp://<jetson-ip>:1234/my_output` 处的 RTSP 客户端（如 VLC 播放器）打开并查看流


## 视频文件


您可以播放和录制 MP4、MKV、AVI 和 FLV 格式的压缩视频文件（以及非容器化的 H264/H265）。


````bash
# 回放
$ video-viewer my_video.mp4 # 显示视频文件
$ video-viewer my_video.mp4 rtp://<remote-ip>:1234 # 通过 RTP 传输视频


# 录音
$ video-viewer csi://0 my_video.mp4 # 将 CSI 摄像头录制到视频文件
$ video-viewer /dev/video0 my_video.mp4 # 将V4L2摄像头录制到视频文件
````


#### 编解码器


加载视频文件时，会自动检测编解码器和分辨率，因此不需要设置这些。
保存视频文件时，默认编解码器是 H.264，但这可以使用 `--output-codec` 选项进行设置。


```bash
$ video-viewer --output-codec=h265 input.mp4 output.mp4  # transcode video to H.265
```


支持以下编解码器: 


* 解码 - `h264, h265, vp8, vp9, mpeg2, mpeg4, mjpeg`

* 编码 - `h264, h265, vp8, vp9, mjpeg`



#### 调整输入大小


加载视频文件时，会自动检测分辨率。  但是，如果您希望将输入视频重新缩放到不同的分辨率，则可以指定 `--input-width` 和 `--input-height` 选项: 


```bash
$ video-viewer --input-width=640 --input-height=480 my_video.mp4  # resize video to 640x480
```


#### 循环输入


默认情况下，一旦到达流末尾 (EOS)，视频就会终止。  但是，通过指定 `--loop` 选项，您可以设置希望视频运行的循环次数。  `--loop` 的可能选项有: 


* `-1` = 永远循环

* &nbsp;` 0` = 不循环（默认）

* `>0` = 设置循环次数


```bash
$ video-viewer --loop=10 my_video.mp4    # loop the video 10 times
$ video-viewer --loop=-1 my_video.mp4    # loop the video forever (until user quits)
```


#### 第二目的地


有时，除了主输出流之外，您可能还希望将未处理的摄像机输入（或后处理的视频）保存到磁盘。  对于已经压缩的传入输入（例如，H264 编码的摄像机或网络流），可以使用 `--input-save=<FILE>` 选项在解码和处理之前将编码视频转储到磁盘。  它支持 MP4、MKV、AVI、FLV 和原始 H264/H265。


对于要压缩的输出流（即 WebRTC/RTP/RTSP 等网络流），`--output-save=<FILE>` 选项除了其主要输出之外还将处理后的视频记录到磁盘。  要保存输出视频文件，同时将其显示在连接到 Jetson 的屏幕上（未经过压缩），只需使用上面的方法[录制视频](#video-files)，OpenGL GUI 窗口就会自动打开。



``` bash
$ detectnet --input-codec=h264 --input-save=camera_dump.mp4 /dev/video0       # save incoming/unprocessed video
$ detectnet --output-save=post_dump.mp4 /dev/video0 rtsp://@:1234/my_stream   # save outgoing/processed video
```


> **注意: ** `--input-save` 和 `--output-save` 只能与已经压缩/编码的流结合使用。


第一个命令将转储原始摄像机视频，第二个命令将在处理后转储它（例如包括边界框等）


## 图像文件


您可以加载/保存以下格式的图像文件: 


* 加载: JPG、PNG、TGA、BMP、GIF、PSD、HDR、PIC 和 PNM（PPM/PGM 二进制）

* 保存: JPG、PNG、TGA、BMP


```bash
$ video-viewer input.jpg output.jpg	# load/save an image
```


您还可以循环图像和图像序列 - 请参阅上面的[循环输入](#looping-inputs) 部分。


#### 序列


如果路径是目录或包含通配符，则将按顺序加载/保存所有图像（按字母数字顺序）。


```bash
$ video-viewer input_dir/ output_dir/   # load all images from input_dir and save them to output_dir
$ video-viewer "*.jpg" output_%i.jpg    # load all jpg images and save them to output_0.jpg, output_1.jpg, ect
```


> **注意: ** 使用通配符时，请始终将其括在引号 (`"*.jpg"`) 中。否则，操作系统将自动扩展序列并修改命令行上的参数顺序，这可能会导致输入图像之一被输出覆盖。


保存一系列图像时，如果路径只是目录（`output_dir`），则图像将自动保存为格式为 `output_dir/%i.jpg` 的 JPG，并使用图像编号作为文件名（`output_dir/0.jpg`、`output_dir/1.jpg` 等）。


如果您希望指定文件名格式，请在路径 (`output_dir/image_%i.png`) 中使用 printf 样式 `%i` 来执行此操作。  您可以应用其他 printf 修饰符（例如 `%04i`）来创建文件名（例如 `output_dir/image_0001.jpg`）。



## 源代码


使用 [`videoSource`](https://github.com/dusty-nv/jetson-utils/tree/master/video/videoSource.h) 和 [`videoOutput`](https://github.com/dusty-nv/jetson-utils/tree/master/video/videoOutput.h) 对象访问流。  它们能够通过一组统一的 API 处理上面的每种类型的流。  可以捕获图像并以以下数据格式输出: 


|格式化字符串| [`imageFormat` 枚举](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/group__imageFormat.html#ga931c48e08f361637d093355d64583406) |数据类型 |位深度|
|----------------|------------------|------------------------|-----------|
| `rgb8` | `IMAGE_RGB8` | `uchar3` | 24 |
| `rgba8` | `IMAGE_RGBA8` | `uchar4` | 32 | 32
| `rgb32f` | `IMAGE_RGB32F` | `float3` | 96 | 96
| `rgba32f` | `IMAGE_RGBA32F` | `float4` | 128 | 128


* 数据类型和 [`imageFormat`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/group__imageFormat.html#ga931c48e08f361637d093355d64583406) 枚举是 C++ 类型

* 在Python中，可以将格式字符串传递给`videoSource.Capture()`来请求特定格式（默认为`rgb8`）

* 在 C++ 中，`videoSource::Capture()` 模板将从输出指针的数据类型推断格式


要将图像转换为不同格式或从不同格式转换图像，请参阅[使用 CUDA 进行图像操作](aux-image-CN.md) 页面了解更多信息。


以下是 [`video-viewer.py`](https://github.com/dusty-nv/jetson-utils/tree/master/python/examples/video-viewer.py) 和 [`video-viewer`](https://github.com/dusty-nv/jetson-utils/tree/master/video/video-viewer/video-viewer.cpp) 的源代码，稍作缩写以提高可读性: 


###Python
````蟒蛇
导入系统
导入argparse


从 jetson_utils 导入 videoSource、videoOutput


# 解析命令行
解析器 = argparse.ArgumentParser()
parser.add_argument("input", type=str, help="输入流的URI")
parser.add_argument("output", type=str, default="", nargs='?', help="输出流的 URI")
args = parser.parse_known_args()[0]


# 创建视频源和输出
input = videoSource(args.input, argv=sys.argv) # 默认: options={'width': 1280, 'height': 720, 'framerate': 30}
输出 = videoOutput(args.output, argv=sys.argv) # 默认值: options={'codec': 'h264', 'bitrate': 4000000}


# 捕获帧直到流结束（或用户退出）
而真实: 
    # 格式可以是: rgb8、rgba8、rgb32f、rgba32f（默认为rgb8）
    # 超时可以是: -1表示无限超时（阻塞），0表示立即返回，>0以毫秒为单位（默认为1000ms）
    图像 = input.Capture(format='rgb8', timeout=1000)


if image is None: # 如果发生超时
        继续


输出.渲染（图像）


# 输入/输出 EOS 时退出
    如果不是 input.IsStreaming() 或不是 output.IsStreaming(): 
        打破
````


要在 Python 中对视频配置设置进行硬编码，您可以将可选的 `options` 字典传递给 videoSource/videoOutput 初始值设定项，该字典大致对应于 C++ 中的 [`videoOptions`](https://github.com/dusty-nv/jetson-utils/blob/master/video/videoOptions.h) 结构: 


``` python
input = videoSource("/dev/video0", options={'width': 1280, 'height': 720, 'framerate': 30, 'flipMethod': 'rotate-180'})
output = videoOutput("my_video.mp4", options={'codec': 'h264', 'bitrate': 4000000})
```


输入设置将使用最接近的可用分辨率/帧速率，但建议首先检查相机的[支持的格式](#v4l2-formats)。


### C++
``c++
#include <jetson-utils/videoSource.h>
#include <jetson-utils/videoOutput.h>


int main( int argc, char** argv )
{
    // 创建输入/输出流
    videoSource* 输入 = videoSource::Create(argc, argv, ARG_POSITION(0));
    videoOutput* 输出 = videoOutput::Create(argc, argv, ARG_POSITION(1));


如果（！输入）
        返回0；


// 捕获/显示循环
    而（真）
    {
        uchar3* 图像 = NULL;  // 可以是 uchar3、uchar4、float3、float4
        整数状态=0；        // 查看 videoSource::Status (OK, TIMEOUT, EOS, ERROR)


if( !input->Capture(&image, 1000, &status) ) // 1000 毫秒超时（默认）
        {
            if( 状态 == videoSource::TIMEOUT )
                继续；


休息; // EOS
        }


if( 输出！= NULL )
        {
            输出->渲染(图像, 输入->GetWidth(), 输入->GetHeight());


if( !output->IsStreaming() ) // 检查用户是否退出
                打破；
        }
    }


// 销毁资源
    SAFE_DELETE（输入）；
    SAFE_DELETE（输出）；
}
````


要从编码设置以编程方式创建接口，还可以像这样填充 [`videoOptions`](https://github.com/dusty-nv/jetson-utils/blob/master/video/videoOptions.h) 结构: 


``c++
videoOptions选项；


选项.宽度= 1280;
选项.高度 = 720;
选项.frameRate = 30;
options.flipMethod = videoOptions::FLIP_ROTATE_180;


videoSource* 输入 = videoSource::Create("/dev/video0", 选项);
````


输入设置将使用最接近的可用分辨率/帧速率，但建议首先检查相机的[支持的格式](#v4l2-formats)。


<palign="right">下一个| <b><a href="aux-image-CN.md">使用 CUDA 进行图像处理</a></b>
<palign="center"><sup>© 2016-2020 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>

