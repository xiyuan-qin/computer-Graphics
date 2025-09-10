# Makefile for macOS OpenGL/GLUT compilation
# [高级版] 可以同时编译三个目标文件

# 编译器
CXX = g++

# 编译参数
CXXFLAGS = -Wall -Wextra

# 链接参数 for macOS
LDFLAGS = -framework OpenGL -framework GLUT

# --- 目标 ---

# 定义我们想要生成的所有可执行文件
TARGETS = pyramid_viewer cube_viewer banana_viewer

# 默认规则: 如果只输入 `make`, 就编译所有的目标
all: $(TARGETS)

# --- 规则 ---

# 如何生成 pyramid_viewer
pyramid_viewer: pyramid.o
	$(CXX) $< -o $@ $(LDFLAGS)
	@echo "编译完成 -> pyramid_viewer"

# 如何生成 cube_viewer
cube_viewer: cube.o
	$(CXX) $< -o $@ $(LDFLAGS)
	@echo "编译完成 -> cube_viewer"

# 如何生成 banana_viewer
banana_viewer: banana.o
	$(CXX) $< -o $@ $(LDFLAGS)
	@echo "编译完成 -> banana_viewer"

# 通用编译规则: 如何从 .cpp 文件生成 .o 文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


# --- 其他命令 ---

# 清理规则: 删除所有生成的文件
clean:
	@echo "正在清理..."
	rm -f $(TARGETS) *.o

# 运行规则: 增加了独立的运行命令
run_pyramid: pyramid_viewer
	@echo "--- 运行 Pyramid Viewer ---"
	./pyramid_viewer

run_cube: cube_viewer
	@echo "--- 运行 Cube Viewer ---"
	./cube_viewer

run_banana: banana_viewer
	@echo "--- 运行 Banana Viewer ---"
	./banana_viewer


# .PHONY 告诉 make, all 和 clean 不是真实的文件名
.PHONY: all clean run_pyramid run_cube run_banana