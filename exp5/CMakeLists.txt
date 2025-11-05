# 1. 设置 CMake 的最低版本要求
cmake_minimum_required(VERSION 3.10)

# 2. 定义项目名称
project(BezierCurve VERSION 1.0)

# 3. 设置 C++ 标准 (我们的代码使用了 C++11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 4. 查找所需的库 (OpenGL 和 GLUT)
# "find_package" 是 CMake 的核心功能
# "REQUIRED" 表示如果找不到这些库, CMake 将会报错
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# 5. 添加可执行文件
# 告诉 CMake, "bezier" 这个目标是由 "bezier.cpp" 编译而来的
add_executable(bezier bezier.cpp)

# 6. 链接库
# 将找到的 OpenGL 和 GLUT 库链接到我们的 "bezier" 目标
# 这是最关键的跨平台步骤:
# - 在 macOS 上, CMake 知道这会转换为: -framework OpenGL -framework GLUT
# - 在 Linux 上, CMake 知道这会转换为: -lGL -lglut
# - 在 Windows 上, CMake 知道要链接 opengl32.lib 和 glut32.lib
target_link_libraries(bezier PRIVATE 
    OpenGL::GL    # 这是现代 CMake 推荐的 OpenGL 目标
    GLUT::GLUT    # 这是现代 CMake 推荐的 GLUT 目标
)

# (可选) 打印一些信息, 方便调试
message(STATUS "OpenGL libraries: ${OPENGL_LIBRARIES}")
message(STATUS "GLUT libraries: ${GLUT_LIBRARY}")