# 先清掉旧的 Python 相关缓存变量。
# `unset(xxx CACHE)` 的意思是：把 CMakeCache.txt 里记住的旧值删除掉。
# 这样做是为了避免你之前配置过别的 Python 版本/路径，导致这次构建继续误用旧配置。
# 这里的 `CACHE` 不是普通变量，而是 CMake 配置阶段持久化保存的缓存项。
# --- Clear legacy Python cache variables (avoid stale cache conflicts) ---
unset(PYTHONINTERP_FOUND CACHE)
unset(PYTHON_EXECUTABLE CACHE)
unset(PYTHON_VERSION_STRING CACHE)
unset(PYTHON_VERSION_MAJOR CACHE)
unset(PYTHON_VERSION_MINOR CACHE)
unset(PYTHON_VERSION_PATCH CACHE)

unset(PYTHON_INCLUDE_PATH CACHE)
unset(PYTHON_INCLUDE_DIRS CACHE)
unset(PYTHON_INCLUDE_DIR CACHE)
unset(PYTHON_LIBRARY CACHE)
unset(PYTHON_LIBRARIES CACHE)
unset(PYTHON_DEBUG_LIBRARIES CACHE)
unset(PYTHON_MODULE_PREFIX CACHE)
unset(PYTHON_MODULE_EXTENSION CACHE)

unset(PYTHONLIBS_FOUND CACHE)
unset(PYTHONLIBS_VERSION_STRING CACHE)

# `message(...)` 会在执行 CMake 配置时打印一行日志。
# 这里用来提示当前正在查找哪个 Python 版本的绑定环境。
message("-- detecting Python ${PYTHON_BINDING_VERSION}...")

# `find_package(...)` 是 CMake 里非常核心的命令，用来查找外部依赖。
# 下面优先走“现代写法”：
# 1. `Python3` 表示查找 Python3 包
# 2. `${PYTHON_BINDING_VERSION}` 表示期望的版本号
# 3. `QUIET` 表示查找失败时先不要直接报错退出
# 4. `COMPONENTS Interpreter Development NumPy` 表示分别查找：
#    - Interpreter: Python 解释器
#    - Development: 头文件和开发库
#    - NumPy: NumPy 的头文件/版本信息
# --- Modern path (CMake >= 3.18): FindPython3 with Interpreter/Development/NumPy ---
find_package(Python3 ${PYTHON_BINDING_VERSION} QUIET COMPONENTS Interpreter Development NumPy)

# `if(A AND B)` 是条件判断语法，两个条件都成立才进入。
# 这里只要解释器和开发库都找到，就继续配置 Python 绑定。
if(Python3_Interpreter_FOUND AND Python3_Development_FOUND)
  # 这里把现代 FindPython3 查到的变量，统一映射成旧变量名。
  # 作用是让下面的构建逻辑不用区分“现代路径”还是“旧路径”，脚本更容易复用。
  set(PYTHON_VERSION_MAJOR ${Python3_VERSION_MAJOR})
  set(PYTHON_VERSION_MINOR ${Python3_VERSION_MINOR})
  set(PYTHONLIBS_VERSION_STRING ${Python3_VERSION})
  set(PYTHON_INCLUDE_DIRS ${Python3_INCLUDE_DIRS})

  # `Python3::Python` 是 CMake 导入目标（imported target）。
  # 比直接写某个 `.so` / `.lib` 路径更稳，因为它会携带正确的链接信息。
  set(PYTHON_LIBRARIES Python3::Python)

  message("-- found Python version:  ${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR} (${PYTHONLIBS_VERSION_STRING})")
  message("-- found Python include:  ${PYTHON_INCLUDE_DIRS}")
  message("-- using Python library target:  Python3::Python")

  # 先默认认为没找到 NumPy。
  # `set(NUMPY_FOUND OFF)` 就是把变量设成假值。
  # NumPy (modern)
  set(NUMPY_FOUND OFF)
  if(Python3_NumPy_FOUND)
    set(NUMPY_FOUND ON)
    set(NUMPY_VERSION ${Python3_NumPy_VERSION})
    set(NUMPY_INCLUDE_DIR ${Python3_NumPy_INCLUDE_DIRS})
    message("-- found NumPy version:  ${NUMPY_VERSION}")
    message("-- found NumPy include:  ${NUMPY_INCLUDE_DIR}")

    # `add_definitions(-DHAS_NUMPY)` 会给编译器增加一个宏定义 `HAS_NUMPY`。
    # C/C++ 代码里就可以用 `#ifdef HAS_NUMPY` 判断是否启用 NumPy 相关代码。
    add_definitions(-DHAS_NUMPY)
  else()
    message("-- NumPy not found (modern path)")
  endif()

else()
  # 如果现代写法失败，就退回老写法。
  # 这是兼容旧版 CMake 或旧系统环境的兜底逻辑。
  # --- Legacy fallback (older CMake / systems) ---
  find_package(PythonInterp ${PYTHON_BINDING_VERSION} QUIET)
  find_package(PythonLibs  ${PYTHON_BINDING_VERSION} QUIET)

  # `NOT` 表示取反。
  # 如果连 Python 开发库都没找到，后面就没法编译 Python 扩展模块，所以直接返回。
  if(NOT PYTHONLIBS_FOUND)
    message("-- Python ${PYTHON_BINDING_VERSION} wasn't found")
    return()
  endif()

  message("-- found Python version:  ${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR} (${PYTHONLIBS_VERSION_STRING})")
  message("-- found Python include:  ${PYTHON_INCLUDE_DIRS}")
  message("-- found Python library:  ${PYTHON_LIBRARIES}")

  # `CMAKE_MODULE_PATH` 是 CMake 查找自定义模块（例如 FindXXX.cmake）时会搜索的目录列表。
  # 这里把额外目录追加进去，这样 `find_package(NumPy)` 时就能找到你项目里的自定义 `FindNumPy.cmake`。
  # Extend module path to pick up your custom FindNumPy.cmake if present
  set(CMAKE_MODULE_PATH
      ${CMAKE_MODULE_PATH}
      "${CMAKE_CURRENT_SOURCE_DIR}"
      "${CMAKE_CURRENT_SOURCE_DIR}/../../utils/python/bindings"
  )
  message("-- CMake module path:  ${CMAKE_MODULE_PATH}")

  find_package(NumPy QUIET)

  if(NUMPY_FOUND)
    message("-- found NumPy version:  ${NUMPY_VERSION}")
    message("-- found NumPy include:  ${NUMPY_INCLUDE_DIR}")

    # 同样给编译器增加 `HAS_NUMPY` 宏，供 C/C++ 代码做条件编译。
    add_definitions(-DHAS_NUMPY)
  else()
    message("-- NumPy not found (legacy path)")
  endif()
endif()

# `file(GLOB ...)` 会把当前目录下所有 `*.cpp` 文件收集到变量里。
# 好处是新增源码文件时通常不用手动再改这里。
# 缺点是某些 CMake 场景下，对“文件新增/删除”的自动感知不如手写列表明确。
# --- Sources & target ---
file(GLOB pythonExtSources *.cpp)

# 定义一个中间变量 `_PYMOD` 作为目标名。
# 最终名字会拼上 Python 主版本号和次版本号，例如 `jetson-inference-python-310`。
set(_PYMOD jetson-inference-python-${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR})

# `cuda_add_library(... SHARED ...)` 表示生成一个共享库。
# Python 的原生扩展模块本质上就是一个动态库，所以这里不是生成可执行文件，而是生成库文件。
# `${pythonExtSources}` 就是前面收集到的所有 `.cpp` 源文件。
cuda_add_library(${_PYMOD} SHARED ${pythonExtSources})

# `target_include_directories` 给某个目标单独添加头文件目录。
# `PRIVATE` 的意思是：这些头文件目录只给当前目标自己编译时使用，不向依赖它的其他目标传播。
# 这比全局 `include_directories(...)` 更清晰，也更不容易污染其他目标。
# Per-target includes and links (avoid global include/link directories)
target_include_directories(${_PYMOD} PRIVATE ${PYTHON_INCLUDE_DIRS})

# `target_link_libraries` 指定当前目标要链接哪些库。
# 这里会链接：
# 1. `jetson-inference` 主库
# 2. Python 开发库/导入目标
# 3. 对应 Python 版本的 `jetson-utils-python` 库
target_link_libraries(${_PYMOD}
  PRIVATE
    jetson-inference
    ${PYTHON_LIBRARIES}
    jetson-utils-python-${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}
)

# 如果找到了 NumPy，就把它的头文件目录也加进来。
# 这样 C/C++ 代码里才能正常包含 NumPy 的 C API 头文件。
# Add NumPy include directories if available
if(NUMPY_FOUND)
  target_include_directories(${_PYMOD} PRIVATE ${NUMPY_INCLUDE_DIR})

  # `DEFINED NUMPY_VERSION` 表示先确认变量存在。
  # `VERSION_LESS "2.0"` 是按版本号语义比较，而不是纯字符串比较。
  # 这里的逻辑是：只有 NumPy 2.0 之前，才尝试额外链接 `npymath`。
  # Only for NumPy < 2.0: attempt to link npymath if it exists
  if(DEFINED NUMPY_VERSION AND NUMPY_VERSION VERSION_LESS "2.0")

    # `find_library(...)` 会在系统库路径里查找名为 `npymath` 的库，并把结果写进 `NPYMATH_LIB`。
    find_library(NPYMATH_LIB NAMES npymath)
    if(NPYMATH_LIB)
      target_link_libraries(${_PYMOD} PRIVATE ${NPYMATH_LIB})
      message(STATUS "NumPy ${NUMPY_VERSION} (< 2.0): linking npymath -> ${NPYMATH_LIB}")
    else()
      message(WARNING "NumPy ${NUMPY_VERSION} (< 2.0) but 'npymath' not found: continuing without it")
    endif()
  else()
    message(STATUS "NumPy ${NUMPY_VERSION} (>= 2.0): skipping npymath linkage")
  endif()
endif()

# `set_target_properties` 用来设置目标的输出属性。
# 这里最关键的几个属性：
# 1. `PREFIX ""`：不要默认库前缀（例如 Linux 下常见的 `lib`）
# 2. `OUTPUT_NAME "jetson_inference_python"`：实际输出文件名
# 3. `LIBRARY_OUTPUT_DIRECTORY ...`：把生成的动态库放到指定目录
set_target_properties(${_PYMOD}
  PROPERTIES
    PREFIX ""
    OUTPUT_NAME "jetson_inference_python"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/python/${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}"
)

# 安装路径按平台区分：
# 1. 如果是 x86_64，装到 conda 风格的 site-packages
# 2. 否则默认按 Jetson / Debian 风格装到 dist-packages
# 这样 `make install` 后，Python 才能直接 `import` 到这个模块。
# --- Install paths (x86_64 vs Jetson) ---
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  set(PYTHON_BINDING_INSTALL_DIR /opt/conda/lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)
else()
  set(PYTHON_BINDING_INSTALL_DIR /usr/lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/dist-packages)
endif()

# `install(TARGETS ...)` 表示安装刚才编译出的动态库到目标目录。
install(TARGETS ${_PYMOD} DESTINATION ${PYTHON_BINDING_INSTALL_DIR})

# `install(DIRECTORY ...)` 表示把整个 Python 包目录一起复制到安装目录。
# 这些目录里通常是纯 Python 文件，和上面的二进制扩展模块一起组成完整包。
install(DIRECTORY ../python/Jetson           DESTINATION ${PYTHON_BINDING_INSTALL_DIR})
install(DIRECTORY ../python/jetson           DESTINATION ${PYTHON_BINDING_INSTALL_DIR})
install(DIRECTORY ../python/jetson_inference DESTINATION ${PYTHON_BINDING_INSTALL_DIR})
