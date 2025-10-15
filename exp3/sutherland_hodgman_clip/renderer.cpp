#include "renderer.h"
#include "clipping.h"
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <vector>
#include <string>

// --- 全局状态管理 ---

// 定义程序当前的操作状态
enum State {
    DEFINING_POLYGON, // 正在定义多边形
    IDLE,             // 空闲状态（定义完成）
    MOVING_POLYGON,   // 正在移动多边形
    MOVING_WINDOW     // 正在移动裁剪窗口
};

static State currentState = DEFINING_POLYGON;

// 定义裁剪窗口和待裁剪的多边形（现在是动态的）
static ClipWindow clipWindow = {150.0f, 650.0f, 150.0f, 450.0f};
static std::vector<Point> subjectPolygon;

// 记录上一次鼠标的位置，用于计算拖动位移
static Point lastMousePos;

// --- 辅助函数：在屏幕上绘制文字 ---
void drawText(float x, float y, const std::string& text) {
    glColor3f(0.2f, 0.2f, 0.2f); // 深灰色
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}


// --- GLUT 回调函数 ---

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // 1. 绘制裁剪窗口
    glColor3f(0.0f, 0.0f, 0.0f); // 黑色
    glBegin(GL_LINE_LOOP);
    glVertex2f(clipWindow.x_min, clipWindow.y_min);
    glVertex2f(clipWindow.x_max, clipWindow.y_min);
    glVertex2f(clipWindow.x_max, clipWindow.y_max);
    glVertex2f(clipWindow.x_min, clipWindow.y_max);
    glEnd();

    // 2. 绘制原始多边形
    if (!subjectPolygon.empty()) {
        glColor3f(1.0f, 0.0f, 0.0f); // 红色
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0x0F0F);
        // 如果正在定义，则不闭合；否则闭合
        glBegin(currentState == DEFINING_POLYGON ? GL_LINE_STRIP : GL_LINE_LOOP);
        for (const auto& p : subjectPolygon) {
            glVertex2f(p.x, p.y);
        }
        glEnd();
        glDisable(GL_LINE_STIPPLE);
    }

    // 3. 只有在定义完成后才进行裁剪和填充
    if (currentState != DEFINING_POLYGON && subjectPolygon.size() > 2) {
        std::vector<Point> clippedPolygon = sutherlandHodgmanClip(subjectPolygon, clipWindow);
        if (!clippedPolygon.empty()) {
            glColor3f(0.0f, 0.5f, 0.8f); // 蓝色
            glBegin(GL_POLYGON);
            for (const auto& p : clippedPolygon) {
                glVertex2f(p.x, p.y);
            }
            glEnd();
        }
    }
    
    // 4. 显示操作提示
    std::string statusText;
    switch (currentState) {
        case DEFINING_POLYGON:
            statusText = "模式: 定义多边形 (左键添加顶点, 右键完成)";
            break;
        case IDLE:
            statusText = "模式: 空闲";
            break;
        case MOVING_POLYGON:
            statusText = "模式: 移动多边形 (按住左键拖动)";
            break;
        case MOVING_WINDOW:
            statusText = "模式: 移动裁剪窗口 (按住左键拖动)";
            break;
    }
    drawText(10, WINDOW_HEIGHT - 25, statusText);
    drawText(10, 15, "按键: [c] 清空 | [p] 移动多边形 | [w] 移动窗口");


    glutSwapBuffers();
}

void mouseCallback(int button, int state, int x, int y) {
    // GLUT 的 y 坐标原点在左上角，需要转换为 OpenGL 的左下角坐标系
    int gl_y = WINDOW_HEIGHT - y;

    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            if (currentState == DEFINING_POLYGON) {
                subjectPolygon.push_back({(float)x, (float)gl_y});
            } else {
                lastMousePos = {(float)x, (float)gl_y};
            }
        } else if (button == GLUT_RIGHT_BUTTON) {
            if (currentState == DEFINING_POLYGON && subjectPolygon.size() > 2) {
                currentState = IDLE;
            }
        }
    }
    glutPostRedisplay(); // 请求重绘窗口
}

void motionCallback(int x, int y) {
    int gl_y = WINDOW_HEIGHT - y;
    float dx = x - lastMousePos.x;
    float dy = gl_y - lastMousePos.y;

    if (currentState == MOVING_POLYGON && !subjectPolygon.empty()) {
        for (auto& p : subjectPolygon) {
            p.x += dx;
            p.y += dy;
        }
    } else if (currentState == MOVING_WINDOW) {
        clipWindow.x_min += dx;
        clipWindow.x_max += dx;
        clipWindow.y_min += dy;
        clipWindow.y_max += dy;
    }

    lastMousePos = {(float)x, (float)gl_y};
    glutPostRedisplay(); // 请求重绘窗口
}

void keyboardCallback(unsigned char key, int x, int y) {
    switch (key) {
        case 'c': // Clear
            subjectPolygon.clear();
            currentState = DEFINING_POLYGON;
            break;
        case 'p': // Move Polygon
            if (!subjectPolygon.empty() && currentState != DEFINING_POLYGON) {
                currentState = MOVING_POLYGON;
            }
            break;
        case 'w': // Move Window
             if (!subjectPolygon.empty() && currentState != DEFINING_POLYGON) {
                currentState = MOVING_WINDOW;
            }
            break;
    }
    glutPostRedisplay(); // 请求重绘窗口
}