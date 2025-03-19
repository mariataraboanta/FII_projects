#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cstring>
using namespace std;

class Value
{
public:
    float floatValue;
    int intValue;
    bool boolValue;
    string stringValue;
    char charValue;
    string type;

    Value() : floatValue(0), intValue(0), boolValue(false), stringValue(""), charValue('\0'), type("") {}
    Value(const char *type) : floatValue(0), intValue(0), boolValue(false), stringValue(""), charValue('\0'), type(type) {}
    Value(float val, const char *type)
        : floatValue(val), intValue(0), boolValue(false), stringValue(""), charValue('\0'), type(type) {}

    Value(int val, const char *type)
        : floatValue(0), intValue(val), boolValue(false), stringValue(""), charValue('\0'), type(type) {}

    Value(bool val, const char *type)
        : floatValue(0), intValue(0), boolValue(val), stringValue(""), charValue('\0'), type(type) {}

    Value(const char *val, const char *type)
        : floatValue(0), intValue(0), boolValue(false), stringValue(val), charValue('\0'), type(type) {}

    Value(char val, const char *type)
        : floatValue(0), intValue(0), boolValue(false), stringValue(""), charValue(val), type(type) {}
};

class ParamInfo
{
public:
    string type;
    string name;
    ParamInfo(const char *type, const char *name) : type(type), name(name) {}
};

class ParamList
{
public:
    vector<ParamInfo> params;
};

class IdInfo
{
public:
    string idType;
    string type;
    string name;
    int size;
    Value valoare;

    ParamList params; 
    IdInfo() : idType(""), type(""), name(""), size(0), valoare() {}
    /*class*/ IdInfo(const char *name, const char *idType)
        : name(name), idType(idType), valoare() {}

    /*function*/ IdInfo(const char *type, const char *name, const char *idType, ParamList &params)
        : type(type), name(name), idType(idType), params(params), valoare() {}
    /*variabile*/ IdInfo(const char *type, const char *name, const char *idType)
        : type(type), name(name), idType(idType), valoare() {}
    /*variabile*/ IdInfo(const char *type, const char *name, const char *idType, Value valoare)
        : type(type), name(name), idType(idType), valoare(valoare) {}
    /*vector*/ IdInfo(const char *type, const char *name, const char *idType, int size)
        : type(type), name(name), idType(idType), size(size), valoare() {}
};

class SymTable
{
public:
    map<string, IdInfo> ids;
    SymTable *parent;
    string name;

    SymTable(SymTable *parent, const char *name) : parent(parent), name(name) {}
    bool existsId(const char *s);
    bool existsIdParents(SymTable *parent, const char *s);
    SymTable *scope(SymTable *current, const char *var);
    SymTable *scopeFunction(SymTable *current);
    SymTable *findClass(vector<SymTable *> classes, const char *clasa);
    IdInfo *existsIdWithTypeCurr(const char *s, const char *idType);
    IdInfo *existsIdWithTypeAll(SymTable *current, const char *s, const char *idType);
    void addVar(const char *type, const char *name);
    void addVar2(const char *type, const char *name, Value valoare);
    void addClass(const char *name);
    void addFunction(const char *type, const char *name, ParamList *params);
    void addVector(const char *type, const char *name, int size);
    void printVars();
    ~SymTable();
};

class ASTNode
{
public:
    string label;
    ASTNode *left = nullptr;
    ASTNode *right = nullptr;
    string type;
    string idType;
    SymTable *symTable = nullptr;
    Value value;

    ASTNode(const char *label) : label(label), left(nullptr), right(nullptr), symTable(nullptr) {}
    ASTNode(const char *label, const char *type) : label(label), type(type), left(nullptr), right(nullptr), symTable(nullptr) {}
    ASTNode(const char *type, Value value) : type(type), value(value), left(nullptr), right(nullptr), symTable(nullptr) {}
    ASTNode(const char *label, const char *type, SymTable *symTable) : label(label), type(type), symTable(symTable), left(nullptr), right(nullptr) {}
    ASTNode(const char *label, const char *type, string idType) : label(label), type(type), idType(idType), symTable(nullptr) {}

    Value evaluate();
    ~ASTNode()
    {
        delete left;
        delete right;
    }
};