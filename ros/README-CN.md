# ROS 深度学习节点说明

这个目录是 `jetson-inference` 提供的 ROS/ROS2 接口层。

如果前面的 `c/`、`python/` 更像“算法库本体”和“Python 绑定”，那么这里的 `ros/` 可以理解成：

- 把 `jetson-inference` 里的分类、检测、分割、视频输入输出能力
- 包装成 ROS/ROS2 可以直接启动的 node
- 通过 topic、parameter、launch 文件接到 ROS 系统里

也就是说，这个目录的重点不是“重新实现推理算法”，而是“把已有推理能力接到 ROS 生态里”。

## 先用一句话理解这个目录

这个包的名字叫 `ros_deep_learning`。

它主要提供 5 类节点：

- `imagenet`
  图像分类节点

- `detectnet`
  目标检测节点

- `segnet`
  语义分割节点

- `video_source`
  视频输入节点

- `video_output`
  视频输出节点

你可以把常见流程理解成：

1. `video_source` 从摄像头、文件、RTSP 等地方读图像
2. 把图像发到 ROS topic
3. `imagenet` / `detectnet` / `segnet` 订阅图像并做推理
4. 把推理结果和叠加后的图像重新发布出去
5. `video_output` 再把结果显示到屏幕、写到文件、推到流媒体输出

## 这个目录里每个文件/目录是干什么的

### 顶层文件

- `README.md`
  原始英文说明，包含安装、测试、topic、参数说明。

- `README-CN.md`
  这份中文说明文档。适合刚开始接触这个包时先看。

- `CMakeLists.txt`
  这个 ROS 包的构建脚本。负责：
  - 查找 `jetson-utils`、`jetson-inference`、CUDA、ROS 依赖
  - 判断当前是 ROS1 还是 ROS2
  - 编译各个节点可执行文件
  - 在 ROS1 下编译 nodelet
  - 安装 launch 文件和二进制

- `package.xml`
  ROS1 风格的包描述文件，声明包名、依赖、导出信息等。

- `package.ros1.xml`
  单独保留的 ROS1 包描述模板。内容和 `package.xml` 类似，主要是为了兼容不同 ROS 版本或构建流程。

- `package.ros2.xml`
  ROS2 风格的包描述文件，声明 `ament_cmake`、`rclcpp`、`sensor_msgs`、`vision_msgs` 等依赖。

- `ros_deep_learning_nodelets.xml`
  ROS1 的 nodelet 插件描述文件。告诉 ROS 这个包里导出了哪些 nodelet 类。

- `.editorconfig`
  编辑器格式配置。

- `.gitignore`
  Git 忽略规则。

### 目录

- `launch/`
  启动文件目录。这里放的是最常用的 demo 和节点组合方式。

- `src/`
  C++ 源码目录。真正的 ROS 节点实现都在这里。

- `config/`
  预留配置目录。目前基本是占位。

## `launch/` 目录里的文件分别做什么

这个目录里基本都是成对出现：

- `*.ros1.launch`
- `*.ros2.launch`

意思是：

- 一套给 ROS1 用
- 一套给 ROS2 用

### 主要 launch 文件

- `video_source.ros1.launch`
- `video_source.ros2.launch`
  启动视频输入节点，负责从摄像头、视频文件、网络流等读取图像。

- `video_output.ros1.launch`
- `video_output.ros2.launch`
  启动视频输出节点，负责把 ROS 图像输出到显示窗口、编码器或流媒体。

- `video_viewer.ros1.launch`
- `video_viewer.ros2.launch`
  最简单的测试组合。通常就是：
  - 启动 `video_source`
  - 再把图像直接接到 `video_output`
  这样可以先确认视频输入输出链路是通的。

- `imagenet.ros1.launch`
- `imagenet.ros2.launch`
  启动图像分类 demo。通常会：
  - 先起 `video_source`
  - 再起 `imagenet`
  - 最后起 `video_output`
  把分类结果画到图像上输出。

- `detectnet.ros1.launch`
- `detectnet.ros2.launch`
  启动目标检测 demo。流程和 `imagenet` 类似，只是中间节点换成 `detectnet`。

- `segnet.ros1.launch`
- `segnet.ros2.launch`
  启动语义分割 demo。输出会包含 overlay、color mask、class mask 等结果。

如果你是刚开始接触这个目录，推荐先看：

1. `video_viewer.*.launch`
2. `imagenet.*.launch`
3. `detectnet.*.launch`
4. `segnet.*.launch`

因为这几份 launch 最能帮助你理解“节点是怎么串起来的”。

## `src/` 目录里的文件分别做什么

### 推理节点

- `node_imagenet.cpp`
  图像分类节点。
  它会：
  - 加载 `imageNet`
  - 订阅输入图像
  - 做分类
  - 发布分类结果、`vision_info` 和 overlay 图像

- `node_detectnet.cpp`
  目标检测节点。
  它会：
  - 加载 `detectNet`
  - 订阅输入图像
  - 执行目标检测
  - 发布 `Detection2DArray`
  - 发布 `vision_info`
  - 按需要发布带框的 overlay 图像

- `node_segnet.cpp`
  语义分割节点。
  它会：
  - 加载 `segNet`
  - 订阅输入图像
  - 生成分割结果
  - 发布 overlay
  - 发布 color mask
  - 发布 class mask
  - 发布 `vision_info`

### 视频节点

- `node_video_source.cpp`
  视频输入节点。
  它底层调用 `jetson-utils` 的 `videoSource`，从这些来源读取图像：
  - CSI 摄像头
  - V4L2 摄像头
  - 视频文件
  - 图片序列
  - RTP / RTSP / WebRTC 流

  然后把图像发布成 ROS 的 `sensor_msgs/Image`。

- `node_video_output.cpp`
  视频输出节点。
  它订阅 ROS 图像消息，再通过 `videoOutput` 输出到：
  - 屏幕窗口
  - 视频文件
  - 网络流

### 辅助适配层

- `image_converter.h`
- `image_converter.cpp`
  图像转换工具。
  作用是把 ROS 的 `sensor_msgs/Image` 和底层 CUDA 图像缓冲区互相转换。因为 ROS 消息格式和 `jetson-inference` / `jetson-utils` 用的 GPU 图像格式不是一回事，所以中间必须有这一层。

- `ros_compat.h`
- `ros_compat.cpp`
  ROS1 / ROS2 兼容层。
  这个文件非常重要，因为这个包是“一套源码同时支持 ROS1 和 ROS2”。两边 API 不一样，所以这里用宏和适配代码把差异包起来，减少主节点代码重复。

- `nodelet_imagenet.cpp`
  ROS1 的 nodelet 版本 `imagenet`。
  这不是 ROS2 用的，是给 ROS1 nodelet 机制准备的。它的目标是把分类节点作为 nodelet 插件运行，而不是普通独立进程节点。

## 这个 ROS 包和 `jetson-inference` 本体是什么关系

这是最容易搞混的一点。

这里的 `ros/` 并不自己实现核心推理网络。

真正做推理的是底层库，比如：

- `imageNet`
- `detectNet`
- `segNet`
- `videoSource`
- `videoOutput`

这些都来自：

- `jetson-inference`
- `jetson-utils`

而 `ros/` 做的是：

- 读取 ROS 参数
- 订阅/发布 ROS topic
- 调用底层网络接口
- 把结果包装成 ROS 消息

你可以理解成：

- `jetson-inference` = 算法和推理库
- `ros_deep_learning` = ROS 封装层

## 这个包支持 ROS1 和 ROS2 吗

支持，而且这个目录就是按“同一套源码兼容两边”来写的。

从文件结构就能看出来：

- `package.xml` / `package.ros1.xml`
- `package.ros2.xml`
- `*.ros1.launch`
- `*.ros2.launch`
- `ros_compat.*`

`CMakeLists.txt` 里也会先判断当前找到的是 `catkin` 还是 `ament_cmake`，再走不同构建分支：

- ROS1 走 `catkin`
- ROS2 走 `ament_cmake`

## 新手最常见的理解方式

如果你刚开始学 ROS，可以这样理解这几个节点：

### `video_source`

像一个“摄像头/视频采集器”。

它负责把外部视频源变成 ROS 图像 topic。

### `imagenet`

像一个“图像分类处理器”。

它拿到图像后，输出：

- 这是哪一类
- 置信度是多少
- 一张带叠加文字的图

### `detectnet`

像一个“目标检测处理器”。

它拿到图像后，输出：

- 检测框
- 类别
- 置信度
- 一张带框的叠加图

### `segnet`

像一个“语义分割处理器”。

它拿到图像后，输出：

- 每个像素属于哪个类别
- 彩色分割图
- 叠加图

### `video_output`

像一个“显示器/录像器/流输出器”。

它把 ROS 图像 topic 再变成你能直接看到或保存的输出。

## 最常见的运行方式

### 1. 先测试纯视频链路

这一步最适合排查：

- 摄像头能不能打开
- 视频输入 URI 对不对
- ROS 图像链路是否正常

ROS1：

```bash
roslaunch ros_deep_learning video_viewer.ros1.launch input:=csi://0 output:=display://0
```

ROS2：

```bash
ros2 launch ros_deep_learning video_viewer.ros2.launch input:=csi://0 output:=display://0
```

### 2. 再测试分类

ROS1：

```bash
roslaunch ros_deep_learning imagenet.ros1.launch input:=csi://0 output:=display://0
```

ROS2：

```bash
ros2 launch ros_deep_learning imagenet.ros2.launch input:=csi://0 output:=display://0
```

### 3. 再测试检测

ROS1：

```bash
roslaunch ros_deep_learning detectnet.ros1.launch input:=csi://0 output:=display://0
```

ROS2：

```bash
ros2 launch ros_deep_learning detectnet.ros2.launch input:=csi://0 output:=display://0
```

### 4. 最后测试分割

ROS1：

```bash
roslaunch ros_deep_learning segnet.ros1.launch input:=csi://0 output:=display://0
```

ROS2：

```bash
ros2 launch ros_deep_learning segnet.ros2.launch input:=csi://0 output:=display://0
```

## launch 文件里你应该重点看什么

对于初学者，launch 文件里最值得先看的是三类内容：

### 1. 输入输出参数

例如：

- `input:=csi://0`
- `output:=display://0`

这决定视频从哪里来、到哪里去。

### 2. topic 重映射

例如 `imagenet` launch 里会把：

- `/imagenet/image_in`

重映射到：

- `/video_source/raw`

这表示：

- `video_source` 发布原始图像
- `imagenet` 从这个 topic 读取图像

### 3. 模型参数

例如：

- `model_name`
- `model_path`
- `class_labels_path`
- `threshold`

这些参数决定你用哪个模型，以及模型输出时的行为。

## 这些 topic 和参数是做什么的

原英文 README 已经把 topic 和参数表列得很全，这里只说最核心的理解方式。

### `imagenet`

主要输入输出：

- 输入 `image_in`
- 输出 `classification`
- 输出 `vision_info`
- 输出 `overlay`

理解方式：

- `classification` 是结构化分类结果
- `overlay` 是给人看的可视化图像
- `vision_info` 是标签表等辅助信息

### `detectnet`

主要输入输出：

- 输入 `image_in`
- 输出 `detections`
- 输出 `vision_info`
- 输出 `overlay`

理解方式：

- `detections` 给程序消费
- `overlay` 给人观察

### `segnet`

主要输入输出：

- 输入 `image_in`
- 输出 `vision_info`
- 输出 `overlay`
- 输出 `color_mask`
- 输出 `class_mask`

理解方式：

- `color_mask` 更适合人看
- `class_mask` 更适合程序后处理

## 为什么还会有 nodelet

`nodelet_imagenet.cpp` 只在 ROS1 里有意义。

Nodelet 的核心目的，是让多个模块在同一个进程里共享数据，减少图像在节点之间复制带来的开销。对图像推理这类大数据量任务来说，这样做可能更高效。

如果你刚入门 ROS，不用先纠结 nodelet。先把普通节点模式跑通更重要。

## 初学者建议的阅读顺序

建议按这个顺序看代码：

1. 先看这份 `README-CN.md`
2. 再看 `launch/video_viewer.*.launch`
3. 再看 `launch/imagenet.*.launch`
4. 再看 `src/node_video_source.cpp`
5. 再看 `src/node_imagenet.cpp`
6. 然后看 `src/node_detectnet.cpp`
7. 最后再看 `src/node_segnet.cpp`

这样会比较容易建立整体概念：

- 视频怎么进来
- 节点怎么接图像
- 推理怎么做
- 结果怎么发出去

## 原 README 重点在讲什么

原英文 `README.md` 主要讲了这些内容：

- 这个包支持 ROS 和 ROS2
- 可以跑分类、检测、分割和视频流节点
- 推荐通过 `jetson-inference` 的容器环境启动
- 给出了常见 demo 的启动命令
- 列出了各节点的 topic 和参数

所以如果你要：

- 快速理解目录结构，看这份中文文档
- 查完整参数表和英文原始说明，再看原 `README.md`

## 最后给初学者一句话总结

如果你现在还不熟 ROS，不要一开始就钻 `CMakeLists.txt` 或 `ros_compat.*`。

先记住这件事就够了：

- `video_source` 负责“进图”
- `imagenet` / `detectnet` / `segnet` 负责“做推理”
- `video_output` 负责“出图”

把这条链路先看懂，这个目录就不会乱了。
