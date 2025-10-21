#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

const int CN = 60;
const double FAIL_WEIGHT = 1;
const int SORT_THRESHOLD = 66;

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
    }

    int found = 0;

    auto sort_order = [&]() {
        sort(order.begin(), order.end(), [&](int a, int b) {
            double priority_a = weight_sum[a] + fail_count[a] * FAIL_WEIGHT;
            double priority_b = weight_sum[b] + fail_count[b] * FAIL_WEIGHT;
            return priority_a > priority_b;
        });
    };

    auto isValidColoring = [&](const vector<int> &coloring) {
        for (int i = 1; i <= v; i++) {
            for (int j = i + 1; j <= v; j++) {
                if (map[i][j] && abs(coloring[i] - coloring[j]) < map[i][j]) {
                    return false;
                }
            }
        }
        return true;
    };

    auto isRightColor = [&](int node, const vector<int> &processed) {
        for (int x : processed) {
            if (map[node][x] && abs(c[node] - c[x]) < map[node][x])
                return false;
        }
        return true;
    };

    auto vt = [&](this auto &&vt, int t) -> bool {
        if (t == v) {
            if (isValidColoring(c)) {
                best = c;
                found = 1;
                return true;
            }
            return false;
        }

        int cur_node = order[t];
        bool success = false;

        for (int color = 1; color < CN; color++) {
            c[cur_node] = color;
            if (isRightColor(cur_node, processed)) {
                success = true;
                processed.push_back(cur_node);
                if (vt(t + 1)) {
                    processed.pop_back();
                    return true;
                }
                processed.pop_back();
            } else {
                fail_count[cur_node]++;
            }

            if (fail_count[cur_node] % SORT_THRESHOLD == 0) {
                sort_order();
            }
        }

        if (!success && CN > 1) {
            fail_count[cur_node]++;
        }

        return false;
    };

    sort_order();
    vt(0);

    if (found) {
        ofstream outFile("result.txt");

        for (int i = 0; i < best.size(); i++) {
            outFile << best[i] << " ";
            if (i % 10 == 0) outFile << endl;
        }

        outFile << "Node\t\tColor\t\tFailures" << endl;
        outFile << "----\t\t-----\t\t--------" << endl;
        for (int i = 1; i <= v; i++) {
            outFile << i << "\t\t\t" << best[i] << "\t\t\t" << fail_count[i] << endl;
        }

        outFile.close();
    } else {
        ofstream outFile("result.txt");
        outFile << "solution not found" << endl;
        outFile.close();
    }

    return 0;
}