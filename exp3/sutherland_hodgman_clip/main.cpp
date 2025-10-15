#include <GLUT/glut.h>
#include "renderer.h"

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Sutherland-Hodgman Polygon Clipping");

    init();

    glutDisplayFunc(display);

    glutMainLoop();

    return 0;
}