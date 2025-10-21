#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <numeric>

using namespace std;

/**
 * 评估图染色解的正确性
 * @param v 节点数量（1-based编号）
 * @param map 邻接矩阵（map[i][j]为节点i与j的边权，0表示无直接边）
 * @param colors 颜色分配结果（colors[i]为节点i的颜色）
 * @return 是否所有约束都满足（true=正确，false=存在错误）
 */
bool evaluateSolution(int v, const vector<vector<int>> &map, const vector<int> &colors) {
    bool isCorrect = true;
    int errorCount = 0; // 记录不满足约束的边数

    cout << "=== 解的正确性评估 ===" << endl;
    cout << "检查所有边的约束：|颜色i - 颜色j| ≥ 边权值" << endl
         << endl;

    // 遍历所有节点对（i < j，避免重复检查无向图的边）
    for (int i = 1; i <= v; ++i) {
        for (int j = i + 1; j <= v; ++j) {
            int weight = map[i][j];
            if (weight == 0) continue; // 无直接边，跳过

            // 计算实际颜色差
            int colorDiff = abs(colors[i] - colors[j]);

            // 检查约束是否满足
            if (colorDiff < weight) {
                isCorrect = false;
                errorCount++;
                // 输出错误详情
                cout << "错误 " << setw(2) << errorCount << "："
                     << "节点" << i << "与节点" << j
                     << "（边权=" << weight << "）" << endl
                     << "  - 颜色：节点" << i << "=" << colors[i]
                     << "，节点" << j << "=" << colors[j] << endl
                     << "  - 实际颜色差：" << colorDiff
                     << "（要求 ≥ " << weight << "）" << endl
                     << endl;
            }
        }
    }

    // 输出评估结果
    if (isCorrect) {
        cout << "评估结果：所有边的约束均满足，解正确！" << endl;
    } else {
        cout << "评估结果：共发现" << errorCount << "处约束不满足，解错误。" << endl;
    }
    return isCorrect;
}

// --------------------------
// 示例用法
// --------------------------
int main() {
    ifstream file("GraphData120.txt");
    int v, n;
    file >> v >> n;
    vector<vector<int>> map(v + 1, vector<int>(v + 1));
    for (int k = 0; k < n; k++) {
        int i, j, val;
        file >> i >> j >> val;
        if (i > v || i < 1 || j > v || j < 1) {
            cerr << "index out of range";
            return 1;
        }
        map[i][j] = map[j][i] = val;
    }

    // 示例：3个节点的图（测试用）
    // int v = 3;
    // vector<vector<int>> map(v + 1, vector<int>(v + 1, 0));
    // // 边：1-2（权值3），2-3（权值2），1-3（权值1）
    // map[1][2] = map[2][1] = 3;
    // map[2][3] = map[3][2] = 2;
    // map[1][3] = map[3][1] = 1;

    // // 测试解1：正确解（颜色差均满足约束）
    vector<int> correctColors(v + 1); // 0号元素 unused
    ifstream file1("test.txt");
    for (int i = 0; i <= v; i++) {
        file1 >> correctColors[i];
    }

    cout << "=== 测试正确解 ===" << endl;
    evaluateSolution(v, map, correctColors);

    // // 测试解2：错误解（1-2颜色差不满足）
    // vector<int> wrongColors = {0, 1, 3, 3};
    // cout << endl
    //      << "=== 测试错误解 ===" << endl;
    // evaluateSolution(v, map, wrongColors);

    return 0;
}