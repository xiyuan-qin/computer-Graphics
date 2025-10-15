#include <GLUT/glut.h>
#include "renderer.h"

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Experiment 1: Scanline Polygon Fill");

    init(); // 调用初始化函数

    glutDisplayFunc(display); // 注册显示回调函数

    glutMainLoop(); // 进入 GLUT 事件循环

    return 0;
}