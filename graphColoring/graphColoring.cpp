#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

const int CN = 60;
const double FAIL_WEIGHT = 0.3;
const int SORT_THRESHOLD = 10;

int main() {
    ifstream file("GraphData120.txt");

    int v, n;
    file >> v >> n;
    vector<int> c(v + 1);
    vector<int> best(v + 1);
    vector<int> weight_sum(v + 1);
    vector<int> fail_count(v + 1);
    vector<int> order(v);
    vector<int> processed;
    vector<vector<int>> map(v + 1, vector<int>(v + 1));
    vector<vector<int>> neighbors(v + 1);
    iota(order.begin(), order.end(), 1);

    for (int k = 0; k < n; k++) {
        int i, j, val;
        file >> i >> j >> val;
        if (i > v || i < 1 || j > v || j < 1) {
            cerr << "index out of range";
            return 1;
        }
        map[i][j] = map[j][i] = val;
        weight_sum[i] += val;
        weight_sum[j] += val;
        neighbors[i].push_back(j);
        neighbors[j].push_back(i);
    }

    int found = 0, min_max_col = CN;

    auto sort_order = [&]() {
        sort(order.begin(), order.end(), [&](int a, int b) {
            double priority_a = weight_sum[a] + fail_count[a] * FAIL_WEIGHT;
            double priority_b = weight_sum[b] + fail_count[b] * FAIL_WEIGHT;
            return priority_a > priority_b;
        });
    };
    auto isRightColor = [&](int node, const vector<int> &processed) {
        for (int neighbor : neighbors[node]) {
            if (find(processed.begin(), processed.end(), neighbor) != processed.end()) {
                if (abs(c[node] - c[neighbor]) < map[node][neighbor])
                    return false;
            }
        }
        return true;
    };
    auto get_possible_colors = [&](int node, const vector<int> &processed) {
        vector<int> forbidden;
        for (int neighbor : neighbors[node]) {
            if (find(processed.begin(), processed.end(), neighbor) != processed.end()) {
                int min_diff = map[node][neighbor];
                int nb_color = c[neighbor];
                // 禁止的颜色范围：(nb_color - min_diff, nb_color + min_diff)
                forbidden.push_back(nb_color - min_diff + 1);
                forbidden.push_back(nb_color + min_diff - 1);
            }
        }
        // 可行颜色：1~min_max_col中，不在任何禁止范围内的颜色
        vector<int> possible;
        for (int color = 1; color <= min_max_col; color++) {
            bool ok = true;
            for (size_t i = 0; i < forbidden.size(); i += 2) {
                int low = forbidden[i];
                int high = forbidden[i + 1];
                if (color >= low && color <= high) {
                    ok = false;
                    break;
                }
            }
            if (ok) possible.push_back(color);
        }
        return possible;
    };
    auto vt = [&](this auto &&vt, int t) {
        if (t == v) {
            int cur_col = ranges::max(c);
            if (!found || cur_col < min_max_col) {
                min_max_col = cur_col;
                best = c;
                found = 1;
            }
            return;
        }

        int cur_node = order[t], all_failed = 1;
        vector<int> possible_colors = get_possible_colors(cur_node, processed);
        if (possible_colors.empty()) {
            fail_count[cur_node]++;
            if (fail_count[cur_node] % SORT_THRESHOLD == 0) {
                sort_order();
            }
            return;
        }

        for (int color : possible_colors) {
            c[cur_node] = color;

            if (isRightColor(cur_node, processed)) {
                all_failed = 0;
                processed.push_back(cur_node);
                vt(t + 1);
                processed.pop_back();
            }
        }
        if (all_failed) {
            fail_count[cur_node]++;
            if (fail_count[cur_node] % SORT_THRESHOLD == 0) {
                sort_order();
            }
        }
    };

    sort_order();
    vt(0);

    if (found) {
        ofstream outFile("result.txt");

        outFile << "Node\t\tColor\t\tFailed" << endl;
        outFile << "----\t\t-----\t\t------" << endl;

        for (int i = 1; i <= v; i++) {
            outFile << i << "\t\t" << best[i] << "\t\t" << fail_count[i] << endl;
        }
        outFile << "Total colors used: " << min_max_col << endl;

        outFile.close();
    } else {
        ofstream outFile("result.txt");
        outFile << "solution not found" << endl;
        outFile.close();
    }
    // if (found) {
    //     ofstream outFile("test.txt");
    //     for (int x : best) outFile << x << endl;
    //     outFile.close();
    // }

    return 0;
}