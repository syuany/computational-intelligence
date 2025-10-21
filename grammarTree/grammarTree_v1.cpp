#include <iostream>
#include <string>
#include <vector>
#include <sstream>

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

#define IF_CODE 550000
#define ELSE_CODE 560000
#define BLOCK_CODE 570000

#define LEFT_BRACKET 300001
#define RIGHT_BRACKET 300002
#define GREATER_CODE 310000
#define LESS_CODE 310001
#define ADD_CODE 420000
#define MINUS_CODE 420001
#define ASSIGN_CODE 330000
#define SEMICOLON_CODE 340000
#define LEFT_BRACE 350000
#define RIGHT_BRACE 360000

#define BOOKNUM_CODE 200001
#define BAR_CODE 200002
#define BOOK_CODE 100001
#define K_CODE 100002
#define K1_CODE 100003
#define K2_CODE 100004
#define K3_CODE 100005
#define K4_CODE 100006
#define W_CODE 110000
#define N_CODE 111000
#define U_CODE 111100
#define DEST_CODE 120000

using namespace std;

int u = 10;
int bookNum = 200;
int bar = 50;
int dest = 60;
int book = 20;
int k = 300;
int w = 5;
int n = 200;
int k1, k2, k3, k4;

// 假设所有单元间至少有一个空格
// 单元定义为 变量名 或 数字 或 符号
// 假设所有算数运算符( + 和 - )只有两个操作数
string s = R"(if ( bookNum > bar + 100 )
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

struct TNode
{
    vector<TNode *> child;
    int data;

    TNode() : data(0) {}
    TNode(int val) : data(val) {}
};

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
    istringstream iss(s);

    convert(iss, code);

    auto it = code.begin();

    TNode *root = buildTree(it);

    vt(root);

    cout << "u = " << u << endl;
    cout << "bookNum = " << bookNum << endl;
    cout << "bar = " << bar << endl;
    cout << "dest = " << dest << endl;
    cout << "book = " << book << endl;
    cout << "w = " << w << endl;
    cout << "n = " << n << endl;
    cout << "k = " << k << endl;
    cout << "k1 = " << k1 << endl;
    cout << "k2 = " << k2 << endl;
    cout << "k3 = " << k3 << endl;
    cout << "k4 = " << k4 << endl;
    // cout << "unknown = " << unknown << endl;

    deleteTree(root);

    return 0;
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
    switch (code)
    {
    case BOOKNUM_CODE:
        return bookNum;
    case BAR_CODE:
        return bar;
    case BOOK_CODE:
        return book;
    case K_CODE:
        return k;
    case K1_CODE:
        return k1;
    case K2_CODE:
        return k2;
    case K3_CODE:
        return k3;
    case K4_CODE:
        return k4;
    case W_CODE:
        return w;
    case N_CODE:
        return n;
    case U_CODE:
        return u;
    case DEST_CODE:
        return dest;
    default:
        cerr << "unknown code: " << code << endl;
        return unknown;
    }
}

void convert(istringstream &iss, vector<int> &code)
{
    string token;

    while (iss >> token)
    {
        if (token == "if")
        {
            code.push_back(IF_CODE);
        }
        else if (token == "else")
        {
            code.push_back(ELSE_CODE);
        }
        else if (token == "(")
        {
            code.push_back(LEFT_BRACKET);
        }
        else if (token == ")")
        {
            code.push_back(RIGHT_BRACKET);
        }
        else if (token == ">")
        {
            code.push_back(GREATER_CODE);
        }
        else if (token == "<")
        {
            code.push_back(LESS_CODE);
        }
        else if (token == "+")
        {
            code.push_back(ADD_CODE);
        }
        else if (token == "-")
        {
            code.push_back(MINUS_CODE);
        }
        else if (token == "=")
        {
            code.push_back(ASSIGN_CODE);
        }
        else if (token == ";")
        {
            code.push_back(SEMICOLON_CODE);
        }
        else if (token == "{")
        {
            code.push_back(LEFT_BRACE);
        }
        else if (token == "}")
        {
            code.push_back(RIGHT_BRACE);
        }
        else if (token == "bookNum")
        {
            code.push_back(BOOKNUM_CODE);
        }
        else if (token == "bar")
        {
            code.push_back(BAR_CODE);
        }
        else if (token == "book")
        {
            code.push_back(BOOK_CODE);
        }
        else if (token == "k")
        {
            code.push_back(K_CODE);
        }
        else if (token == "k1")
        {
            code.push_back(K1_CODE);
        }
        else if (token == "k2")
        {
            code.push_back(K2_CODE);
        }
        else if (token == "k3")
        {
            code.push_back(K3_CODE);
        }
        else if (token == "k4")
        {
            code.push_back(K4_CODE);
        }
        else if (token == "w")
        {
            code.push_back(W_CODE);
        }
        else if (token == "n")
        {
            code.push_back(N_CODE);
        }
        else if (token == "u")
        {
            code.push_back(U_CODE);
        }
        else if (token == "dest")
        {
            code.push_back(DEST_CODE);
        }
        else
        {
            bool isNumber = true;
            for (char c : token)
            {
                if (!isdigit(c))
                {
                    isNumber = false;
                    break;
                }
            }
            if (isNumber)
            {
                int numVal = stoi(token);
                code.push_back(numVal);
            }
            else
            {
                cout << "not valid: '" << token << "'" << endl;
            }
        }
    }
}