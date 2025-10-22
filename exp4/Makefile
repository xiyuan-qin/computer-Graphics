# 编译器
CXX = clang++

# 编译选项
# -std=c++11: 使用 C++11 标准
# -Wall: 显示所有警告
CXXFLAGS = -std=c++11 -Wall

# 链接选项 (macOS specific)
LDFLAGS = -framework OpenGL -framework GLUT

# 目标可执行文件名
TARGET = cornu_spiral

# 源文件列表
SOURCES = main.cpp Spiral.cpp Vector.cpp

# 根据源文件自动生成目标文件列表 (.o 文件)
OBJECTS = $(SOURCES:.cpp=.o)

# --- 规则 ---

# 默认规则：all
# 当你只输入 'make' 时，会执行这个规则
all: $(TARGET)

# 链接规则：如何从目标文件生成最终的可执行文件
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# 编译规则：如何从 .cpp 源文件生成 .o 目标文件
# $< 代表第一个依赖项 (即 .cpp 文件)
# $@ 代表目标 (即 .o 文件)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理规则：删除所有生成的文件
clean:
	rm -f $(TARGET) $(OBJECTS)

# 伪目标：告诉 make 'all' 和 'clean' 不是文件名
.PHONY: all clean