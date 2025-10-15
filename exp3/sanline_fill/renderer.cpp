#include "renderer.h"
#include "scanline.h"
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <vector>

// 定义一个多边形用于测试
static std::vector<Point> polygon = { {100, 150}, {300, 450}, {500, 400}, {650, 200}, {400, 50} };

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0); // 设置清屏颜色为白色
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT); // 设置二维正交投影
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // 绘制多边形轮廓（用于对照）
    glColor3f(0.0f, 0.0f, 0.0f); // 黑色
    glBegin(GL_LINE_LOOP);
    for (const auto& p : polygon) {
        glVertex2i(p.x, p.y);
    }
    glEnd();

    // 调用扫描线算法填充多边形
    scanlineFill(polygon, WINDOW_HEIGHT);

    glutSwapBuffers();
}