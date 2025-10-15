#ifndef SCANLINE_H
#define SCANLINE_H

#include <vector>

// 定义二维点
struct Point {
    int x, y;
};

// 扫描线填充算法的声明
void scanlineFill(const std::vector<Point>& polygon, int window_height);

#endif // SCANLINE_H