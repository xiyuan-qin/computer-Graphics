#include <GLUT/glut.h>
#include "renderer.h"

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(150, 150);
    glutCreateWindow("Experiment 2: Cohen-Sutherland Clipping");

    init(); // 调用初始化函数

    glutDisplayFunc(display); // 注册显示回调函数

    glutMainLoop(); // 进入 GLUT 事件循环

    return 0;
}