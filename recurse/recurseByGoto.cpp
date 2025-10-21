#include<chrono>
#include<cstdio>
#include<vector>
#include<stack>

int p[] = { 565,478,124,537,371,435,439,228,302,246,273,549,541,500,305,144,102,204,471,292,439,545,160,181,457,336,268,167,536,268,280,345,241,171,480,358,268,344,530,235,381,459,565,352,251,447,307,190,113,577,207,340,263,118,324,531,183,117,193,471 };
int c[] = { 266,205,323,192,191,356,340,164,194,202,201,109,213,282,339,229,398,123,326,106,196,288,378,131,252,316,183,175,389,358,130,206,129,110,130,154,102,239,135,283,126,208,322,219,135,395,335,180,327,312,217,253,277,147,362,360,375,297,161,363 };

int BC = 3931;
int N = 30;
int ans = 0;
int ans_c = 0;

struct State{
    int t;
    int cc;
    int cp;
};

std::stack<State> s;
std::vector<int> x(N);
std::vector<int> a(N);

void push(int t, int cc, int cp);
void pop();
State &get_top();

int main(){
    auto start = std::chrono::steady_clock::now();

    push(0, 0, 0);
    
dfs_start:
    State current = get_top();
    int t = current.t;
    int cc = current.cc;
    int cp = current.cp;
    
    if(t == N)
    {
        if(cp > ans && cc <= BC)
        {
            ans = cp;
            ans_c = cc;
            x = a;
        }
        goto backtrack;
    }
    
    if(cc + c[t] <= BC)
    {
        a[t] = 1;
        push(t + 1, cc + c[t], cp + p[t]);
        goto dfs_start;
        
back_from_select:
    }
    
    a[t] = 0;
    push(t + 1, cc, cp);
    goto dfs_start;
    
back_from_unselect:
    
backtrack:
    pop();
    
    if(s.empty()) {
        goto dfs_end;
    }
    
    current = get_top();
    t = current.t;
    cc = current.cc;
    cp = current.cp;
    if( t < N && a[t] == 1) {
        goto back_from_select;
    } else {
        goto back_from_unselect;
    }

dfs_end:
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    printf("cp: %d\n", ans);
    printf("x: ");
    for (int i = 0; i < N;i++)
    {
        printf("%d ", x[i]);
    }
    printf("\ncc: %d", ans_c);
    printf("\nBC: %d", BC);
    printf("\ntime: %.3f s\n", duration.count()/1000.0);

    return 0;
}

void push(int t, int cc, int cp){
    State current = {t, cc, cp};
    s.push(current);
}

void pop(){
    s.pop();
}

State& get_top(){
    return s.top();
}