#include <iostream>
#include <vector>
#include <cmath>

// 在 macOS 上，需要这样引入 GLUT
#include <GLUT/glut.h>
#include <OpenGL/gl.h>

// --- 向量数学辅助结构体和函数 ---

const float PI = 3.1415926535;

struct Vec2f {
    float x, y;

    // 向量加法
    Vec2f operator+(const Vec2f& other) const {
        return {x + other.x, y + other.y};
    }

    // 向量减法
    Vec2f operator-(const Vec2f& other) const {
        return {x - other.x, y - other.y};
    }
    
    // 标量乘法
    Vec2f operator*(float scalar) const {
        return {x * scalar, y * scalar};
    }

    // 计算向量长度
    float length() const {
        return std::sqrt(x * x + y * y);
    }

    // 向量归一化
    void normalize() {
        float len = length();
        if (len > 1e-6) { // 防止除以零
            x /= len;
            y /= len;
        }
    }
};

// 向量旋转函数
Vec2f rotate(const Vec2f& v, float angle_rad) {
    float cos_a = std::cos(angle_rad);
    float sin_a = std::sin(angle_rad);
    return {v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a};
}


// --- 全局变量 ---

// 用于存储螺旋线顶点的容器
std::vector<Vec2f> spiralPointsPositive;
std::vector<Vec2f> spiralPointsNegative;


// --- 核心逻辑：生成螺旋线顶点 ---

// 该函数根据文档中的算法生成 Cornu 螺旋线的顶点
void generateCornuSpiral() {
    // 根据文档建议，设置参数 [cite: 6]
    int num_points = 2000; // 点的数量决定了螺旋的长度
    float step = 0.01f;   // 步长 [cite: 6]

    // 初始化
    Vec2f p_curr = {0.0f, 0.0f};
    // 为了让初始切线方向为(1,0)，我们将 p_prev 放在 p_curr 左边
    Vec2f p_prev = {-step, 0.0f}; 
    float s = 0.0f;

    spiralPointsPositive.push_back(p_curr);

    // 迭代生成 s > 0 的部分
    for (int i = 0; i < num_points; ++i) {
        s += step;

        // 根据文档中的核心方程 k(s) = s [cite: 7]
        float curvature = s;
        float radius = 1.0f / curvature;

        // 1. 计算切线和法线
        Vec2f tangent = p_curr - p_prev;
        tangent.normalize();
        Vec2f normal = {-tangent.y, tangent.x}; // 法线是切线旋转90度

        // 2. 计算局部圆心
        Vec2f center = p_curr + normal * radius;

        // 3. 计算旋转角度 (弧长 = 半径 * 角度)
        float d_theta = step / radius;

        // 4. 计算下一个点的位置
        Vec2f v_to_curr = p_curr - center;
        Vec2f v_to_next = rotate(v_to_curr, d_theta);
        Vec2f p_next = center + v_to_next;

        // 更新点
        p_prev = p_curr;
        p_curr = p_next;
        
        spiralPointsPositive.push_back(p_curr);
    }

    // 利用中心对称性生成 s < 0 的部分
    // 跳过第一个点(原点)，因为它已经存在
    for (size_t i = 1; i < spiralPointsPositive.size(); ++i) {
        const auto& p = spiralPointsPositive[i];
        spiralPointsNegative.push_back({-p.x, -p.y});
    }
}


// --- OpenGL 回调函数 ---

// 窗口大小改变时的回调函数
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 保证横纵比，防止拉伸变形
    if (width > height) {
        float aspect = (float)width / height;
        gluOrtho2D(-1.2 * aspect, 1.2 * aspect, -1.2, 1.2);
    } else {
        float aspect = (float)height / width;
        gluOrtho2D(-1.2, 1.2, -1.2 * aspect, 1.2 * aspect);
    }
    glMatrixMode(GL_MODELVIEW);
}

// 绘制函数
void display() {
    // 清除颜色缓冲区
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // 1. 绘制坐标轴 (灰色)
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
        // X 轴
        glVertex2f(-2.0f, 0.0f);
        glVertex2f(2.0f, 0.0f);
        // Y 轴
        glVertex2f(0.0f, -2.0f);
        glVertex2f(0.0f, 2.0f);
    glEnd();

    // 2. 绘制 s > 0 的螺旋线部分 (红色)
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
    for (const auto& p : spiralPointsPositive) {
        glVertex2f(p.x, p.y);
    }
    glEnd();

    // 3. 绘制 s < 0 的螺旋线部分 (蓝色)
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINE_STRIP);
    for (const auto& p : spiralPointsNegative) {
        glVertex2f(p.x, p.y);
    }
    glEnd();
    
    // 如果使用双缓冲，交换缓冲区
    glutSwapBuffers();
}


// --- 主函数 ---

int main(int argc, char** argv) {
    // 生成螺旋线的顶点数据
    std::cout << "Generating Cornu Spiral points..." << std::endl;
    generateCornuSpiral();
    std::cout << "Point generation complete. Starting renderer." << std::endl;

    // 初始化 GLUT
    glutInit(&argc, argv);
    // 设置显示模式为 RGBA 颜色和双缓冲
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE); 
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Cornu Spiral Drawing");

    // 注册回调函数
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    // 设置背景色为黑色
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // 进入 GLUT 的主事件循环
    glutMainLoop();

    return 0;
}