#ifndef CLIPPING_H
#define CLIPPING_H

#include <vector>
#include "geom.h"

// 裁剪窗口的边界
struct ClipWindow {
    float x_min, x_max, y_min, y_max;
};

// Sutherland-Hodgman 算法主函数
std::vector<Point> sutherlandHodgmanClip(
    const std::vector<Point>& subjectPolygon,
    const ClipWindow& window
);

#endif // CLIPPING_H