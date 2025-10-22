#include <iostream>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>

#include "Spiral.h" // 引入我们的螺旋线模块

// --- 新增的全局变量，用于处理鼠标拖动 ---

// 视图偏移量 (平移)
float g_view_offset_x = 0.0f;
float g_view_offset_y = 0.0f;

// 鼠标状态
bool  g_mouse_down = false;
int   g_mouse_x = 0;
int   g_mouse_y = 0;

// 窗口尺寸
int   g_window_width = 800;
int   g_window_height = 800;

// 投影区域尺寸 (由 reshape 函数更新)
float g_ortho_width = 2.4f;
float g_ortho_height = 2.4f;


// --- 新增的回调函数 ---

// 鼠标按键回调函数
void mouseCallback(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            g_mouse_down = true;
            g_mouse_x = x;
            g_mouse_y = y;
        } else if (state == GLUT_UP) {
            g_mouse_down = false;
        }
    }
}

// 鼠标拖动回调函数
void motionCallback(int x, int y) {
    if (g_mouse_down) {
        // 1. 计算鼠标在屏幕上移动的像素距离
        int dx = x - g_mouse_x;
        int dy = y - g_mouse_y;

        // 2. 将像素距离转换为 OpenGL 世界坐标的距离
        // (屏幕像素 / 屏幕总像素) * 世界坐标总宽度 = 世界坐标偏移量
        float delta_vx = (float)dx / g_window_width * g_ortho_width;
        float delta_vy = (float)dy / g_window_height * g_ortho_height;

        // 3. 更新视图偏移量
        // 因为屏幕坐标 Y 轴向下，而 OpenGL 坐标 Y 轴向上，所以 dy 需要反向
        g_view_offset_x += delta_vx;
        g_view_offset_y -= delta_vy;

        // 4. 更新上次的鼠标位置
        g_mouse_x = x;
        g_mouse_y = y;

        // 5. 通知 GLUT 重绘窗口
        glutPostRedisplay();
    }
}


// --- 修改后的核心回调函数 ---

// 窗口大小改变时的回调函数 (已修改)
void reshape(int width, int height) {
    // 更新全局窗口尺寸变量
    g_window_width = width;
    g_window_height = height > 0 ? height : 1; // 防止除以零

    glViewport(0, 0, g_window_width, g_window_height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // 保证横纵比，防止拉伸变形
    float aspect = (float)g_window_width / g_window_height;
    if (g_window_width >= g_window_height) {
        g_ortho_height = 2.4f;
        g_ortho_width = g_ortho_height * aspect;
    } else {
        g_ortho_width = 2.4f;
        g_ortho_height = g_ortho_width / aspect;
    }
    gluOrtho2D(-g_ortho_width / 2, g_ortho_width / 2, -g_ortho_height / 2, g_ortho_height / 2);

    glMatrixMode(GL_MODELVIEW);
}

// 绘制函数 (已修改)
void display() {
    // 清除颜色缓冲区
    glClear(GL_COLOR_BUFFER_BIT);
    
    // 在绘制任何物体之前，先重置模型视图矩阵
    glLoadIdentity();

    // 应用鼠标拖动产生的平移变换
    glTranslatef(g_view_offset_x, g_view_offset_y, 0.0f);

    // --- 绘制内容 (与之前相同) ---
    // 绘制坐标轴 (灰色)
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
        glVertex2f(-100.0f, 0.0f); glVertex2f(100.0f, 0.0f); // 绘制更长的轴线以适应拖动
        glVertex2f(0.0f, -100.0f); glVertex2f(0.0f, 100.0f);
    glEnd();

    // 绘制 s > 0 的部分 (红色)
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
    for (const auto& p : spiralPointsPositive) {
        glVertex2f(p.x, p.y);
    }
    glEnd();

    // 绘制 s < 0 的部分 (蓝色)
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINE_STRIP);
    for (const auto& p : spiralPointsNegative) {
        glVertex2f(p.x, p.y);
    }
    glEnd();
    
    glutSwapBuffers();
}


// 主函数 (已修改)
int main(int argc, char** argv) {
    std::cout << "Generating Cornu Spiral points..." << std::endl;
    generateCornuSpiral();
    std::cout << "Point generation complete. Starting renderer." << std::endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE); 
    glutInitWindowSize(g_window_width, g_window_height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Cornu Spiral Drawing (Draggable)");

    // 注册回调函数 (新增了鼠标和拖动回调)
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(motionCallback);

    // 设置背景色为白色 (已修改)
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glutMainLoop();

    return 0;
}