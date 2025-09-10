#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

// 在 macOS 上, 必须使用 <GLUT/glut.h> 和 <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>

// --- 数据结构 ---
// 用于存储三维向量 (顶点或法线)
struct Vec3 {
    float x, y, z;
};

// 用于存储一个三角形面 (包含三个顶点索引)
struct Face {
    int v1, v2, v3;
};

// --- 全局变量 ---
// 模型数据
std::vector<Vec3> vertices; // 存储从OBJ文件读取的顶点
std::vector<Face> faces;    // 存储从OBJ文件读取的面

// 交互控制
float rotateX = 20.0f;
float rotateY = 0.0f;
float zoom = -5.0f;
int lastMouseX, lastMouseY;
bool isDragging = false;
bool isWireframe = false; // 控制显示模式 (线框/填充)

// --- 函数声明 ---
void loadOBJ(const std::string& filename);
void init();
void display();
void reshape(int w, int h);
void mouseButton(int button, int state, int x, int y);
void mouseMove(int x, int y);
void keyboard(unsigned char key, int x, int y);

// --- 主函数 ---
int main(int argc, char** argv) {
    // 1. 初始化GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OBJ Pyramid Viewer");

    // 2. 加载模型
    loadOBJ("pyramid.obj");

    // 3. 设置OpenGL状态
    init();

    // 4. 注册回调函数
    glutDisplayFunc(display);       // 渲染函数
    glutReshapeFunc(reshape);       // 窗口大小改变函数
    glutMouseFunc(mouseButton);     // 鼠标点击函数
    glutMotionFunc(mouseMove);      // 鼠标拖动函数
    glutKeyboardFunc(keyboard);     // 键盘输入函数

    // 5. 进入主循环
    glutMainLoop();
    return 0;
}

// --- 函数实现 ---

/**
 * @brief 从 .obj 文件加载顶点和面信息
 */
void loadOBJ(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file) {
        std::cerr << "错误: 无法打开文件 " << filename << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") { // 顶点
            Vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        } else if (prefix == "f") { // 面
            Face face;
            // OBJ 索引从1开始, C++数组从0开始, 所以要减1
            ss >> face.v1 >> face.v2 >> face.v3;
            face.v1--; 
            face.v2--;
            face.v3--;
            faces.push_back(face);
        }
    }
    std::cout << "模型加载成功: " << vertices.size() << " 个顶点, " << faces.size() << " 个面." << std::endl;
}

/**
 * @brief 初始化OpenGL状态 (光照、深度测试等)
 */
void init() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // 设置背景颜色为深灰色
    glEnable(GL_DEPTH_TEST); // 开启深度测试, 保证物体前后关系正确

    glEnable(GL_NORMALIZE); // 让OpenGL自动将法线长度缩放到1

    // --- 设置光照 ---
    glEnable(GL_LIGHTING); // 开启光照总开关
    glEnable(GL_LIGHT0);   // 开启0号光源

    // 设置光源属性
    GLfloat light_pos[] = { 2.0f, 2.0f, 2.0f, 1.0f }; // 光源位置
    GLfloat white_light[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // 光源颜色
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);

    // --- 设置材质 ---
    glEnable(GL_COLOR_MATERIAL); // 允许使用glColor来指定材质颜色
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

/**
 * @brief 核心渲染函数
 */
void display() {
    // 1. 清除颜色和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. 切换到模型视图矩阵, 并重置
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 3. 应用鼠标交互的变换 (平移和旋转)
    glTranslatef(0.0f, 0.0f, zoom);
    glRotatef(rotateX, 1.0f, 0.0f, 0.0f); // 绕X轴旋转
    glRotatef(rotateY, 0.0f, 1.0f, 0.0f); // 绕Y轴旋转

    // 4. 根据isWireframe变量切换渲染模式
    glPolygonMode(GL_FRONT_AND_BACK, isWireframe ? GL_LINE : GL_FILL);
    
    // 5. 绘制模型
    glColor3f(0.5f, 0.7f, 1.0f); // 设置物体颜色
    for (size_t i = 0; i < faces.size(); ++i) {
        const Face& face = faces[i];
        const Vec3& v1 = vertices[face.v1];
        const Vec3& v2 = vertices[face.v2];
        const Vec3& v3 = vertices[face.v3];

        // 为了简单的光照, 我们即时计算面法线 (Flat Shading)
        Vec3 U = {v2.x - v1.x, v2.y - v1.y, v2.z - v1.z};
        Vec3 V = {v3.x - v1.x, v3.y - v1.y, v3.z - v1.z};
        Vec3 normal = {
            U.y * V.z - U.z * V.y,
            U.z * V.x - U.x * V.z,
            U.x * V.y - U.y * V.x
        };

        glBegin(GL_TRIANGLES);
            glNormal3f(normal.x, normal.y, normal.z); // 在顶点前指定法线
            glVertex3f(v1.x, v1.y, v1.z);
            glVertex3f(v2.x, v2.y, v2.z);
            glVertex3f(v3.x, v3.y, v3.z);
        glEnd();
    }

    // 6. 交换前后缓冲区, 显示图像
    glutSwapBuffers();
}

/**
 * @brief 窗口大小改变时的回调函数
 */
void reshape(int w, int h) {
    if (h == 0) h = 1;
    float ratio = 1.0f * w / h;

    // 1. 设置视口
    glViewport(0, 0, w, h);

    // 2. 设置投影矩阵
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 使用透视投影
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);

    // 3. 切回模型视图矩阵
    glMatrixMode(GL_MODELVIEW);
}

/**
 * @brief 鼠标点击事件
 */
void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isDragging = true;
            lastMouseX = x;
            lastMouseY = y;
        } else {
            isDragging = false;
        }
    }
}

/**
 * @brief 鼠标拖动事件
 */
void mouseMove(int x, int y) {
    if (isDragging) {
        rotateY += (x - lastMouseX) * 0.5f;
        rotateX += (y - lastMouseY) * 0.5f;
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay(); // 请求重绘窗口
    }
}

/**
 * @brief 键盘输入事件
 */
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // ESC键
        case 'q':
            exit(0);
            break;
        case 'w': // 'w' 键切换线框模式
            isWireframe = !isWireframe;
            std::cout << "显示模式切换: " << (isWireframe ? "线框" : "填充") << std::endl;
            glutPostRedisplay(); // 请求重绘
            break;
    }
}