# Makefile for OpenGL Polynomial Root Finder (macOS)

# 编译器
CXX = clang++

# 编译标志
CXXFLAGS = -std=c++17 -Wall

# 链接标志 (macOS 特有)
# -framework OpenGL: 链接 Apple 的 OpenGL 框架
# -framework GLUT: 链接 Apple 的 GLUT 框架
LDFLAGS = -framework OpenGL -framework GLUT

# 目标可执行文件
TARGET = poly_opengl

# 源文件
SOURCES = main_opengl.cpp

# 头文件 (依赖项)
HEADERS = PolySolve.h

# 默认目标
all: $(TARGET)

# 链接规则
$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

# "make clean" 用来删除生成的文件
clean:
	rm -f $(TARGET)

# 声明 "all" 和 "clean" 不是真实的文件名
.PHONY: all clean