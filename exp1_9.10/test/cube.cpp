#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include <GLUT/glut.h>
#include <OpenGL/gl.h>

// --- 数据结构 ---
struct Vec3 { float x, y, z; };

// 新的Face结构, 需要同时存储顶点索引和法线索引
struct Face {
    int v_indices[3];
    int vn_indices[3];
};

// --- 全局变量 ---
std::vector<Vec3> vertices; // 存储顶点
std::vector<Vec3> normals;  // 存储法线
std::vector<Face> faces;    // 存储面

// 交互控制 (与之前相同)
float rotateX = 20.0f, rotateY = -30.0f, zoom = -5.0f;
int lastMouseX, lastMouseY;
bool isDragging = false, isWireframe = false;

// --- 函数声明 ---
void loadOBJ(const std::string& filename);
void init();
void display();
void reshape(int w, int h);
void mouseButton(int button, int state, int x, int y);
void mouseMove(int x, int y);
void keyboard(unsigned char key, int x, int y);


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(150, 150);
    glutCreateWindow("OBJ Cube Viewer"); // 修改了窗口标题
    loadOBJ("cube.obj"); // 修改了加载的文件名
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMove);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}

/**
 * @brief [升级版] 从 .obj 文件加载顶点、法线和面信息
 * 现在可以解析 "f v//vn" 格式
 */
void loadOBJ(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file) { std::cerr << "错误: 无法打开文件 " << filename << std::endl; exit(1); }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") { // 顶点
            Vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        } else if (prefix == "vn") { // 法线
            Vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        } else if (prefix == "f") { // 面
            Face face;
            // 使用 sscanf 来轻松解析 "v//vn" 格式
            // C++的字符串流处理这种格式比较麻烦, sscanf 更直接
            char buffer[128];
            ss.read(buffer, sizeof(buffer)); // 读取剩余行内容
            sscanf(buffer, "%d//%d %d//%d %d//%d", 
                   &face.v_indices[0], &face.vn_indices[0],
                   &face.v_indices[1], &face.vn_indices[1],
                   &face.v_indices[2], &face.vn_indices[2]);

            // OBJ 索引从1开始, C++数组从0开始, 所以要减1
            for (int i = 0; i < 3; ++i) {
                face.v_indices[i]--;
                face.vn_indices[i]--;
            }
            faces.push_back(face);
        }
    }
    std::cout << "Cube模型加载成功: " << vertices.size() << " 个顶点, " << normals.size() << " 个法线, " << faces.size() << " 个面." << std::endl;
}


/**
 * @brief [升级版] 核心渲染函数
 * 现在使用从OBJ文件加载的法线, 实现更平滑的光照
 */
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, -0.5f, zoom); // 调整一下平移,让立方体居中
    glRotatef(rotateX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotateY, 0.0f, 1.0f, 0.0f);

    glPolygonMode(GL_FRONT_AND_BACK, isWireframe ? GL_LINE : GL_FILL);
    
    glColor3f(1.0f, 0.5f, 0.2f); // 给立方体一个橙色
    
    // 遍历所有面并绘制
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < faces.size(); ++i) {
        const Face& face = faces[i];
        for (int j = 0; j < 3; ++j) {
            // 在绘制每个顶点前, 先指定它的法线
            const Vec3& normal = normals[face.vn_indices[j]];
            glNormal3f(normal.x, normal.y, normal.z);
            
            const Vec3& vertex = vertices[face.v_indices[j]];
            glVertex3f(vertex.x, vertex.y, vertex.z);
        }
    }
    glEnd();
    
    glutSwapBuffers();
}


// --- 其他函数 (与pyramid.cpp基本相同, 无需修改) ---
void init() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat light_pos[] = { 2.0f, 3.0f, 3.0f, 1.0f };
    GLfloat white_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) { isDragging = true; lastMouseX = x; lastMouseY = y; }
        else { isDragging = false; }
    }
}

void mouseMove(int x, int y) {
    if (isDragging) {
        rotateY += (x - lastMouseX) * 0.5f;
        rotateX += (y - lastMouseY) * 0.5f;
        lastMouseX = x; lastMouseY = y;
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: case 'q': exit(0); break;
        case 'w': isWireframe = !isWireframe; glutPostRedisplay(); break;
    }
}