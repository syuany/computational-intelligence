#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

#define IF_CODE 51000
#define ELSE_CODE 51100
#define BLOCK_CODE 51110
#define WHILE_CODE 52000
#define FOR_CODE 52001

#define SEMICOLON_CODE 41000
#define LEFT_BRACKET 40001
#define RIGHT_BRACKET 40002
#define LEFT_BRACE 40003
#define RIGHT_BRACE 40004
#define GREATER_CODE 42000
#define LESS_CODE 42001

#define ADD_CODE 32000
#define MINUS_CODE 32001
#define ASSIGN_CODE 33000

// 0-10000 为常量
// 10001-30000 为变量
// 30001-50000 为符号
// 50001-60000 为关键字
struct CodeRange {
    int con_lb, con_ub, con_cnt; // 常量
    int var_lb, var_ub, var_cnt; // 变量
    int opt_lb, opt_ub, opt_cnt; // 运算符
    int pun_lb, pun_ub, pun_cnt; // 字符
    int key_lb, key_ub, key_cnt; // 关键字
} codeRange = {
    0, 10000, 0,
    10001, 30000, 0,
    30001, 40000, 0,
    40001, 50000, 0,
    50001, 60000, 0

};

struct TNode {
    vector<TNode *> child;
    int data;

    TNode(int val = 0) : data(val) {}
};

unordered_map<int, string> codeToName; // 变量表
unordered_map<int, string> codeToCons; // 常量表
unordered_map<string, int> strToCode;  // 其他字符表
unordered_map<int, int> varValue;

vector<int> code;

vector<string> tokenize(string &input);
void init();
void convert(vector<string> tokens);
TNode *buildTree(vector<int>::iterator &it);
TNode *Statement(vector<int>::iterator &it);
TNode *BlockNode(vector<int>::iterator &it);
TNode *AssignExp(vector<int>::iterator &it);
TNode *CompareExp(vector<int>::iterator &it);
TNode *ArithExp(vector<int>::iterator &it);
TNode *IfExp(vector<int>::iterator &it);
TNode *WhileExp(vector<int>::iterator &it);
TNode *ForExp(vector<int>::iterator &it);
int vt(TNode *p);
void deleteTree(TNode *node);
vector<string> readFiles(const string &dirPath);
string readFile(const string &filepath);
void printVar(ofstream &out);
string getNodeDescription(int code);
void printTree(TNode *root, ofstream &out);
void printNode(TNode *node, ofstream &out, int depth = 0);

int main() {
    // 从文件读入
    string dirPath = "txt";
    vector<string> ss = readFiles(dirPath);

    for (int i = 0; i < ss.size(); i++) {
        string outputFileName = "res" + to_string(i + 1) + ".txt";
        ofstream outFile(outputFileName);
        string &s = ss[i];

        init();
        vector<string> tokens = tokenize(s);
        convert(tokens);
        auto it = code.begin();
        TNode *root = buildTree(it);

        printTree(root, outFile);

        vt(root);

        printVar(outFile);

        deleteTree(root);
        outFile.close();
    }

    return 0;
}

void printTree(TNode *root, ofstream &out) {
    out << "| " << root->data << ": ROOT" << endl;
    for (auto child : root->child) {
        printNode(child, out, 1);
    }
}

string getNodeDescription(int code) {
    // 关键字
    if (code == IF_CODE) return "IF";
    if (code == ELSE_CODE) return "ELSE";
    if (code == BLOCK_CODE) return "BLOCK";
    if (code == WHILE_CODE) return "WHILE";
    if (code == FOR_CODE) return "FOR";

    // 标点符号
    if (code == SEMICOLON_CODE) return ";";
    if (code == LEFT_BRACKET) return "(";
    if (code == RIGHT_BRACKET) return ")";
    if (code == LEFT_BRACE) return "{";
    if (code == RIGHT_BRACE) return "}";

    // 运算符
    if (code == GREATER_CODE) return "GREAT";
    if (code == LESS_CODE) return "LESS";
    if (code == ADD_CODE) return "ADD";
    if (code == MINUS_CODE) return "SUB";
    if (code == ASSIGN_CODE) return "ASSI";

    // 常量
    if (code >= codeRange.con_lb && code <= codeRange.con_ub) {
        auto it = codeToCons.find(code);
        if (it != codeToCons.end()) {
            return it->second;
        }
        return "const_" + to_string(code);
    }

    // 变量
    if (code >= codeRange.var_lb && code <= codeRange.var_ub) {
        auto it = codeToName.find(code);
        if (it != codeToName.end()) {
            return it->second;
        }
        return "var_" + to_string(code);
    }

    return "UNKNOWN_" + to_string(code);
}

void printNode(TNode *node, ofstream &out, int depth) {
    if (!node) return;

    for (int i = 0; i < depth; i++) {
        out << "|——";
    }
    out << "|";

    string nodeDesc = getNodeDescription(node->data);
    out << " " << node->data << ": " << nodeDesc << endl;

    for (auto child : node->child) {
        printNode(child, out, depth + 1);
    }
}

void printVar(ofstream &out) {
    out << endl;
    for (const auto &[code, val] : varValue) {
        string name = codeToName[code];
        out << name << ": " << val << endl;
    }
}

string readFile(const string &filepath) {
    ifstream file(filepath);
    if (!file.is_open()) {
        cerr << "open file failed: " << filepath << endl;
        return "";
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

vector<string> readFiles(const string &dirPath) {
    vector<string> ss;
    try {
        for (const auto &entry : fs::directory_iterator(dirPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                string s = readFile(entry.path().string());
                if (!s.empty()) {
                    ss.push_back(s);
                }
            }
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << "file system error: " << e.what() << endl;
    }
    return ss;
}

// 可处理单元间无空格的情况
vector<string> tokenize(string &input) {
    vector<string> tokens;
    string token;
    for (auto c : input) {
        // 空格作为一个单元的结束符
        if (isspace(c)) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        }
        else if (ispunct(c)) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back(string(1, c));
        }
        else {
            token += c;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

// 由于无法区分关键字和变量名，需要先注册关键字
void init() {
    code.clear();
    codeToName.clear();
    codeToCons.clear();
    strToCode.clear();
    varValue.clear();
    codeRange.con_cnt = 0;
    codeRange.var_cnt = 0;
    codeRange.opt_cnt = 0;
    codeRange.pun_cnt = 0;
    codeRange.key_cnt = 0;
    strToCode["if"] = IF_CODE;
    strToCode["else"] = ELSE_CODE;
    strToCode["while"] = WHILE_CODE;
    strToCode["for"] = FOR_CODE;
    codeRange.key_cnt += 4;
    strToCode[";"] = SEMICOLON_CODE;
    strToCode["("] = LEFT_BRACKET;
    strToCode[")"] = RIGHT_BRACKET;
    strToCode["{"] = LEFT_BRACE;
    strToCode["}"] = RIGHT_BRACE;
    codeRange.pun_cnt += 5;
    strToCode[">"] = GREATER_CODE;
    strToCode["<"] = LESS_CODE;
    strToCode["+"] = ADD_CODE;
    strToCode["-"] = MINUS_CODE;
    strToCode["="] = ASSIGN_CODE;
    codeRange.opt_cnt += 5;
}

void convert(vector<string> tokens) {
    unordered_map<string, int> registeredCon;
    unordered_map<string, int> registeredVar;
    for (const auto &token : tokens) {
        if (strToCode.count(token)) {
            code.push_back(strToCode[token]);
        }
        else if (all_of(token.begin(), token.end(), ::isdigit)) {
            // 处理常量
            if (registeredCon.count(token)) {
                code.push_back(registeredCon[token]);
            }
            else {
                int co = codeRange.con_cnt + codeRange.con_lb;
                if (co <= codeRange.con_ub) {
                    codeRange.con_cnt++;
                    codeToCons[co] = token;
                    registeredCon[token] = co;
                    code.push_back(co);
                }
                else {
                    cerr << "constants table full: " << token << endl;
                }
            }
        }
        else {
            // 处理变量
            if (registeredVar.count(token)) {
                code.push_back(registeredVar[token]);
            }
            else {
                int co = codeRange.var_cnt + codeRange.var_lb;
                if (co <= codeRange.var_ub) {
                    codeRange.var_cnt++;
                    codeToName[co] = token;
                    registeredVar[token] = co;
                    code.push_back(co);
                }
                else {
                    cerr << "variables table full: " << token << endl;
                }
            }
        }
    }
}

void deleteTree(TNode *node) {
    if (!node)
        return;
    for (auto child : node->child) {
        deleteTree(child);
    }
    delete node;
}

TNode *buildTree(vector<int>::iterator &it) {
    if (it == code.end())
        return nullptr;

    TNode *rootBlock = new TNode(BLOCK_CODE);

    while (it != code.end()) {
        TNode *stmt = Statement(it);
        if (stmt) {
            rootBlock->child.push_back(stmt);
        }
    }

    return rootBlock;
}

TNode *CompareExp(vector<int>::iterator &it) {
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

TNode *BlockNode(vector<int>::iterator &it) {
    if (it == code.end())
        return nullptr;

    TNode *block = new TNode(BLOCK_CODE);
    if (LEFT_BRACE == *it)
        ++it;
    while (it != code.end() && *it != RIGHT_BRACE) {
        TNode *t = Statement(it);
        if (t)
            block->child.push_back(t);
    }
    if (it != code.end() && *it == RIGHT_BRACE)
        ++it;
    return block;
}

TNode *IfExp(vector<int>::iterator &it) {
    if (it == code.end())
        return nullptr;

    TNode *ifNode = new TNode(*it);
    ++it;
    if (*it == LEFT_BRACKET) ++it;

    TNode *cond = CompareExp(it);
    ifNode->child.push_back(cond);
    if (*it == RIGHT_BRACKET) ++it;

    TNode *blo = BlockNode(it);
    ifNode->child.push_back(blo);

    if (it != code.end() && ELSE_CODE == *it) {
        TNode *f = BlockNode(++it);
        ifNode->child.push_back(f);
    }

    return ifNode;
}

TNode *WhileExp(vector<int>::iterator &it) {
    if (it == code.end())
        return nullptr;

    TNode *whileNode = new TNode(*it);
    ++it;
    if (*it == LEFT_BRACKET) ++it;

    TNode *cond = CompareExp(it);
    whileNode->child.push_back(cond);
    if (*it == RIGHT_BRACKET) ++it;

    TNode *blo = BlockNode(it);
    whileNode->child.push_back(blo);

    return whileNode;
}

TNode *ForExp(vector<int>::iterator &it) {
    if (it == code.end())
        return nullptr;

    TNode *forNode = new TNode(*it); // FOR_CODE
    ++it;
    if (it == code.end() || *it != LEFT_BRACKET)
        return nullptr;
    if (*it == LEFT_BRACKET) ++it;

    // 即使表达式为空也要插入占位节点
    // 解析初始化表达式
    TNode *init = Statement(it);
    forNode->child.push_back(init ? init : new TNode());
    if (it == code.end() || *it != SEMICOLON_CODE)
        return nullptr;
    ++it;

    // 解析条件表达式
    TNode *cond = CompareExp(it);
    forNode->child.push_back(cond ? cond : new TNode());
    if (it == code.end() || *it != SEMICOLON_CODE)
        return nullptr;
    ++it;

    // 解析迭代表达式
    TNode *iter = Statement(it);
    forNode->child.push_back(iter ? iter : new TNode());
    if (it == code.end() || *it != RIGHT_BRACKET)
        return nullptr;
    if (*it == RIGHT_BRACKET) ++it;

    // 解析循环体
    TNode *body = BlockNode(it);
    forNode->child.push_back(body ? body : new TNode());

    return forNode;
}

TNode *Statement(vector<int>::iterator &it) {
    if (it == code.end())
        return nullptr;

    if (*it == IF_CODE) {
        // if 语句
        return IfExp(it);
    }
    else if (*it == WHILE_CODE) {
        // while 语句
        return WhileExp(it);
    }
    else if (*it == FOR_CODE) {
        // for 语句
        return ForExp(it);
    }
    else if (*it >= codeRange.var_lb && *it <= codeRange.var_ub) {
        // 赋值语句
        return AssignExp(it);
    }
    else if (*it == SEMICOLON_CODE) {
        // 分号
        ++it;
        return nullptr;
    }

    // 未识别
    cerr << "unrecognized code: " << *it << endl;
    ++it;
    return nullptr;
}

TNode *ArithExp(vector<int>::iterator &it) {
    if (it == code.end())
        return nullptr;

    TNode *left;
    if (*it >= codeRange.con_lb && *it <= codeRange.con_ub) {
        left = new TNode(*it);
        ++it;
    }
    else if (*it >= codeRange.var_lb && *it <= codeRange.var_ub) {
        left = new TNode(*it);
        ++it;
    }
    else {
        return nullptr;
    }

    if (it != code.end() && (*it >= codeRange.opt_lb && *it <= codeRange.opt_ub)) {
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

TNode *AssignExp(vector<int>::iterator &it) {
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

int vt(TNode *p) {
    if (!p)
        return 0;

    if (p->data == IF_CODE) {
        int condResult = vt(p->child[0]);
        if (condResult) {
            vt(p->child[1]);
        }
        else if (p->child.size() > 2) {
            vt(p->child[2]);
        }
        return 0;
    }
    if (p->data == WHILE_CODE) {
        while (vt(p->child[0])) {
            vt(p->child[1]);
        }
        return 0;
    }
    if (p->data == FOR_CODE) {
        vt(p->child[0]);
        while (vt(p->child[1])) {
            vt(p->child[3]);
            vt(p->child[2]);
        }
        return 0;
    }
    if (p->data == BLOCK_CODE) {
        for (auto childNode : p->child) {
            vt(childNode);
        }
        return 0;
    }
    if (p->data == GREATER_CODE || p->data == LESS_CODE) {
        int left = vt(p->child[0]);
        int right = vt(p->child[1]);
        if (p->data == GREATER_CODE) {
            return left > right ? 1 : 0;
        }
        else {
            return left < right ? 1 : 0;
        }
    }
    if (p->data == MINUS_CODE || p->data == ADD_CODE) {
        int left = vt(p->child[0]);
        int right = vt(p->child[1]);
        if (p->data == MINUS_CODE) {
            return left - right;
        }
        else {
            return left + right;
        }
    }
    if (p->data == ASSIGN_CODE) {
        int varCode = p->child[0]->data;
        int val = vt(p->child[1]);
        varValue[varCode] = val;
        return val;
    }
    if (p->data >= codeRange.con_lb && p->data <= codeRange.con_ub) {
        int val = stoi(codeToCons[p->data]);
        return val;
    }
    if (p->data >= codeRange.var_lb && p->data <= codeRange.var_ub) {
        return varValue[p->data];
    }
    return 0;
}
