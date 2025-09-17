# 编译器 (明确使用 clang++)
CXX = clang++

# 编译选项
# -std=c++11: 使用 C++11 标准
# -O2: 优化级别
# -DGL_SILENCE_DEPRECATION: 消除 macOS 上的 OpenGL 废弃警告
CXXFLAGS = -std=c++11 -O2 -DGL_SILENCE_DEPRECATION

# 目标可执行文件名
TARGET = arcball_glut

# 源文件
SRCS = main.cpp

# 头文件搜索路径
ifeq ($(shell uname -m), arm64)
	HOMEBREW_PREFIX = /opt/homebrew
else
	HOMEBREW_PREFIX = /usr/local
endif
INCLUDES = -I$(HOMEBREW_PREFIX)/include

# 链接库和框架
LDFLAGS = -framework OpenGL -framework GLUT

# 默认目标
all: $(TARGET)

# 编译规则
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(SRCS) $(LDFLAGS)

# 清理命令
clean:
	rm -f $(TARGET)

# 声明伪目标
.PHONY: all clean