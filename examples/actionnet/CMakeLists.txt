
# 收集当前目录下的 C++ 源文件。
# 这里这个示例目录只有 actionnet.cpp，所以最后基本就是把它编成一个可执行程序。
file(GLOB actionnetSources *.cpp)

# 收集头文件。
# 这个变量当前没有继续往下使用，更多是保持和其它示例目录写法一致。
file(GLOB actionnetIncludes *.h )

# 生成名为 actionnet 的可执行文件。
# 这里用的是 cuda_add_executable，因为这个项目整体是按 CUDA/TensorRT 程序来构建的。
cuda_add_executable(actionnet ${actionnetSources})

# 链接项目核心库 jetson-inference。
# actionnet.cpp 里真正用到的推理功能、网络封装等实现都在这个库里，不在这个 CMake 文件里。
target_link_libraries(actionnet jetson-inference)

# 安装到 bin 目录。
# 这样在 make install 之后，actionnet 可执行文件会被放到统一的 bin 路径下。
install(TARGETS actionnet DESTINATION bin)
