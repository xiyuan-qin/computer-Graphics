#include "Spiral.h"
#include <iostream>

// 定义在 Spiral.h 中声明的全局变量
std::vector<Vec2f> spiralPointsPositive;
std::vector<Vec2f> spiralPointsNegative;

// 实现生成函数 (已修正为更稳定的算法)
void generateCornuSpiral() {
    // 清空旧数据
    spiralPointsPositive.clear();
    spiralPointsNegative.clear();

    // 调整参数以获得更平滑的曲线
    int num_points = 5000;
    float step = 0.005f;

    // 初始化
    Vec2f p = {0.0f, 0.0f};          // 当前位置，从原点开始
    Vec2f tangent = {1.0f, 0.0f};    // 初始方向，沿着 X 轴正方向
    float s = 0.0f;                  // 弧长

    spiralPointsPositive.push_back(p);

    // 迭代生成 s > 0 的部分
    for (int i = 0; i < num_points; ++i) {
        // 1. 沿着当前切线方向移动一小步
        p = p + tangent * step;
        
        // 2. 累加弧长
        s += step;
        
        // 3. 计算切线需要旋转的角度
        // 角度变化率 d(theta)/ds = 曲率 kappa
        // 所以，一个步长内的角度变化 d_theta = kappa * ds (这里 ds 就是 step)
        float curvature = s;
        float d_theta = curvature * step;
        
        // 4. 旋转切线，为下一步做准备
        tangent = rotate(tangent, d_theta);
        
        // 5. 存储新的点
        spiralPointsPositive.push_back(p);
    }

    // 利用中心对称性生成 s < 0 的部分
    for (size_t i = 1; i < spiralPointsPositive.size(); ++i) {
        const auto& pt = spiralPointsPositive[i];
        spiralPointsNegative.push_back({-pt.x, -pt.y});
    }
}