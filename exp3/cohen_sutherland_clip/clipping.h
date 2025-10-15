#ifndef CLIPPING_H
#define CLIPPING_H

// 声明全局的裁剪窗口边界
extern const double X_MIN, Y_MIN, X_MAX, Y_MAX;

// Cohen-Sutherland 裁剪算法的声明
bool cohenSutherlandClip(double& x1, double& y1, double& x2, double& y2);

#endif // CLIPPING_H