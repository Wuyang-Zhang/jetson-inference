<img src="https://github.com/dusty-nv/jetson-inference/raw/master/docs/images/deep-vision-header.jpg" width="100%">


# 构建nvcaffe


TX1 上使用了 caffe 的一个特殊分支，其中包括对 FP16 的支持。<br />
该代码发布在 NVIDIA 的 caffe 存储库的 Experimental/fp16 分支中，位于此处: 

> https://github.com/nvidia/caffe/tree/experimental/fp16


#### 1. 安装依赖项


`` 重击
$ sudo apt-get update -y
$ sudo apt-get install cmake -y


# 一般依赖关系
$ sudo apt-get install libprotobuf-dev libleveldb-dev libsnappy-dev \
libhdf5-serial-dev protobuf-编译器-y
$ sudo apt-get install --no-install-recommends libboost-all-dev -y


# 布拉斯
$ sudo apt-get install libatlas-base-dev -y


# 剩余的依赖项
$ sudo apt-get install libgflags-dev libgoogle-glog-dev liblmdb-dev -y


# Python 依赖项
$ sudo apt-get install python-dev python-numpy python-skimage python-protobuf -y
````


Snappy 包需要为 Caffe 创建一个符号链接才能正确链接: 


``` bash
$ sudo ln -s /usr/lib/libsnappy.so.1 /usr/lib/libsnappy.so
$ sudo ldconfig
```


#### 2. 克隆 nvcaffe fp16 分支


``` bash
$ git clone -b experimental/fp16 https://github.com/NVIDIA/caffe
```


这会将存储库检出到 Jetson 上名为 `caffe` 的本地目录。


#### 3. 设置构建选项


``` bash
$ cd caffe
$ cp Makefile.config.example Makefile.config
$ rm -rf cmake/ CMakeLists.txt
```


###### 启用 FP16: 


``` bash
$ sed -i 's/# NATIVE_FP16/NATIVE_FP16/g' Makefile.config
```


###### 启用 cuDNN: 


``` bash
$ sed -i 's/# USE_CUDNN/USE_CUDNN/g' Makefile.config
```


###### 启用compute_53/sm_53: 


``` bash 
$ sed -i 's/-gencode arch=compute_50,code=compute_50/-gencode arch=compute_53,code=sm_53 -gencode arch=compute_53,code=compute_53/g' Makefile.config
```


###### 设置头文件/链接器路径


`` 重击
    INCLUDE_DIRS := $(PYTHON_INCLUDE) /usr/local/include
    变成
    INCLUDE_DIRS := $(PYTHON_INCLUDE) /usr/local/include /usr/include/hdf5/serial/


LIBRARY_DIRS := $(PYTHON_LIB) /usr/local/lib /usr/lib
    变成
    LIBRARY_DIRS := $(PYTHON_LIB) /usr/local/lib /usr/lib /usr/lib/aarch64-linux-gnu/hdf5/serial/
````


###### 修复 HDF5 链接问题


``` bash
$ sudo ln -s /usr/lib/aarch64-linux-gnu/libhdf5_serial.so.10 /usr/lib/aarch64-linux-gnu/libhdf5.so
$ sudo ln -s /usr/lib/aarch64-linux-gnu/libhdf5_serial_hl.so.10 /usr/lib/aarch64-linux-gnu/libhdf5_hl.so
```


#### 4. 编译nvcaffe


``` bash
$ make -j4
$ make pycaffe
$ make distribute
```


#### 5. 设置环境路径


``` bash
# Edit the following lines in .bashrc and then source .bashrc
export PATH=/usr/local/cuda-8.0/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/home/ubuntu/caffe/build/tools
export PYTHONPATH=/home/ubuntu/caffe/python:$PYTHONPATH
```


#### 6. 安装 iPython（可选）


``` bash
$ sudo apt-get install ipython ipython-notebook python-pandas -y
```


#### 6. 测试 nvcaffe


``` bash
$ make runtest
```