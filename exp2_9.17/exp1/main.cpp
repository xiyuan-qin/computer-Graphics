#include <iostream>
#include <vector>
#include <cmath>
#include <OpenGL/gl.h> // 使用 macOS 的 OpenGL 头文件
#include <GLUT/glut.h>  // 使用 macOS 的 GLUT 头文件

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// --- 设置 ---
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// --- Arcball 状态 ---
bool arcball_on = false;
glm::vec2 last_mouse_pos;
glm::quat current_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
glm::quat final_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

// --- 自动旋转 ---
float auto_rotate_speed = 0.2f;
glm::vec3 auto_rotate_axis = glm::normalize(glm::vec3(0.3f, 1.0f, 0.2f));

// --- 着色器和球体数据 ---
GLuint shaderProgram;
GLuint VAO, VBO;
int sphere_vertex_count;

// --- 函数声明 ---
void display();
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void idle();
void initShader();
void initSphere();
glm::vec3 map_to_arcball(glm::vec2 point);
std::vector<float> generate_sphere(float radius, int sectors, int stacks);

// --- 着色器代码 (与之前相同) ---
const char *vertexShaderSource = R"glsl(
#version 120
attribute vec3 aPos;
attribute vec3 aNormal;

varying vec3 FragPos;
varying vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    // 法线矩阵计算在 GLSL 120 中更手动
    Normal = mat3(model) * aNormal; 
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)glsl";

const char *fragmentShaderSource = R"glsl(
#version 120
varying vec3 FragPos;
varying vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    // Ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.8;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    gl_FragColor = vec4(result, 1.0);
}
)glsl";

// 在 main 函数或任何其他函数之前添加这个新函数
void drawAxes() {
    // 关闭着色器，使用 OpenGL 的固定管线功能来画简单的带颜色的线
    glUseProgram(0); 

    // 设置线宽
    glLineWidth(2.0f);

    glBegin(GL_LINES);
    // X轴 (红色)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    // Y轴 (绿色)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    // Z轴 (蓝色)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();

    // 恢复使用我们的着色器程序，以便后续绘制球体
    glUseProgram(shaderProgram);
}

void idle()
{
    // 使用静态变量来记录上一帧的时间
    static float lastTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    
    // 获取当前时间
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    // 计算时间差 (delta time)
    float deltaTime = currentTime - lastTime;
    // 更新上一帧的时间
    lastTime = currentTime;
    
    // --- 自动旋转 ---
    // 只有在鼠标没有拖拽时才执行自动旋转
    if (!arcball_on) {
        // 根据时间差计算当前帧需要旋转的角度
        float rotate_angle = auto_rotate_speed * deltaTime * 50.0f;
        glm::quat auto_rot = glm::angleAxis(glm::radians(rotate_angle), auto_rotate_axis);
        // 将自动旋转累加到总旋转中
        final_rotation = auto_rot * final_rotation;
    }
    
    // 请求 GLUT 在下一个循环中重绘窗口，这会触发 display() 函数的调用
    glutPostRedisplay();
}


int main(int argc, char** argv)
{
    // --- 初始化GLUT ---
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
    glutCreateWindow("GLUT Arcball Demo");

    // --- 注册回调函数 ---
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutIdleFunc(idle);

    // --- 初始化 ---
    initShader();
    initSphere();
    glEnable(GL_DEPTH_TEST);

    // --- 进入主循环 ---
    glutMainLoop();
    return 0;
}

void display()
{
    // --- 1. 清理屏幕 ---
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // --- 2. 计算变换矩阵 ---
    // 投影矩阵 (决定了视野范围)
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    // 视图矩阵 (决定了相机位置和朝向)
    glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::mat4 view = glm::lookAt(camera_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // 模型矩阵 (包含了物体的旋转、平移、缩放，这里只有旋转)
    glm::mat4 model = glm::toMat4(final_rotation); 

    // --- 3. 绘制坐标轴 (使用固定管线) ---
    // 设置 OpenGL 的旧版矩阵堆栈
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(projection));
    glMatrixMode(GL_MODELVIEW);
    // 将视图矩阵和模型矩阵相乘，这样坐标轴就会和球体一样被相机观察，并一起旋转
    glLoadMatrixf(glm::value_ptr(view * model)); 
    // 调用绘制函数
    drawAxes();

    // --- 4. 绘制球体 (使用着色器) ---
    // 激活我们的着色器程序
    glUseProgram(shaderProgram);

    // 将矩阵作为 uniform 变量传递给着色器
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    
    // 将光照相关的 uniform 变量传递给着色器
    glm::vec3 lightPos(5.0f, 5.0f, 2.0f); // 使用一个更偏的光源位置
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, &lightPos[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &camera_pos[0]);
    glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 0.8f, 0.3f, 0.31f);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);

    // 绑定球体的顶点数组对象(VAO)并绘制
    glBindVertexArrayAPPLE(VAO);
    glDrawArrays(GL_TRIANGLES, 0, sphere_vertex_count);
    glBindVertexArrayAPPLE(0); // 解绑VAO

    // --- 5. 交换前后缓冲区，显示画面 ---
    glutSwapBuffers();
}


void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            arcball_on = true;
            last_mouse_pos = glm::vec2(x, y);
        } else if (state == GLUT_UP) {
            arcball_on = false;
        }
    }
}

void motion(int x, int y)
{
    if (arcball_on) {
        glm::vec2 current_mouse_pos = glm::vec2(x, y);

        glm::vec3 v_start = map_to_arcball(last_mouse_pos);
        glm::vec3 v_end = map_to_arcball(current_mouse_pos);

        float angle = acos(glm::min(1.0f, glm::dot(v_start, v_end)));
        glm::vec3 axis = glm::cross(v_start, v_end);

        float sensitivity = 1.5f;
        current_rotation = glm::angleAxis(angle * sensitivity, axis);
        
        final_rotation = current_rotation * final_rotation;
        last_mouse_pos = current_mouse_pos;
    }
}

glm::vec3 map_to_arcball(glm::vec2 point) {
    glm::vec3 result(0.0f);
    result.x = (2.0f * point.x / SCR_WIDTH) - 1.0f;
    result.y = 1.0f - (2.0f * point.y / SCR_HEIGHT);
    result.z = 0.0f;

    float mag_sq = result.x * result.x + result.y * result.y;
    if (mag_sq <= 1.0f) {
        result.z = sqrt(1.0f - mag_sq);
    } else {
        result = glm::normalize(result);
    }
    return result;
}

// --- 初始化和几何体生成函数 ---

void initShader() {
    // 为了适配 macOS 上 GLUT 默认的旧版 GLSL，版本号改为 120
    // 并将 in/out/layout 关键字改为 attribute/varying
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // ... 应该添加错误检查 ...

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // ... 应该添加错误检查 ...

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    
    // GLSL 120 中，我们需要手动绑定 attribute 位置
    glBindAttribLocation(shaderProgram, 0, "aPos");
    glBindAttribLocation(shaderProgram, 1, "aNormal");

    glLinkProgram(shaderProgram);
    // ... 应该添加错误检查 ...
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void initSphere() {
    std::vector<float> sphere_vertices = generate_sphere(0.6f, 50, 50);
    sphere_vertex_count = sphere_vertices.size() / 6;

    // 在 macOS 的 GLUT 环境中，VAO 需要使用 APPLE 扩展
    glGenVertexArraysAPPLE(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArrayAPPLE(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphere_vertices.size() * sizeof(float), sphere_vertices.data(), GL_STATIC_DRAW);

    GLuint pos_attrib = glGetAttribLocation(shaderProgram, "aPos");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    GLuint normal_attrib = glGetAttribLocation(shaderProgram, "aNormal");
    glEnableVertexAttribArray(normal_attrib);
    glVertexAttribPointer(normal_attrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArrayAPPLE(0);
}

// generate_sphere 函数与之前的版本完全相同，这里省略以节省空间
// 如果需要，我可以再次提供
std::vector<float> generate_sphere(float radius, int sectors, int stacks) {
    std::vector<float> vertices;
    float x, y, z, xy;
    float nx, ny, nz, length_inv = 1.0f / radius;
    const float PI = 3.14159265359f;

    float sector_step = 2 * PI / sectors;
    float stack_step = PI / stacks;
    float sector_angle, stack_angle;

    for(int i = 0; i <= stacks; ++i) {
        stack_angle = PI / 2 - i * stack_step;
        xy = radius * cosf(stack_angle);
        z = radius * sinf(stack_angle);

        for(int j = 0; j <= sectors; ++j) {
            sector_angle = j * sector_step;
            x = xy * cosf(sector_angle);
            y = xy * sinf(sector_angle);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            nx = x * length_inv;
            ny = y * length_inv;
            nz = z * length_inv;
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
        }
    }
    
    std::vector<float> sphere_data;
    int k1, k2;
    for(int i = 0; i < stacks; ++i) {
        k1 = i * (sectors + 1);
        k2 = k1 + sectors + 1;
        for(int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if(i != 0) {
                for(int k=0; k<6; ++k) sphere_data.push_back(vertices[k1*6+k]);
                for(int k=0; k<6; ++k) sphere_data.push_back(vertices[k2*6+k]);
                for(int k=0; k<6; ++k) sphere_data.push_back(vertices[(k1+1)*6+k]);
            }
            if(i != (stacks-1)) {
                for(int k=0; k<6; ++k) sphere_data.push_back(vertices[(k1+1)*6+k]);
                for(int k=0; k<6; ++k) sphere_data.push_back(vertices[k2*6+k]);
                for(int k=0; k<6; ++k) sphere_data.push_back(vertices[(k2+1)*6+k]);
            }
        }
    }
    return sphere_data;
}