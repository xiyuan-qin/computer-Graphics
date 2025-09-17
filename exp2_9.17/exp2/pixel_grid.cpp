#include <iostream>
#include <vector>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <glm/glm.hpp>

// --- 网格配置 ---
int windowWidth = 800;
int windowHeight = 800;
const int NUM_COLS = 20; // 20列
const int NUM_ROWS = 20; // 20行
const float LINE_WIDTH = 2.0f; // 线条粗细

// --- 状态管理 ---
// 使用 glm::ivec2 存储选中的格子坐标 (列, 行)
// 初始化为 (-1, -1) 表示没有格子被选中
glm::ivec2 selectedCell(-1, -1);

// --- 函数声明 ---
void display();
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Interactive Pixel Grid");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);

    glutMainLoop();
    return 0;
}

void reshape(int w, int h)
{
    // 当窗口大小改变时，更新全局变量
    windowWidth = w;
    windowHeight = h;
    // 设置视口为整个窗口
    glViewport(0, 0, w, h);

    // 设置正交投影
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 这里的投影矩阵将坐标系设置为左下角是(0,0)，右上角是(w,h)
    // 这使得窗口像素坐标和OpenGL世界坐标一一对应
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display()
{
    // --- 1. 清屏 ---
    // 使用一个浅灰色作为背景，以和白色格子区分
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 计算每个格子的宽高
    float cellWidth = (float)windowWidth / NUM_COLS;
    float cellHeight = (float)windowHeight / NUM_ROWS;

    // --- 2. 绘制所有格子的白色背景 ---
    glColor3f(1.0f, 1.0f, 1.0f); // 设置颜色为白色
    glRectf(0.0f, 0.0f, windowWidth, windowHeight); // 画一个覆盖全屏的矩形

    // --- 3. 绘制被选中的红色格子 ---
    // 检查 selectedCell 的坐标是否有效
    if (selectedCell.x >= 0 && selectedCell.y >= 0)
    {
        // 计算红色格子的左下角坐标
        float x_start = selectedCell.x * cellWidth;
        float y_start = selectedCell.y * cellHeight;
        
        glColor3f(1.0f, 0.0f, 0.0f); // 设置颜色为红色
        glRectf(x_start, y_start, x_start + cellWidth, y_start + cellHeight);
    }

    // --- 4. 绘制网格线 ---
    glColor3f(0.0f, 0.0f, 0.0f); // 设置颜色为黑色
    glLineWidth(LINE_WIDTH);     // 设置线宽

    glBegin(GL_LINES);
    // 绘制所有竖直线
    for (int i = 0; i <= NUM_COLS; ++i)
    {
        float x = i * cellWidth;
        glVertex2f(x, 0);
        glVertex2f(x, windowHeight);
    }
    // 绘制所有水平线
    for (int i = 0; i <= NUM_ROWS; ++i)
    {
        float y = i * cellHeight;
        glVertex2f(0, y);
        glVertex2f(windowWidth, y);
    }
    glEnd();

    // --- 5. 交换缓冲区 ---
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
    // 我们只关心鼠标左键按下的事件
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // 计算每个格子的宽高
        float cellWidth = (float)windowWidth / NUM_COLS;
        float cellHeight = (float)windowHeight / NUM_ROWS;
        
        // --- 坐标转换 ---
        // 将鼠标的窗口坐标 (x, y) 转换为网格坐标 (col, row)
        // x -> col: 直接除以格子宽度
        int col = x / cellWidth;
        
        // y -> row: GLUT的y坐标原点在左上角，而我们的OpenGL坐标原点在左下角
        // 所以需要用窗口高度减去y，再进行计算
        int row = (windowHeight - y) / cellHeight;
        
        // 检查点击是否在有效范围内
        if (col >= 0 && col < NUM_COLS && row >= 0 && row < NUM_ROWS)
        {
            // 更新选中的格子
            selectedCell.x = col;
            selectedCell.y = row;
            
            // 打印信息到控制台，方便调试
            std::cout << "Clicked Cell: (" << col << ", " << row << ")" << std::endl;
            
            // 请求重绘窗口，这样display()函数就会被调用，颜色变化会显示出来
            glutPostRedisplay();
        }
    }
}