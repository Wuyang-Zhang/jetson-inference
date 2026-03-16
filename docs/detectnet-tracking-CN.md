<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">
<palign="right"><sup><a href="detectnet-tao-CN.md">返回</a> | <a href="segnet-console-2-CN.md">下一步</a> | </sup><a href="../README-CN.md#hello-ai-world"><sup>内容</sup></a>
<br/>
<sup>物体检测</sup></p>


# 视频上的对象跟踪


尽管凭借现代检测 DNN 的准确性，您基本上可以进行“通过检测进行跟踪”，但某种程度的时间过滤可能有利于平滑检测中的光点和视频中的临时遮挡。  jetson-inference 包括使用来自 [`High-Speed Tracking-by-Detection Without Using Image Information`](http://elvera.nue.tu-berlin.de/typo3/files/1517Bochinski2017.pdf) 的帧到帧 IOU（交叉交集）边界框比较的基本（但快速）多对象跟踪（DeepStream 有更全面的跟踪实现[从此处](https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvtracker.html)）。


<a href="https://www.youtube.com/watch?v=L8vwuXKQrow" target="_blank"><img src=https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/detectnet-tracking-pedestrians-youtube.jpg></a>


要使用 detectornet/detectnet.py 启用跟踪，请使用 `--tracking` 标志运行它。


`` 重击
# 下载测试视频
wget https://nvidia.box.com/shared/static/veuuimq6pwvd62p9fresqhrrmfqz0e2f.mp4 -O 行人.mp4


# C++
$ detectornet --model=peoplenet --tracking 行人.mp4 行人_tracking.mp4


# Python
$ detectornet.py --model=peoplenet --tracking 行人.mp4 行人_tracking.mp4
````


### 追踪参数


您可以使用以下命令行选项更改其他跟踪器设置: 


```
objectTracker arguments:
  --tracking               flag to enable default tracker (IOU)
  --tracker-min-frames=N   the number of re-identified frames for a track to be considered valid (default: 3)
  --tracker-drop-frames=N  number of consecutive lost frames before a track is dropped (default: 15)
  --tracker-overlap=N      how much IOU overlap is required for a bounding box to be matched (default: 0.5)
```


这些设置也可以通过 Python 使用 [`detectNet.SetTrackerParams()`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#detectNet) 函数进行，或者在 C++ 中通过 [`objectTracker`](../c/tracking/objectTrackerIOU.h) API 进行: 


####Python
``Python
从 jetson_utils 导入 detectorNet


网络 = 检测网络()


net.SetTrackingEnabled(True)
net.SetTrackingParams（minFrames = 3，dropFrames = 15，overlapThreshold = 0.5）
````


#### C++
````.cpp
#include <jetson-inference/detectNet.h>
#include <jetson-inference/objectTrackerIOU.h>


detectorNet* net = detectorNet::Create();


net->SetTracker(objectTrackerIOU::Create(3, 15, 0.5f));
````


要以交互方式使用这些设置，您可以从浏览器使用 [Flask webapp](webrtc-flask-CN.md)。


### 追踪状态


启用跟踪后，从 Detect() 返回的 [`detectNet.Detection`](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/structdetectNet_1_1Detection.html) 结果将激活其他变量，这些变量由跟踪器更新并描述每个对象的跟踪状态 - 例如 TrackID、TrackStatus、TrackFrames 和 TrackLost: 


````.cpp
结构检测
{
	// 检测信息
	uint32_t 类ID；	/**< 检测到的对象的类索引。 */
	浮动信心；	/**< 检测到的对象的置信度值。 */


// 追踪信息
	int TrackID；		/**< 唯一的跟踪 ID（如果未跟踪则为 -1） */
	int TrackStatus；	/**< -1 表示已删除，0 表示正在初始化，1 表示活动/有效 */ 
	int TrackFrames；	/**< 物体被重新识别的帧数 */
	int 轨迹丢失；   	/**<连续帧跟踪数量已丢失*/


// 边界框坐标
	向左浮动；		/**< 左边界框坐标（以像素为单位） */
	向右浮动；		/**< 右边界框坐标（以像素为单位） */
	浮动顶部；		/**< 顶部边界框坐标（以像素为单位）*/
	浮动底部；		/**< 底部边界框坐标（以像素为单位） */
};
````


这些可以从 [C++](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/structdetectNet_1_1Detection.html) 和 [Python](https://rawgit.com/dusty-nv/jetson-inference/master/docs/html/python/jetson.inference.html#detectNet) 访问。  例如，从Python: 


```` 蟒蛇
检测= net.Detect(img)


用于检测中的检测: 
    if detector.TrackStatus >= 0: # 主动跟踪
        print(f"({detection.Left}, {detection.Top}) 处的对象 {detection.TrackID} 已被跟踪 {detection.TrackFrames} 帧”)
    else: # 如果跟踪丢失，该对象将在下一帧被丢弃
        print(f"对象 {检测.TrackID} 丢失跟踪")   
````


如果跟踪丢失 (`TrackStatus=-1`)，该对象将被丢弃，并且不再包含在后续帧的检测数组中。


<palign="right">下一个| <b><a href="segnet-console-2-CN.md">语义分割</a></b>
<br/>
返回 | <b><a href="detectnet-tao-CN.md">使用 TAO 检测模型</a></p>
</b><palign="center"><sup>© 2016-2023 NVIDIA | </sup><a href="../README-CN.md#hello-ai-world"><sup>目录</sup></a></p>