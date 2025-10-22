#ifndef SPIRAL_H
#define SPIRAL_H

#include <vector>
#include "Vector.h"

// 使用 extern 关键字声明全局变量，表明它们在别处定义。
// 这样 main.cpp 就可以访问这些由 Spiral.cpp 生成的数据。
extern std::vector<Vec2f> spiralPointsPositive;
extern std::vector<Vec2f> spiralPointsNegative;

// 声明生成顶点的函数
void generateCornuSpiral();

#endif // SPIRAL_H