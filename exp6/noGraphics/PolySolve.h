#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

using namespace std;

// 定义一个很小的数，用于比较
const double EPSILON = 1e-9;

/**
 * @brief 使用霍纳法 (Horner's method) 计算多项式在 x 处的值
 * @param coeffs 系数 {c0, c1, c2, ...} 代表 c0 + c1*x + c2*x^2 + ...
 * @param x 要求值的点
 * @return double P(x)
 */
double evaluate(const vector<double>& coeffs, double x) {
    double result = 0.0;
    for (int i = coeffs.size() - 1; i >= 0; --i) {
        result = result * x + coeffs[i];
    }
    return result;
}

/**
 * @brief 计算多项式的导数
 * @param coeffs P(x) 的系数
 * @return vector<double> P'(x) 的系数
 */
vector<double> derivative(const vector<double>& coeffs) {
    if (coeffs.size() <= 1) {
        return {0.0}; // 常数的导数是 0
    }
    vector<double> deriv_coeffs(coeffs.size() - 1);
    for (size_t i = 1; i < coeffs.size(); ++i) {
        deriv_coeffs[i - 1] = coeffs[i] * i;
    }
    return deriv_coeffs;
}

/**
 * @brief 二分法求根
 * 假定在 [a, b] 内有且仅有一个根
 */
double bisection(const vector<double>& coeffs, double a, double b) {
    double ya = evaluate(coeffs, a);
    // 注意：二分法要求 f(a) 和 f(b) 异号
    
    // 迭代 100 次以达到高精度
    for (int i = 0; i < 100; ++i) {
        double m = (a + b) / 2.0;
        double ym = evaluate(coeffs, m);

        if (abs(ym) < EPSILON) { // 找到了根
            return m;
        }

        if (ym * ya < 0) { // 根在 [a, m]
            b = m;
        } else { // 根在 [m, b]
            a = m;
            ya = ym; // 更新 ya
        }
        
        if (abs(b - a) < EPSILON) { // 区间足够小
            break;
        }
    }
    return (a + b) / 2.0;
}


/**
 * @brief 查找多项式在 [left, right] 区间内的所有根
 * @param coefficients 多项式系数 {c0, c1, c2, ...}
 * @param left 区间左端点
 * @param right 区间右端点
 * @return vector<double> 包含所有根的向量
 */
vector<double> FindRoots(const vector<double>& coefficients, double left, double right) {
    vector<double> roots;

    // 基本情况 1: 常数 P(x) = c0
    if (coefficients.size() == 1) {
        return roots; // 没有根
    }

    // 基本情况 2: 线性 P(x) = c0 + c1*x
    if (coefficients.size() == 2) {
        if (abs(coefficients[1]) > EPSILON) {
            double root = -coefficients[0] / coefficients[1];
            if (root >= left - EPSILON && root <= right + EPSILON) {
                roots.push_back(root);
            }
        }
        return roots;
    }

    // 递归步骤
    // 1. 获取导数的系数
    vector<double> deriv_coeffs = derivative(coefficients);
    // 2. 递归找到导数的根（即 P(x) 的极值点）
    vector<double> deriv_roots = FindRoots(deriv_coeffs, left, right);

    // 3. 创建所有要检查的区间
    vector<double> intervals;
    intervals.push_back(left);
    intervals.insert(intervals.end(), deriv_roots.begin(), deriv_roots.end());
    intervals.push_back(right);

    // 4. 清理区间（排序和去重）
    sort(intervals.begin(), intervals.end());
    intervals.erase(unique(intervals.begin(), intervals.end(),
                      [](double a, double b){ return abs(a-b) < EPSILON; }),
                      intervals.end());

    // 5. 检查每一个单调子区间
    for (size_t i = 0; i < intervals.size() - 1; ++i) {
        double a = intervals[i];
        double b = intervals[i + 1];
        double ya = evaluate(coefficients, a);
        double yb = evaluate(coefficients, b);

        // 检查端点是否是根
        if (abs(ya) < EPSILON) {
            roots.push_back(a);
        }

        // 检查区间内是否有根（异号）
        if (ya * yb < 0) {
            roots.push_back(bisection(coefficients, a, b));
        }
    }

    // 检查最后的端点
    if (abs(evaluate(coefficients, right)) < EPSILON) {
        roots.push_back(right);
    }

    // 6. 清理所有找到的根（排序和去重）
    sort(roots.begin(), roots.end());
    roots.erase(unique(roots.begin(), roots.end(),
                  [](double a, double b){ return abs(a-b) < EPSILON; }),
                  roots.end());
                  
    return roots;
}