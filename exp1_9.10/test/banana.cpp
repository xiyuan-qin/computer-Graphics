#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include <GLUT/glut.h>
#include <OpenGL/gl.h>

// --- 数据结构 ---
struct Vec2 { float u, v; };
struct Vec3 { float x, y, z; };

// 新的Face结构, 存储顶点、纹理和法线索引
struct Face {
    int v_indices[3];
    int vt_indices[3];
    int vn_indices[3];
};

// --- 全局变量 ---
std::vector<Vec3> vertices;
std::vector<Vec2> texcoords; // 新增：存储纹理坐标
std::vector<Vec3> normals;
std::vector<Face> faces;

// 交互控制
float rotateX = 75.0f, rotateY = 0.0f, zoom = -100.0f; // 调整了初始视角
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
    glutInitWindowPosition(200, 200);
    glutCreateWindow("OBJ Banana Viewer");
    loadOBJ("banana.obj");
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
 * @brief [高级版] 从 .obj 文件加载数据
 * - 可以解析 "v", "vt", "vn"
 * - 可以解析 "f v/vt/vn" 格式
 * - 可以将四边形面 (Quads) 自动拆分为两个三角形
 */
void loadOBJ(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file) { std::cerr << "错误: 无法打开文件 " << filename << std::endl; exit(1); }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            Vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        } else if (prefix == "vt") {
            Vec2 texcoord;
            ss >> texcoord.u >> texcoord.v;
            texcoords.push_back(texcoord);
        } else if (prefix == "vn") {
            Vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        } else if (prefix == "f") {
            int v[4], vt[4], vn[4];
            int count = sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", 
                   &v[0], &vt[0], &vn[0], &v[1], &vt[1], &vn[1], 
                   &v[2], &vt[2], &vn[2], &v[3], &vt[3], &vn[3]);

            // 根据sscanf成功匹配的参数数量来判断是三角形还是四边形
            int num_verts = count / 3;

            // 第一个三角形 (对三角形和四边形都适用)
            Face face1;
            for(int i = 0; i < 3; ++i) {
                face1.v_indices[i] = v[i] - 1;
                face1.vt_indices[i] = vt[i] - 1;
                face1.vn_indices[i] = vn[i] - 1;
            }
            faces.push_back(face1);

            // 如果是四边形, 创建第二个三角形
            if (num_verts == 4) {
                Face face2;
                int indices[] = {0, 2, 3}; // (v0, v2, v3) 构成第二个三角形
                for(int i = 0; i < 3; ++i) {
                    face2.v_indices[i] = v[indices[i]] - 1;
                    face2.vt_indices[i] = vt[indices[i]] - 1;
                    face2.vn_indices[i] = vn[indices[i]] - 1;
                }
                faces.push_back(face2);
            }
        }
    }
    std::cout << "Banana模型加载成功: " << vertices.size() << " 个顶点, " << texcoords.size() << " 个纹理坐标, " << normals.size() << " 个法线, " << faces.size() << " 个三角面." << std::endl;
}

/**
 * @brief [高级版] 核心渲染函数
 * 现在会传递纹理坐标
 */
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, -20.0f, zoom); // 根据模型大小调整平移
    glRotatef(rotateX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotateY, 0.0f, 1.0f, 0.0f);

    glPolygonMode(GL_FRONT_AND_BACK, isWireframe ? GL_LINE : GL_FILL);
    
    glColor3f(1.0f, 1.0f, 0.3f); // 给香蕉一个黄色
    
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < faces.size(); ++i) {
        const Face& face = faces[i];
        for (int j = 0; j < 3; ++j) {
            const Vec3& normal = normals[face.vn_indices[j]];
            glNormal3f(normal.x, normal.y, normal.z);
            
            const Vec2& texcoord = texcoords[face.vt_indices[j]];
            glTexCoord2f(texcoord.u, texcoord.v);
            
            const Vec3& vertex = vertices[face.v_indices[j]];
            glVertex3f(vertex.x, vertex.y, vertex.z);
        }
    }
    glEnd();
    
    glutSwapBuffers();
}

// --- 其他函数 (与之前相同) ---
void init() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glFrontFace(GL_CW);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat light_pos[] = { 0.0f, 50.0f, 50.0f, 1.0f };
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
    gluPerspective(45.0f, (float)w / h, 0.1f, 500.0f); // 调整了远裁剪面
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