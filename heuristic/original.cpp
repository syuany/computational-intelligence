#include<string>
#include<chrono>
#include<cstdio>
#include<vector>

int p[] = { 565,478,124,537,371,435,439,228,302,246,273,549,541,500,305,144,102,204,471,292,439,545,160,181,457,336,268,167,536,268,280,345,241,171,480,358,268,344,530,235,381,459,565,352,251,447,307,190,113,577,207,340,263,118,324,531,183,117,193,471 };
int c[] = { 266,205,323,192,191,356,340,164,194,202,201,109,213,282,339,229,398,123,326,106,196,288,378,131,252,316,183,175,389,358,130,206,129,110,130,154,102,239,135,283,126,208,322,219,135,395,335,180,327,312,217,253,277,147,362,360,375,297,161,363 };

int BC = 3931;
int N = 30;
std::vector<int> x(N);
std::vector<int> a(N);
int ans = 0;
int ans_c = 0;

int main(){
    auto dfs = [&](this auto &&dfs, int t, int cc, int cp)
    {
        if(t == N)
        {
            if(cp>ans && cc<=BC)
            {
                ans = cp;
                ans_c = cc;
                x = a;
            }

            return;
        }
        if(cc + c[t] <= BC)
        {
            a[t] = 1;
            dfs(t + 1, cc + c[t], cp + p[t]);
        }
        a[t] = 0;
        dfs(t + 1, cc, cp);
    };

    auto start = std::chrono::steady_clock::now();

    dfs(0, 0, 0);

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    printf("cp: %d\n", ans);
    printf("x: ");
    // printf("选择：");
    for (int i = 0; i < N;i++)
    {
        printf("%d ", x[i]);
    }
    printf("\ncc: %d", ans_c);
    printf("\nBC: %d", BC);
    printf("\ntime: %.3f s\n", duration.count()/1000.0);

    return 0;
}