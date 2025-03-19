#include <fstream>
#include <cstring>
#include "SymTable.h"
using namespace std;

void SymTable::addVar(const char *type, const char *name)
{
    IdInfo var(type, name, "var");
    Value x(type);
    var.valoare = x;
    ids[name] = var;
}

void SymTable::addVar2(const char *type, const char *name, Value valoare)
{
    IdInfo var(type, name, "var", valoare);
    ids[name] = var;
}

void SymTable::addVector(const char *type, const char *name, int size)
{
    IdInfo var(type, name, "vec", size);
    ids[name] = var;
}

void SymTable::addClass(const char *name)
{
    IdInfo var(name, "class");
    ids[name] = var;
}

void SymTable::addFunction(const char *type, const char *name, ParamList *params)
{
    ParamList validParams;
    if (params != nullptr)
    {
        validParams = *params;
    }

    IdInfo var(type, name, "function", validParams);
    Value x(type);
    var.valoare = x;
    ids[name] = var;
}

bool SymTable::existsId(const char *var)
{
    return ids.find(var) != ids.end();
}

bool SymTable::existsIdParents(SymTable *current, const char *var)
{
    SymTable *copie = current;
    while (copie != nullptr)
    {
        if (copie->existsId(var) == 1)
        {
            return 1;
        }
        copie = copie->parent;
    }
    return 0;
}

void SymTable::printVars()
{
    std::ofstream outFile("OutputTable.txt", std::ios::app);
    if (!outFile.is_open())
    {
        cout << "Error at opening file\n";
        return;
    }

    if (this->name == "global")
        outFile << "Scope: " << this->name << "\n";
    else
        outFile << "Scope: " << this->name << " with parent " << this->parent->name << "\n";

    for (const auto &v : ids)
        if (v.second.idType == "var" || v.second.idType == "vec")
        {
            outFile << "name: " << v.first
                    << " type: " << v.second.type
                    << " idtype: " << v.second.idType << "\n";
        }
        else if (v.second.idType == "function")
        {
            outFile << "name: " << v.first
                    << " type: " << v.second.type
                    << " idtype: " << v.second.idType << "\n";

            outFile << " Parameters: \n";
            for (const auto &param : v.second.params.params)
            {
                outFile << "  param name: " << param.name
                        << " type: " << param.type << "\n";
            }
        }
        else if (v.second.idType == "class")
        {
            outFile << "name: " << v.first
                    << " idtype: " << v.second.idType << "\n";
        }

    outFile.close();
}

SymTable *SymTable::scope(SymTable *current, const char *var)
{
    SymTable *copie = current;
    while (copie->parent != nullptr)
    {
        if (copie->existsId(var) == 1)
        {
            return copie;
        }
        copie = copie->parent;
    }
    return copie;
}

SymTable *SymTable::scopeFunction(SymTable *current)
{
    SymTable *copie = current;
    while (copie->parent != nullptr)
    {
        for (auto &v : copie->parent->ids)
            if (v.second.idType == "function")
            {
                return copie;
            }
        copie = copie->parent;
    }
    return copie;
}

SymTable *SymTable::findClass(vector<SymTable *> classes, const char *clasa)
{
    for (size_t i = 0; i < classes.size(); ++i)
    {
        SymTable *ptr = classes[i];
        if (ptr->name == clasa)
            return ptr;
    }
    return nullptr;
}

IdInfo *SymTable::existsIdWithTypeCurr(const char *s, const char *idType)
{
    for (auto &v : ids)
        if (v.first == s && v.second.idType == idType)
        {
            return &v.second;
        }
    return nullptr;
}

IdInfo *SymTable::existsIdWithTypeAll(SymTable *current, const char *s, const char *idType)
{
    SymTable *copie = current;
    while (copie != nullptr)
    {
        IdInfo *v = copie->existsIdWithTypeCurr(s, idType);
        if (v != 0)
        {
            return v;
        }
        copie = copie->parent;
    }
    return nullptr;
}

SymTable::~SymTable()
{
    ids.clear();
}

Value ASTNode::evaluate()
{
    if (idType == "function" || idType == "vec")
    {
        Value x(type.c_str());
        return x;
    }
    if (type == "int" || type == "float" || type == "bool" || type == "string" || type == "char")
    {
        return value;
    }
    else if (type != "operator")
    {
        return Value();
    }

    Value leftValue, rightValue;
    if (left != nullptr)
    {
        leftValue = left->evaluate();
    }
    if (right != nullptr)
    {
        rightValue = right->evaluate();
    }

    if (type == "operator")
    {
        if (label == "+" || label == "-" || label == "*" || label == "/" || label == "%")
            type = leftValue.type;
        else
            type = "bool";
    }

    if (label == "+")
    {

        if (leftValue.type == "float" && rightValue.type == "float")
        {
            return Value(leftValue.floatValue + rightValue.floatValue, "float");
        }
        else if (leftValue.type == "int" && rightValue.type == "int")
        {
            return Value(leftValue.intValue + rightValue.intValue, "int");
        }
        throw runtime_error("Type mismatch in expression1");
    }
    else if (label == "-")
    {
        if (leftValue.type == "float" && rightValue.type == "float")
        {
            return Value(leftValue.floatValue - rightValue.floatValue, "float");
        }
        else if (leftValue.type == "int" && rightValue.type == "int")
        {
            return Value(leftValue.intValue - rightValue.intValue, "int");
        }
        throw runtime_error("Type mismatch in expression2");
    }
    else if (label == "*")
    {

        if (leftValue.type == "float" && rightValue.type == "float")
        {
            return Value(leftValue.floatValue * rightValue.floatValue, "float");
        }
        else if (leftValue.type == "int" && rightValue.type == "int")
        {
            return Value(leftValue.intValue * rightValue.intValue, "int");
        }
        throw runtime_error("Type mismatch in expression3");
    }
    else if (label == "/")
    {
        if (leftValue.type == "float" && rightValue.type == "float")
        {
            if (rightValue.floatValue == 0)
            {
                throw runtime_error("Division by zero");
            }
            else
                return Value(leftValue.floatValue / rightValue.floatValue, "float");
        }
        else if (leftValue.type == "int" && rightValue.type == "int")
        {
            if (rightValue.intValue == 0)
            {
                throw runtime_error("Division by zero");
            }
            else
                return Value(leftValue.intValue / rightValue.intValue, "int");
        }
        throw runtime_error("Type mismatch in expression4");
    }
    else if (label == "%")
    {
        if (leftValue.type == "float" && rightValue.type == "float")
        {
            throw runtime_error("Cannot % on floats");
        }
        else if (leftValue.type == "int" && rightValue.type == "int")
        {
            if (rightValue.intValue == 0)
            {
                throw runtime_error("Division by zero");
            }
            else
                return Value(leftValue.intValue % rightValue.intValue, "int");
        }
        throw runtime_error("Type mismatch in expression5");
    }
    else if (label == "&&")
    {
        return Value(leftValue.boolValue && rightValue.boolValue, "bool");
    }
    else if (label == "||")
    {
        return Value(leftValue.boolValue || rightValue.boolValue, "bool");
    }
    else if (label == "==")
    {
        if (leftValue.type == "float" && rightValue.type == "float")
        {
            return Value(leftValue.floatValue == rightValue.floatValue, "bool");
        }
        else if (leftValue.type == "int" && rightValue.type == "int")
        {
            return Value(leftValue.intValue == rightValue.intValue, "bool");
        }
        else if (leftValue.type == "bool" && rightValue.type == "bool")
        {
            return Value(leftValue.boolValue == rightValue.boolValue, "bool");
        }
        throw runtime_error("Type mismatch in expression6");
    }
    else if (label == "!=")
    {
        if (leftValue.type == "float" && rightValue.type == "float")
        {
            return Value(leftValue.floatValue != rightValue.floatValue, "bool");
        }
        else if (leftValue.type == "int" && rightValue.type == "int")
        {
            return Value(leftValue.intValue != rightValue.intValue, "bool");
        }
        else if (leftValue.type == "bool" && rightValue.type == "bool")
        {
            return Value(leftValue.boolValue != rightValue.boolValue, "bool");
        }
        throw runtime_error("Type mismatch in expression7");
    }
    else if (label == "<")
    {
        cout << leftValue.type << " " << rightValue.type << endl;
        if (leftValue.type == "float" && rightValue.type == "float")
        {
            return Value(leftValue.floatValue < rightValue.floatValue, "bool");
        }
        else if (leftValue.type == "int" && rightValue.type == "int")
        {
            return Value(leftValue.intValue < rightValue.intValue, "bool");
        }
        throw runtime_error("Type mismatch in expression8");
    }
    else if (label == ">")
    {
        if (leftValue.type == "float" && rightValue.type == "float")
        {
            return Value(leftValue.floatValue > rightValue.floatValue, "bool");
        }
        else if (leftValue.type == "int" && rightValue.type == "int")
        {
            return Value(leftValue.intValue > rightValue.intValue, "bool");
        }
        throw runtime_error("Type mismatch in expression9");
    }
    else if (label == "<=")
    {
        if (leftValue.type == "float" && rightValue.type == "float")
        {
            return Value(leftValue.floatValue <= rightValue.floatValue, "bool");
        }
        else if (leftValue.type == "int" && rightValue.type == "int")
        {
            return Value(leftValue.intValue <= rightValue.intValue, "bool");
        }
        throw runtime_error("Type mismatch in expression10");
    }
    else if (label == ">=")
    {
        if (leftValue.type == "float" && rightValue.type == "float")
        {
            return Value(leftValue.floatValue >= rightValue.floatValue, "bool");
        }
        else if (leftValue.type == "int" && rightValue.type == "int")
        {
            return Value(leftValue.intValue >= rightValue.intValue, "bool");
        }
        throw runtime_error("Type mismatch in expression11");
    }
    else if (label == "!")
    {
        return Value(!leftValue.boolValue, "bool");
    }

    if (symTable != nullptr && symTable->existsId(label.c_str()))
    {
        IdInfo id = symTable->ids[label];
        return id.valoare;
    }

    throw runtime_error("Label does not exist " + label);
}