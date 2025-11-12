#include "PolySolve.h"
#include <iostream>
#include <iomanip> // 用于设置输出精度
#include <vector>
#include <string>
#include <limits> // 用于处理错误的输入

/**
 * @brief 辅助函数：安全地从用户那里获取一个浮点数
 */
double getDouble(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            // 清除输入缓冲区中剩余的任何内容（例如换行符）
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        } else {
            std::cout << "输入无效，请输入一个数字。\n";
            std::cin.clear(); // 清除错误标志
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 丢弃错误输入
        }
    }
}

/**
 * @brief 辅助函数：安全地从用户那里获取一个正整数
 */
int getDegree() {
    int degree;
    while (true) {
        std::cout << "请输入多项式的最高次幂 (例如: 3 表示 3 次方): ";
        if (std::cin >> degree && degree >= 0) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return degree;
        } else {
            std::cout << "输入无效，请输入一个非负整数。\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

/**
 * @brief 辅助函数：将多项式打印为字符串
 */
void printPolynomial(const std::vector<double>& coeffs) {
    std::cout << "P(x) = ";
    for (int i = coeffs.size() - 1; i >= 0; --i) {
        if (std::abs(coeffs[i]) < EPSILON) continue;
        
        // 系数和符号
        if (i < (int)coeffs.size() - 1) {
            std::cout << (coeffs[i] > 0 ? " + " : " - ");
            std::cout << std::abs(coeffs[i]);
        } else {
            std::cout << coeffs[i];
        }

        // x 和幂
        if (i > 0) {
            std::cout << "x";
            if (i > 1) {
                std::cout << "^" << i;
            }
        }
    }
    std::cout << "\n";
}


int main() {
    std::cout << "--- 多项式求根 (C++ 版本) ---\n";

    // 1. 获取多项式系数
    int degree = getDegree();
    std::vector<double> coeffs(degree + 1);
    
    std::cout << "请按照从低到高的次序输入系数 (c0, c1, ...):\n";
    for (int i = 0; i <= degree; ++i) {
        std::string prompt = "请输入 c" + std::to_string(i) + " (x^" + std::to_string(i) + " 的系数): ";
        coeffs[i] = getDouble(prompt);
    }
    
    // 打印多项式以供确认
    printPolynomial(coeffs);

    // 2. 获取查找区间
    double left = getDouble("请输入查找区间的左边界 (left): ");
    double right;
    while (true) {
        right = getDouble("请输入查找区间的右边界 (right): ");
        if (right > left) {
            break;
        }
        std::cout << "右边界必须大于左边界，请重新输入。\n";
    }

    // 3. 计算并输出结果
    std::cout << "\n正在查找区间 [" << left << ", " << right << "] 上的根...\n";
              
    std::vector<double> roots = FindRoots(coeffs, left, right);

    std::cout << "找到了 " << roots.size() << " 个根:" << std::endl;
    std::cout << std::fixed << std::setprecision(6); // 设置输出精度
    if (roots.empty()) {
        std::cout << "在该区间内没有找到根。\n";
    } else {
        for (double root : roots) {
            std::cout << "x = " << root << std::endl;
        }
    }

    return 0;
}