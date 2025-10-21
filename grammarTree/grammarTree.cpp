#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>

using namespace std;

// 1-100000 为常量
// 100001-300000为变量
// 300001-500000 为符号
// 500001-600000 为关键字
#define CON_LB 1
#define CON_UB 100000
#define VAR_LB 100001
#define VAR_UB 300000
#define CHA_LB 300001
#define CHA_UB 400000
#define CHA_OPT_LB 400001
#define CHA_OPT_UB 500000
#define KEY_LB 500001
#define KEY_UB 600000

int conCounter = CON_LB;
int varCounter = VAR_LB;
int chaCounter = CHA_LB;
int chaOptCounter = CHA_OPT_LB;
int keyCounter = KEY_LB;

// 假设所有单元间至少有一个空格
// 单元定义为 变量名 或 数字 或 符号
// 假设所有算数运算符( + 和 - )只有两个操作数
string s1 = R"(u = 10;
bookNum = 200;
bar = 50;
dest = 60;
book = 20;
k = 300;
w = 5;
n = 200;
if ( bookNum > bar + 100 )
{
    if ( n - 6 < 90 + dest )
    {
        k1 = 100 - bar ;
        k2 = k1 + dest ;
        k3 = k2 - w ;
        k4 = k3 + u ;
        if ( k > book )
        {
            k = book + 100 ;
        }
        else
        {
            book = k - 20 ;
        }
    }
    else
    {
        w = w + 6 ;
    }
}
)";

string s2 = R"(u  =  10;
mycthenumt = 200 ;
dock345 = 50 ;
mypad56 = 60 ;
apple123 = 20 ;
k = 300 + mypad56 ;
w = 5 ;
thenum = 200 ;
if ( mycthenumt > dock345 + 100 )
{
    if ( thenum - 60 < 90 + mypad56 )
    {
        k1 = 100 - dock345;
        k2 = k1 + mypad56;
        k3 = k2- w;
        k4 = k3+ u;
        if (k > apple123 + 350 )
        {
             k = apple123 + 100;
             k2 = k + mypad56;
             k3 =  k2 + thenum;
             k4 = k3+  k2;
        }
        else
        {
             apple123 = k - 20;
             k2 =  apple123 - w;
             k3 =  k2 + thenum;
             k4 = k3-  k2;
        }
    }
    else 
    {
        w = w + 60;
         k2 =  dock345 - w;
         k3 =  k2 + mycthenumt;
         k4 = k3-  k2;
    }
}
)";

struct TNode
{
    vector<TNode *> child;
    int data;

    TNode() : data(0) {}
    TNode(int val) : data(val) {}
};

struct CodeRange
{
    int con_lb;
    int con_ub;
    int var_lb;
    int var_ub;
    int sym_lb;
    int sym_ub;
    int opt_lb;
    int opt_ub;
    int key_lb;
    int key_ub;
} codeRange = {
    1, 100000,
    100001, 300000,
    300001, 400000,
    400001, 500000,
    500001, 600000

};

unordered_map<string, int> varToCode;
unordered_map<string, int> strToCode;
unordered_map<int, int> varCodeToVal;

void convert(istringstream &iss, vector<int> &code);
TNode *buildTree(vector<int>::iterator &it);
TNode *Statement(vector<int>::iterator &it);
TNode *BlockNode(vector<int>::iterator &it);
TNode *AssignExp(vector<int>::iterator &it);
TNode *CompareExp(vector<int>::iterator &it);
TNode *ArithExp(vector<int>::iterator &it);
int &getVarVal(int Code);
int vt(TNode *p);
void deleteTree(TNode *node);

vector<int> code;
int unknown = -1;

int main()
{
    istringstream iss(s1);

    convert(iss, code);

    auto it = code.begin();

    TNode *root = buildTree(it);

    vt(root);

    deleteTree(root);

    return 0;
}

vector<string> tokenize(string &input)
{
    vector<string> tokens;
    string token;
    for (auto c : input)
    {
        if (isspace(c))
        {
            if (!token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }
        }
        else if (ispunct(c))
        {
            if (!token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back(string(1, c));
        }
        else
        {
            token += c;
        }
    }
    if (!token.empty())
    {
        tokens.push_back(token);
    }
    return tokens;
}

void convert(vector<string> tokens, vector<int> &code)
{
    for (const auto &token : tokens)
    {
        if (strToCode.count(token))
        {
            code.push_back(strToCode[token]);
        }
        else if (all_of(token.begin(), token.end(), ::isdigit))
        {
            int num = stoi(token);
            if (num >= codeRange.con_lb && num <= codeRange.con_ub)
            {
                code.push_back(num);
            }
            else
            {
                cerr << "constant out of range: " << token << endl;
            }
        }
        else
        {
            int newVarCode =
        }
    }
}

void deleteTree(TNode *node)
{
    if (!node)
        return;
    for (auto child : node->child)
    {
        deleteTree(child);
    }
    delete node;
}

TNode *buildTree(vector<int>::iterator &it)
{
    if (it == code.end())
        return nullptr;

    if (IF_CODE == *it)
    {
        TNode *ifNode = new TNode(IF_CODE);
        ++it;
        if (*it == LEFT_BRACKET)
            ++it;
        TNode *cond = CompareExp(it);
        ifNode->child.push_back(cond);
        if (*it == RIGHT_BRACKET)
            ++it;
        TNode *t = BlockNode(it);
        ifNode->child.push_back(t);

        if (it != code.end() && ELSE_CODE == *it)
        {
            TNode *f = BlockNode(++it);
            ifNode->child.push_back(f);
        }

        return ifNode;
    }
    return Statement(it);
}

TNode *CompareExp(vector<int>::iterator &it)
{
    if (it == code.end())
        return nullptr;

    TNode *left = ArithExp(it);
    if (!left)
        return nullptr;

    if (it == code.end() || (*it != GREATER_CODE && *it != LESS_CODE))
        return left;

    int opt = *it;
    TNode *compNode = new TNode(opt);
    compNode->child.push_back(left);

    ++it;
    TNode *right = ArithExp(it);
    if (!right)
        return nullptr;

    compNode->child.push_back(right);
    return compNode;
}

TNode *BlockNode(vector<int>::iterator &it)
{
    if (it == code.end())
        return nullptr;

    TNode *block = new TNode(BLOCK_CODE);
    if (LEFT_BRACE == *it)
        ++it;
    while (it != code.end() && *it != RIGHT_BRACE)
    {
        TNode *t = Statement(it);
        if (t)
            block->child.push_back(t);
    }
    if (it != code.end() && *it == RIGHT_BRACE)
        ++it;
    return block;
}

TNode *Statement(vector<int>::iterator &it)
{
    if (it == code.end())
        return nullptr;

    if (*it >= KEY_LB && *it <= KEY_UB)
    {
        return buildTree(it);
    }
    else if (*it >= VAR_LB && *it <= VAR_UB)
    {
        return AssignExp(it);
    }
    else if (*it == SEMICOLON_CODE)
    {
        ++it;
        return nullptr;
    }
    ++it;
    return nullptr;
}

TNode *ArithExp(vector<int>::iterator &it)
{
    if (it == code.end())
        return nullptr;

    TNode *left;
    if (*it >= CON_LB && *it <= CON_UB)
    {
        left = new TNode(*it);
        ++it;
    }
    else if (*it >= VAR_LB && *it <= VAR_UB)
    {
        left = new TNode(*it);
        ++it;
    }
    else
    {
        return nullptr;
    }

    if (it != code.end() && (*it >= CHA_OPT_LB && *it <= CHA_OPT_UB))
    {
        TNode *opt = new TNode(*it);
        ++it;

        TNode *right = ArithExp(it);
        if (!right)
            return left;

        opt->child.push_back(left);
        opt->child.push_back(right);
        return opt;
    }

    return left;
}

TNode *AssignExp(vector<int>::iterator &it)
{
    if (it == code.end())
        return nullptr;

    TNode *left = new TNode(*it);
    ++it;
    if (it == code.end() || *it != ASSIGN_CODE)
        return nullptr;
    TNode *assiNode = new TNode(*it);
    TNode *right = ArithExp(++it);
    assiNode->child.push_back(left);
    assiNode->child.push_back(right);
    return assiNode;
}

int vt(TNode *p)
{
    if (!p)
        return 0;

    if (p->data == IF_CODE)
    {
        int condResult = vt(p->child[0]);
        if (condResult)
        {
            vt(p->child[1]);
        }
        else if (p->child.size() > 2)
        {
            vt(p->child[2]);
        }
        return 0;
    }
    if (p->data == BLOCK_CODE)
    {
        for (auto childNode : p->child)
        {
            vt(childNode);
        }
        return 0;
    }
    if (p->data == GREATER_CODE || p->data == LESS_CODE)
    {
        int left = vt(p->child[0]);
        int right = vt(p->child[1]);
        if (p->data == GREATER_CODE)
        {
            return left > right ? 1 : 0;
        }
        else
        {
            return left < right ? 1 : 0;
        }
    }
    if (p->data == MINUS_CODE || p->data == ADD_CODE)
    {
        int left = vt(p->child[0]);
        int right = vt(p->child[1]);
        if (p->data == MINUS_CODE)
        {
            return left - right;
        }
        else
        {
            return left + right;
        }
    }
    if (p->data == ASSIGN_CODE)
    {
        int varCode = p->child[0]->data;
        int val = vt(p->child[1]);
        getVarVal(varCode) = val;
        return val;
    }
    if (p->data >= CON_LB && p->data <= CON_UB)
    {
        return p->data;
    }
    if (p->data >= VAR_LB && p->data <= VAR_UB)
    {
        return getVarVal(p->data);
    }
    return 0;
}

int &getVarVal(int code)
{
}
