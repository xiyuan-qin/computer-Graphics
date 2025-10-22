#include "Vector.h"

// --- Vec2f 成员函数实现 ---
Vec2f Vec2f::operator+(const Vec2f& other) const {
    return {x + other.x, y + other.y};
}

Vec2f Vec2f::operator-(const Vec2f& other) const {
    return {x - other.x, y - other.y};
}

Vec2f Vec2f::operator*(float scalar) const {
    return {x * scalar, y * scalar};
}

float Vec2f::length() const {
    return std::sqrt(x * x + y * y);
}

void Vec2f::normalize() {
    float len = length();
    if (len > 1e-6) { // 防止除以零
        x /= len;
        y /= len;
    }
}

// --- 旋转函数实现 ---
Vec2f rotate(const Vec2f& v, float angle_rad) {
    float cos_a = std::cos(angle_rad);
    float sin_a = std::sin(angle_rad);
    return {v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a};
}