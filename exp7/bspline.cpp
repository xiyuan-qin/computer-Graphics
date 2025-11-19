/*
 * 文件名: bspline.cpp
 * 描述: macOS下使用OpenGL/GLUT实现的B样条曲线 (De Boor算法)
 * 编译环境: C++11, macOS GLUT framework
 */

#define GL_SILENCE_DEPRECATION // 屏蔽macOS的OpenGL废弃警告

#include <GLUT/glut.h> // macOS专用头文件路径
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

// ----------------- 数据结构与全局变量 -----------------

struct Point {
  float x, y;
};

// 全局变量
std::vector<Point> controlPoints; // 控制点集合
int degree = 3;       // B样条的次数 (Degree)，3表示三次B样条
int resolution = 200; // 曲线的采样精度

// 窗口大小
int width = 800;
int height = 600;

// ----------------- 核心算法: De Boor -----------------

/**
 * 生成准均匀节点向量 (Clamped/Open Uniform Knot Vector)
 * 特点：首尾节点的重复度为 degree + 1，使曲线经过首尾控制点
 * n: 控制点数量
 * p: 次数 (degree)
 * 返回: 节点向量
 */
std::vector<float> createKnotVector(int n, int p) {
  std::vector<float> knots;
  int m = n + p + 1; // 节点总数

  // 前 p+1 个节点为 0
  for (int i = 0; i <= p; ++i) {
    knots.push_back(0.0f);
  }

  // 中间节点均匀分布
  int internalKnots = m - 2 * (p + 1);
  for (int i = 1; i <= internalKnots; ++i) {
    knots.push_back(static_cast<float>(i) / (internalKnots + 1));
  }

  // 后 p+1 个节点为 1
  for (int i = 0; i <= p; ++i) {
    knots.push_back(1.0f);
  }

  return knots;
}

/**
 * De Boor 算法实现
 * u: 当前参数值 [0, 1]
 * k: 节点区间的索引，使得 knots[k] <= u < knots[k+1]
 * p: 次数
 * knots: 节点向量
 * ctrlPoints: 控制点
 */
Point deBoor(int k, float u, int p, const std::vector<float> &knots,
             const std::vector<Point> &ctrlPoints) {
  // 复制相关的 p+1 个控制点用于迭代计算
  // 只需要索引从 k-p 到 k 的控制点
  std::vector<Point> d;
  for (int j = 0; j <= p; ++j) {
    d.push_back(ctrlPoints[k - p + j]);
  }

  // 进行 p 次迭代
  for (int r = 1; r <= p; ++r) {
    for (int j = p; j >= r; --j) {
      int i = k - p + j;
      float denominator = knots[i + p + 1 - r] - knots[i];

      float alpha = 0.0f;
      if (denominator != 0.0f) {
        alpha = (u - knots[i]) / denominator;
      }

      // 线性插值公式: d[j] = (1 - alpha) * d[j-1] + alpha * d[j]
      d[j].x = (1.0f - alpha) * d[j - 1].x + alpha * d[j].x;
      d[j].y = (1.0f - alpha) * d[j - 1].y + alpha * d[j].y;
    }
  }

  return d[p]; // 最终结果在 d[p]
}

// 查找参数 u 所在的节点区间索引 k
int findKnotInterval(float u, int p, int n, const std::vector<float> &knots) {
  // 特殊情况：u == 1.0 (最后一个点)
  if (u >= knots[n])
    return n - 1;

  // 线性搜索 (也可以用二分查找优化)
  for (int i = p; i < n; ++i) {
    if (u >= knots[i] && u < knots[i + 1]) {
      return i;
    }
  }
  return -1;
}

// ----------------- OpenGL 渲染与交互 -----------------

void display() {
  glClear(GL_COLOR_BUFFER_BIT);

  // 1. 绘制控制多边形 (灰色)
  if (!controlPoints.empty()) {
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_STRIP);
    for (const auto &p : controlPoints) {
      glVertex2f(p.x, p.y);
    }
    glEnd();

    // 绘制控制点 (红色)
    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    for (const auto &p : controlPoints) {
      glVertex2f(p.x, p.y);
    }
    glEnd();
  }

  // 2. 绘制 B 样条曲线 (黄色)
  // 至少需要 p+1 个控制点才能绘制次数为 p 的 B 样条
  int n = controlPoints.size();
  if (n > degree) {
    std::vector<float> knots = createKnotVector(n, degree);

    glColor3f(1.0f, 1.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);

    // u 的定义域通常是 [knots[p], knots[n]]
    // 对于准均匀向量，范围就是 [0, 1]
    float uStart = knots[degree];
    float uEnd = knots[n];

    for (int i = 0; i <= resolution; ++i) {
      float u = uStart + (uEnd - uStart) * (float)i / (float)resolution;

      // 防止 u 稍微溢出导致数组越界
      if (u > uEnd)
        u = uEnd;

      int k = findKnotInterval(u, degree, n, knots);
      if (k != -1) {
        Point pOnCurve = deBoor(k, u, degree, knots, controlPoints);
        glVertex2f(pOnCurve.x, pOnCurve.y);
      }
    }
    glEnd();
  }

  glutSwapBuffers();
}

// 窗口重塑回调
void reshape(int w, int h) {
  width = w;
  height = h;
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, w, h, 0.0); // 设置坐标系：左上角(0,0)，右下角(w,h)
  glMatrixMode(GL_MODELVIEW);
}

// 鼠标点击回调
void mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    controlPoints.push_back({(float)x, (float)y});
    glutPostRedisplay(); // 触发重绘
  }
}

// 键盘回调
void keyboard(unsigned char key, int x, int y) {
  if (key == 27)
    exit(0); // ESC 退出
  if (key == 'c' || key == 'C') {
    controlPoints.clear(); // 清空画布
    glutPostRedisplay();
  }
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(width, height);
  glutCreateWindow("Mac B-Spline De Boor Demo");

  // 设置背景色为黑色
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutKeyboardFunc(keyboard);

  std::cout << "操作说明:" << std::endl;
  std::cout << "1. 鼠标左键点击屏幕添加控制点" << std::endl;
  std::cout << "2. 按 'C' 键清空屏幕" << std::endl;
  std::cout << "3. 至少需要 " << degree + 1 << " 个点才能生成曲线" << std::endl;

  glutMainLoop();
  return 0;
}