# Makefile for Pure C++ Polynomial Root Finder (macOS)

# 编译器
CXX = clang++

# 编译标志
# -std=c++17: 使用 C++17 标准
# -Wall: 开启所有警告
# -g: 添加调试信息
CXXFLAGS = -std=c++17 -Wall

# 目标可执行文件
TARGET = poly_cpp

# 源文件
SOURCES = main_cpp_only.cpp

# 头文件 (依赖项)
HEADERS = PolySolve.h

# 默认目标 (第一个目标)
all: $(TARGET)

# 链接规则：如何生成目标文件
$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

# "make clean" 用来删除生成的文件
clean:
	rm -f $(TARGET)

# 声明 "all" 和 "clean" 不是真实的文件名
.PHONY: all clean