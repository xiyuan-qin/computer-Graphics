#include <GLUT/glut.h>
#include "renderer.h"

// 从 renderer.cpp 引入回调函数的声明
void mouseCallback(int button, int state, int x, int y);
void motionCallback(int x, int y);
void keyboardCallback(unsigned char key, int x, int y);

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Interactive Sutherland-Hodgman Clipping");

    init();

    // 注册回调函数
    glutDisplayFunc(display);
    glutMouseFunc(mouseCallback);     // 注册鼠标点击回调
    glutMotionFunc(motionCallback);   // 注册鼠标拖动回调
    glutKeyboardFunc(keyboardCallback); // 注册键盘输入回调

    glutMainLoop();

    return 0;
}