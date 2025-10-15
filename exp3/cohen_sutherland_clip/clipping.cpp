#include "clipping.h"
#include <algorithm>

// 区域码
const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

// 裁剪窗口边界 (可以定义在 renderer.cpp 中并通过参数传递，这里为简化直接定义)
const double X_MIN = 150, Y_MIN = 150;
const double X_MAX = 650, Y_MAX = 450;

// 计算点的区域码
int computeCode(double x, double y) {
    int code = INSIDE;
    if (x < X_MIN) code |= LEFT;
    else if (x > X_MAX) code |= RIGHT;
    if (y < Y_MIN) code |= BOTTOM;
    else if (y > Y_MAX) code |= TOP;
    return code;
}

bool cohenSutherlandClip(double& x1, double& y1, double& x2, double& y2) {
    int code1 = computeCode(x1, y1);
    int code2 = computeCode(x2, y2);

    while (true) {
        if ((code1 == 0) && (code2 == 0)) { // 情况1: 完全可见
            return true;
        } else if (code1 & code2) { // 情况2: 完全不可见
            return false;
        } else { // 情况3: 需要裁剪
            int code_out = (code1 != 0) ? code1 : code2;
            double x = 0, y = 0;

            if (code_out & TOP) {
                x = x1 + (x2 - x1) * (Y_MAX - y1) / (y2 - y1);
                y = Y_MAX;
            } else if (code_out & BOTTOM) {
                x = x1 + (x2 - x1) * (Y_MIN - y1) / (y2 - y1);
                y = Y_MIN;
            } else if (code_out & RIGHT) {
                y = y1 + (y2 - y1) * (X_MAX - x1) / (x2 - x1);
                x = X_MAX;
            } else if (code_out & LEFT) {
                y = y1 + (y2 - y1) * (X_MIN - x1) / (x2 - x1);
                x = X_MIN;
            }

            if (code_out == code1) {
                x1 = x; y1 = y;
                code1 = computeCode(x1, y1);
            } else {
                x2 = x; y2 = y;
                code2 = computeCode(x2, y2);
            }
        }
    }
}