#include "PolySolve.h"
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm> // for std::min/max

// 在 macOS 上包含 GLUT 的正确方式
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// --- 全局变量 ---
// 这些变量将由 main 函数中的用户输入来填充
std::vector<double> poly_coeffs;
std::vector<double> poly_roots;
double view_left;
double view_right;
double view_bottom;
double view_top;
// ---

// --- 辅助函数 (与 C++ 版本相同) ---

double getDouble(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        } else {
            std::cout << "输入无效，请输入一个数字。\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

int getDegree() {
    int degree;
    while (true) {
        std::cout << "请输入多项式的最高次幂 (例如: 3 表示 3 次方): ";
        if (std::cin >> degree && degree >= 0) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return degree;
        } else {
            std::cout << "输入无效，请输入一个非负整数。\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

void printPolynomial(const std::vector<double>& coeffs) {
    std::cout << "P(x) = ";
    for (int i = coeffs.size() - 1; i >= 0; --i) {
        if (std::abs(coeffs[i]) < EPSILON) continue;
        if (i < (int)coeffs.size() - 1) {
            std::cout << (coeffs[i] > 0 ? " + " : " - ");
            std::cout << std::abs(coeffs[i]);
        } else {
            std::cout << coeffs[i];
        }
        if (i > 0) {
            std::cout << "x";
            if (i > 1) {
                std::cout << "^" << i;
            }
        }
    }
    std::cout << "\n";
}

// --- OpenGL 回调函数 (与之前相同) ---

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // 1. 绘制坐标轴
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    glVertex2f(view_left, 0.0f);
    glVertex2f(view_right, 0.0f);
    glVertex2f(0.0f, view_bottom);
    glVertex2f(0.0f, view_top);
    glEnd();

    // 2. 绘制多项式函数曲线
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
    int steps = 200;
    for (int i = 0; i <= steps; ++i) {
        double x = view_left + (view_right - view_left) * i / (double)steps;
        double y = evaluate(poly_coeffs, x);
        glVertex2f(x, y);
    }
    glEnd();

    // 3. 绘制找到的根
    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(8.0f);
    glBegin(GL_POINTS);
    for (double root : poly_roots) {
        if (root >= view_left && root <= view_right) {
            glVertex2f(root, 0.0);
        }
    }
    glEnd();
    
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 使用全局变量设置 2D 投影
    gluOrtho2D(view_left, view_right, view_bottom, view_top);
    glMatrixMode(GL_MODELVIEW);
}

// --- 主函数 (已修改) ---

int main(int argc, char** argv) {
    std::cout << "--- 多项式求根 (OpenGL 可视化版本) ---\n";
    
    // --- 1. 在终端获取所有输入 ---
    int degree = getDegree();
    poly_coeffs.resize(degree + 1);
    
    std::cout << "请按照从低到高的次序输入系数 (c0, c1, ...):\n";
    for (int i = 0; i <= degree; ++i) {
        std::string prompt = "请输入 c" + std::to_string(i) + " (x^" + std::to_string(i) + " 的系数): ";
        poly_coeffs[i] = getDouble(prompt);
    }
    
    printPolynomial(poly_coeffs);

    view_left = getDouble("请输入查找区间的左边界 (left): ");
    while (true) {
        view_right = getDouble("请输入查找区间的右边界 (right): ");
        if (view_right > view_left) {
            break;
        }
        std::cout << "右边界必须大于左边界，请重新输入。\n";
    }

    // --- 2. 计算根 和 Y轴范围 ---
    std::cout << "\n正在计算根和视图范围...\n";
    
    // a. 计算根
    poly_roots = FindRoots(poly_coeffs, view_left, view_right);
    
    // b. 自动计算 Y 轴范围 (view_bottom, view_top)
    double min_y = evaluate(poly_coeffs, view_left);
    double max_y = min_y;
    int steps = 200;
    for (int i = 1; i <= steps; ++i) {
        double x = view_left + (view_right - view_left) * i / (double)steps;
        double y = evaluate(poly_coeffs, x);
        min_y = std::min(min_y, y);
        max_y = std::max(max_y, y);
    }

    // 添加 10% 的垂直边距
    double padding = (max_y - min_y) * 0.1;
    if (padding < 1.0) padding = 1.0; // 至少有 1.0 的边距
    view_bottom = min_y - padding;
    view_top = max_y + padding;
    
    // 处理水平线的情况
    if (std::abs(view_bottom - view_top) < EPSILON) {
        view_bottom -= 1.0;
        view_top += 1.0;
    }

    std::cout << "计算完成，找到 " << poly_roots.size() << " 个根。" << std::endl;
    for(double r : poly_roots) {
        std::cout << "  x = " << r << std::endl;
    }
    std::cout << "自动 Y 轴范围: [" << view_bottom << ", " << view_top << "]\n";
    std::cout << "即将启动 OpenGL 窗口...\n";

    // --- 3. 初始化并运行 OpenGL ---
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("计算机图形学实验 - 多项式求根 (可输入)");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glutMainLoop();
    return 0;
}