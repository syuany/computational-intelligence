#include <iostream>
#include <fstream>
#include <vector>

struct Item
{
    int perf, cost, energy, num;
    double ratio;
    bool operator<(const Item &oth) const { return ratio > oth.ratio; }
};

int main()
{
    std::ifstream file("data200.txt");

    int N, BC, BE;
    file >> N >> BC >> BE;
    std::vector<int> P(N);
    std::vector<int> C(N);
    std::vector<int> E(N);
    for (int i = 0; i < N; i++)
        file >> P[i];
    for (int i = 0; i < N; i++)
        file >> C[i];
    for (int i = 0; i < N; i++)
        file >> E[i];

    std::vector<Item> items(N);
    for (int i = 0; i < N; i++)
        items[i] = {P[i], C[i], E[i], i, static_cast<double>(P[i]) / (C[i] + E[i])};
    sort(items.begin(), items.end());

    std::vector<int> x(N);
    std::vector<int> a(N);
    int ans = 0, ans_c = 0, ans_e = 0;

    auto dfs = [&](this auto &&dfs, int t, int cc, int ce, int cp)
    {
        if (t == N)
        {
            if (cp > ans && cc <= BC && ce <= BE)
            {
                ans = cp;
                ans_c = cc;
                ans_e = ce;
                x = a;
            }
            return;
        }

        int bound = cp, temp_cc = cc, temp_ce = ce;
        for (int i = t; i < N; i++)
        {
            if (temp_cc + items[i].cost <= BC && temp_ce + items[i].energy <= BE)
            {
                temp_cc += items[i].cost;
                temp_ce += items[i].energy;
                bound += items[i].perf;
            }
        }
        if (bound <= ans)
            return;

        if (cc + items[t].cost <= BC && ce + items[t].energy <= BE)
        {
            a[t] = 1;
            dfs(t + 1, cc + items[t].cost, ce + items[t].energy, cp + items[t].perf);
        }
        a[t] = 0;
        dfs(t + 1, cc, ce, cp);
    };

    dfs(0, 0, 0, 0);
    std::cout << "g_BestP: " << ans << std::endl;
    std::cout << "CC: " << ans_c << "< BC: " << BC << std::endl;
    std::cout << "CE: " << ans_e << "< BE: " << BE << std::endl;

    return 0;
}