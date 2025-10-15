#include "clipping.h"

// 定义边的代码，方便判断点在哪一侧
enum Edge { LEFT, RIGHT, BOTTOM, TOP };

// 计算两点连线与裁剪边界的交点
Point intersect(const Point& p1, const Point& p2, Edge boundary, const ClipWindow& window) {
    Point intersection;
    float m = (p2.y - p1.y) / (p2.x - p1.x); // 斜率

    switch (boundary) {
        case LEFT:
            intersection.x = window.x_min;
            intersection.y = p1.y + m * (window.x_min - p1.x);
            break;
        case RIGHT:
            intersection.x = window.x_max;
            intersection.y = p1.y + m * (window.x_max - p1.x);
            break;
        case BOTTOM:
            intersection.y = window.y_min;
            intersection.x = p1.x + (window.y_min - p1.y) / m;
            break;
        case TOP:
            intersection.y = window.y_max;
            intersection.x = p1.x + (window.y_max - p1.y) / m;
            break;
    }
    return intersection;
}

// 检查一个点是否在裁剪边界的“内侧”
bool isInside(const Point& p, Edge boundary, const ClipWindow& window) {
    switch (boundary) {
        case LEFT:   return p.x >= window.x_min;
        case RIGHT:  return p.x <= window.x_max;
        case BOTTOM: return p.y >= window.y_min;
        case TOP:    return p.y <= window.y_max;
    }
    return false;
}

// 针对一条裁剪边界，裁剪一个多边形（顶点列表）
std::vector<Point> clipAgainstEdge(const std::vector<Point>& subjectPolygon, Edge boundary, const ClipWindow& window) {
    std::vector<Point> outputList;
    if (subjectPolygon.empty()) {
        return outputList;
    }

    Point s = subjectPolygon.back(); // s 是上一轮循环的终点

    for (const Point& p : subjectPolygon) {
        bool s_inside = isInside(s, boundary, window);
        bool p_inside = isInside(p, boundary, window);

        // 情况1：两个顶点都在内侧 -> 只输出终点 p
        if (s_inside && p_inside) {
            outputList.push_back(p);
        }
        // 情况2：起点在内，终点在外 -> 输出交点
        else if (s_inside && !p_inside) {
            outputList.push_back(intersect(s, p, boundary, window));
        }
        // 情况3：起点在外，终点在内 -> 输出交点和终点 p
        else if (!s_inside && p_inside) {
            outputList.push_back(intersect(s, p, boundary, window));
            outputList.push_back(p);
        }
        // 情况4：两个顶点都在外侧 -> 无输出

        s = p; // 更新 s 为当前点，作为下一轮循环的起点
    }

    return outputList;
}

// Sutherland-Hodgman 算法主函数
std::vector<Point> sutherlandHodgmanClip(const std::vector<Point>& subjectPolygon, const ClipWindow& window) {
    // 依次对四条边进行裁剪
    std::vector<Point> outputList = clipAgainstEdge(subjectPolygon, LEFT, window);
    outputList = clipAgainstEdge(outputList, RIGHT, window);
    outputList = clipAgainstEdge(outputList, BOTTOM, window);
    outputList = clipAgainstEdge(outputList, TOP, window);

    return outputList;
}