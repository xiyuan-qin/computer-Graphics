#include "scanline.h"
#include <OpenGL/gl.h>
#include <vector>
#include <algorithm>
#include <cmath>

// 内部使用的边结构体
struct Edge {
    int ymax;      // 边的最大y值
    float x;       // 边与当前扫描线的交点的x坐标
    float inv_m;   // 斜率的倒数 1/m
    Edge* next;    // 指向下一条边的指针
};

// 扫描线填充算法实现
void scanlineFill(const std::vector<Point>& polygon, int window_height) {
    if (polygon.size() < 3) return;

    int minY = window_height, maxY = 0;
    for (const auto& p : polygon) {
        if (p.y < minY) minY = p.y;
        if (p.y > maxY) maxY = p.y;
    }

    // 1. 创建边表 (ET)
    std::vector<Edge*> et(window_height, nullptr);
    for (size_t i = 0; i < polygon.size(); ++i) {
        Point p1 = polygon[i];
        Point p2 = polygon[(i + 1) % polygon.size()];

        if (p1.y == p2.y) continue; // 忽略水平边

        int y_max = std::max(p1.y, p2.y);
        int y_start = std::min(p1.y, p2.y);
        float x_start = (p1.y < p2.y) ? (float)p1.x : (float)p2.x;
        float inv_m = (float)(p2.x - p1.x) / (float)(p2.y - p1.y);

        Edge* newEdge = new Edge{y_max, x_start, inv_m, et[y_start]};
        et[y_start] = newEdge;
    }

    // 2. 创建活性边表 (AET)
    Edge* aet = nullptr;

    // 3. 逐行扫描
    for (int y = minY; y < maxY; ++y) {
        // a. 将 ET 中 y 对应的边全部移入 AET
        if (et[y]) {
            Edge* p = et[y];
            while (p) {
                Edge* next = p->next;
                p->next = aet;
                aet = p;
                p = next;
            }
        }
        
        // b. 从 AET 中移除 ymax <= y 的边
        Edge* prev = nullptr;
        Edge* curr = aet;
        while (curr) {
            if (curr->ymax <= y) {
                if (prev) {
                    prev->next = curr->next;
                    delete curr;
                    curr = prev->next;
                } else {
                    aet = curr->next;
                    delete curr;
                    curr = aet;
                }
            } else {
                prev = curr;
                curr = curr->next;
            }
        }
        
        // c. 对整个 AET 按 x 坐标进行排序（使用冒泡排序，简单可靠）
        if (aet && aet->next) {
            bool sorted = false;
            while (!sorted) {
                sorted = true;
                Edge* p = aet;
                Edge** pp = &aet;
                while (p && p->next) {
                    if (p->x > p->next->x) {
                        sorted = false;
                        Edge* temp = p->next;
                        p->next = temp->next;
                        temp->next = p;
                        *pp = temp;
                    }
                    pp = &((*pp)->next);
                    p = *pp;
                }
            }
        }

        // d. 填充像素
        glColor3f(0.0f, 0.5f, 0.8f);
        glBegin(GL_POINTS);
        for (Edge* p = aet; p && p->next; p = p->next->next) {
            for (int x = (int)ceil(p->x); x < (int)floor(p->next->x); ++x) {
                glVertex2i(x, y);
            }
        }
        glEnd();

        // e. 更新 AET 中每条边的 x 值
        for (Edge* p = aet; p; p = p->next) {
            p->x += p->inv_m;
        }
    }
}