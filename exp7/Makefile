# Makefile for macOS

# 编译器
CXX = g++

# 编译选项
# -Wno-deprecated-declarations: 忽略macOS上OpenGL/GLUT被标记为废弃的警告
CXXFLAGS = -std=c++11 -Wall -Wno-deprecated-declarations

# 链接选项 (macOS 必须使用 -framework)
LDFLAGS = -framework OpenGL -framework GLUT

# 目标文件
TARGET = bspline
SRC = bspline.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)