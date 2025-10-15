# 编译器和编译选项
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# 链接选项 (macOS specific)
LDFLAGS = -framework OpenGL -framework GLUT

# 目标文件名
TARGET = scanline_app

# 源文件和目标文件
SOURCES = main.cpp renderer.cpp scanline.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# 默认目标
all: $(TARGET)

# 链接规则
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# 编译规则 (将 .cpp 编译为 .o)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理规则
clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean