#include "renderer.h"
#include "clipping.h"
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <vector>

// 定义一个静态的裁剪窗口
static ClipWindow clipWindow = {150.0f, 650.0f, 150.0f, 450.0f};

// 定义一个待裁剪的多边形 (一个五边形)
static std::vector<Point> subjectPolygon = {
    {100, 300}, {300, 500}, {700, 400}, {600, 100}, {200, 50}
};

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // 1. 调用裁剪算法
    std::vector<Point> clippedPolygon = sutherlandHodgmanClip(subjectPolygon, clipWindow);

    // 2. 绘制裁剪窗口
    glColor3f(0.0f, 0.0f, 0.0f); // 黑色
    glBegin(GL_LINE_LOOP);
    glVertex2f(clipWindow.x_min, clipWindow.y_min);
    glVertex2f(clipWindow.x_max, clipWindow.y_min);
    glVertex2f(clipWindow.x_max, clipWindow.y_max);
    glVertex2f(clipWindow.x_min, clipWindow.y_max);
    glEnd();

    // 3. 绘制原始多边形轮廓 (红色虚线)
    glColor3f(1.0f, 0.0f, 0.0f);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x0F0F);
    glBegin(GL_LINE_LOOP);
    for (const auto& p : subjectPolygon) {
        glVertex2f(p.x, p.y);
    }
    glEnd();
    glDisable(GL_LINE_STIPPLE);

    // 4. 绘制裁剪后的多边形 (蓝色填充)
    if (!clippedPolygon.empty()) {
        glColor3f(0.0f, 0.5f, 0.8f); // 蓝色
        glBegin(GL_POLYGON);
        for (const auto& p : clippedPolygon) {
            glVertex2f(p.x, p.y);
        }
        glEnd();
    }

    glutSwapBuffers();
}