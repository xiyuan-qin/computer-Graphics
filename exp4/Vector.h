#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>

const float PI = 3.1415926535;

// 二维向量结构体
struct Vec2f {
    float x, y;

    // 成员函数声明
    Vec2f operator+(const Vec2f& other) const;
    Vec2f operator-(const Vec2f& other) const;
    Vec2f operator*(float scalar) const;
    float length() const;
    void normalize();
};

// 旋转函数的声明
Vec2f rotate(const Vec2f& v, float angle_rad);

#endif // VECTOR_H