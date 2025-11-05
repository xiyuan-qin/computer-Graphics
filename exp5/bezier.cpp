/*
 * 文件名: bezier.cpp
 * 编译 (macOS):
 * g++ bezier.cpp -o bezier -framework OpenGL -framework GLUT
 *
 * 功能:
 * - 使用鼠标左键在窗口中添加控制点.
 * - 自动绘制控制点（白色方块）和连接它们的控制多边形（灰色虚线）.
 * - 自动绘制n阶贝塞尔曲线（亮蓝色）.
 * - 使用右键清除所有点并重新开始.
 */

#include <iostream>
#include <vector>
#include <cmath>

// 在 macOS 上, 必须使用 <GLUT/glut.h>
#include <GLUT/glut.h>

// 窗口尺寸
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// 定义一个简单的2D点结构
struct Point
{
    float x;
    float y;
};

// 存储用户点击的控制点
std::vector<Point> controlPoints;

// --- 贝塞尔曲线数学函数 ---

// 1. 计算二项式系数 C(n, k)
// (n 选 k) = n! / (k! * (n-k)!)
long long binomialCoeff(int n, int k)
{
    if (k < 0 || k > n)
    {
        return 0;
    }
    if (k == 0 || k == n)
    {
        return 1;
    }
    if (k > n / 2)
    {
        k = n - k; // C(n, k) == C(n, n-k)
    }
    long long res = 1;
    for (int i = 1; i <= k; ++i)
    {
        res = res * (n - i + 1) / i;
    }
    return res;
}

// 2. 计算伯恩斯坦基函数 B(i, n, t)
double bernstein(int i, int n, double t)
{
    return binomialCoeff(n, i) * pow(t, i) * pow(1.0 - t, n - i);
}

// 3. 计算贝塞尔曲线上在参数 t 处的点
Point calculateBezierPoint(double t)
{
    Point p = {0.0f, 0.0f};
    int n = controlPoints.size() - 1; // n 阶
    if (n < 0)
        return p; // 没有点

    for (int i = 0; i <= n; ++i)
    {
        double b = bernstein(i, n, t);
        p.x += controlPoints[i].x * b;
        p.y += controlPoints[i].y * b;
    }
    return p;
}

// --- OpenGL 绘图函数 ---

void init()
{
    // 设置清除颜色为黑色
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // 设置2D正交投影
    // 将坐标系设置为 (0, WINDOW_WIDTH) 和 (0, WINDOW_HEIGHT)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // 1. 绘制控制点 (白色方块)
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(8.0f);
    glBegin(GL_POINTS);
    for (const auto &p : controlPoints)
    {
        glVertex2f(p.x, p.y);
    }
    glEnd();

    // 2. 绘制控制多边形 (灰色虚线)
    if (controlPoints.size() > 1)
    {
        glColor3f(0.5f, 0.5f, 0.5f);
        glLineStipple(1, 0x00FF); // 虚线模式
        glEnable(GL_LINE_STIPPLE);
        glBegin(GL_LINE_STRIP);
        for (const auto &p : controlPoints)
        {
            glVertex2f(p.x, p.y);
        }
        glEnd();
        glDisable(GL_LINE_STIPPLE);
    }

    // 3. 绘制贝塞尔曲线 (亮蓝色)
    // 至少需要2个点 (1阶曲线，即直线) 才能绘制
    if (controlPoints.size() > 1)
    {
        glColor3f(0.0f, 0.7f, 1.0f); // 亮蓝色
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
        // 采样 100 个点来近似曲线
        for (int i = 0; i <= 100; ++i)
        {
            double t = (double)i / 100.0;
            Point p = calculateBezierPoint(t);
            glVertex2f(p.x, p.y);
        }
        glEnd();
    }

    // 刷新缓冲区
    glutSwapBuffers(); // 使用双缓冲
}

// --- 交互处理函数 ---

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // 将屏幕坐标 (x, y) 转换为 OpenGL 坐标
        // GLUT 的 (0,0) 在左上角, OpenGL 的 (0,0) 在左下角
        Point newPoint = {(float)x, (float)(WINDOW_HEIGHT - y)};
        controlPoints.push_back(newPoint);

        std::cout << "添加控制点 " << controlPoints.size() << ": (" << newPoint.x << ", " << newPoint.y << ")" << std::endl;

        // 告诉 GLUT 窗口需要重新绘制
        glutPostRedisplay();
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        // 右键清除所有点
        controlPoints.clear();
        std::cout << "--- 已清除所有点 ---" << std::endl;
        glutPostRedisplay();
    }
}

// 当窗口大小改变时调用的函数
void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 保持 1:1 的像素坐标映射
    gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
    glMatrixMode(GL_MODELVIEW);
}

// --- Main 函数 ---

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // 启用双缓冲
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("n-Order Bezier Curve on macOS");

    init(); // 初始化 OpenGL 状态

    // 注册回调函数
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);

    std::cout << "在窗口中点击鼠标左键添加控制点." << std::endl;
    std::cout << "点击鼠标右键清除所有点." << std::endl;

    glutMainLoop(); // 进入主事件循环
    return 0;
}