#ifndef RENDERER_H
#define RENDERER_H

// 窗口尺寸
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// 初始化 OpenGL 环境
void init();

// GLUT 的显示回调函数
void display();

#endif // RENDERER_H