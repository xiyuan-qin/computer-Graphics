#include "renderer.h"
#include "clipping.h" // 包含头文件以获取声明
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <vector>


void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0); // 设置清屏颜色为白色
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // 1. 绘制裁剪窗口
    glColor3f(0.0f, 0.0f, 0.0f); // 黑色
    glBegin(GL_LINE_LOOP);
    glVertex2d(X_MIN, Y_MIN);
    glVertex2d(X_MAX, Y_MIN);
    glVertex2d(X_MAX, Y_MAX);
    glVertex2d(X_MIN, Y_MAX);
    glEnd();

    // 2. 定义几条待裁剪的线
    std::vector<std::vector<double>> lines = {
        {50, 50, 750, 550},     // 完全穿过
        {200, 200, 600, 400},   // 完全在内
        {50, 250, 100, 350},    // 完全在外
        {300, 50, 500, 500}     // 一端在内，一端在外
    };

    for (const auto& line : lines) {
        double x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];

        // 绘制原始线段 (红色虚线)
        glColor3f(1.0f, 0.0f, 0.0f);
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0x0F0F); // 设置虚线样式
        glBegin(GL_LINES);
        glVertex2d(x1, y1);
        glVertex2d(x2, y2);
        glEnd();
        glDisable(GL_LINE_STIPPLE);

        // 调用裁剪算法
        if (cohenSutherlandClip(x1, y1, x2, y2)) {
            // 如果可见，绘制裁剪后的线段 (绿色实线)
            glColor3f(0.0f, 1.0f, 0.0f);
            glBegin(GL_LINES);
            glVertex2d(x1, y1);
            glVertex2d(x2, y2);
            glEnd();
        }
    }
    
    glutSwapBuffers();
}