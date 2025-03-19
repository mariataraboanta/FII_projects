%{
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include "SymTable.h"

extern FILE* yyin;
extern char* yytext;
extern int yylineno;
extern int yylex();
void yyerror(const char * s);

class SymTable* current;
class SymTable* copie;
vector<class SymTable*> classes;

int errorCount = 0;

%}
%union {
     const char* string;
     int intval;
     float floatval;
     class ParamList* paramList;
     class ParamInfo* paramInfo;
     class IdInfo* obiect;
     class ASTNode* astnode;
}

%token BGIN END CLASS ACCESS
%token<string> EQ LE GE NE LT GT AND OR NOT
%token IF ELSE WHILE FOR
%token ASSIGN 
%token<string> ADD_ASSIGN MUL_ASSIGN MINUS_ASSIGN DIV_ASSIGN MOD_ASSIGN
%token<string> INCREMENT DECREMENT
%token PRINT TYPEOF RETURN
%token<string> ID TYPE
%token<intval> INTEGER
%token<floatval> FLOAT
%token<string> STRING
%token<string> CHAR TRUE FALSE

%type <paramList> list_param call_list
%type <paramInfo> param
%type <obiect> var
%type <astnode> e e_bool expression compare_e check
%type <string> assign_op unary_op return_fct

%start progr
%left ASSIGN ADD_ASSIGN MUL_ASSIGN MINUS_ASSIGN DIV_ASSIGN MOD_ASSIGN
%left '+' '-' 
%left '*' '/' '%'
%left OR
%left AND
%left EQ NE
%left LT LE GT GE
%nonassoc INCREMENT DECREMENT
%right NOT
%%
progr :   {
               if (std::remove("OutputTable.txt") == 0) { std::cout << "File deleted successfully.\n"; } 
               else { std::cerr << "Error: Could not delete file\n"; }
          } 
          classes_section_decl var_section_decl function_section_decl main 
          {
               current->printVars(); 
               if (errorCount == 0)  {cout<< "The program is correct!" << endl;}
          }
      ;

classes_section_decl : classes_section
                     | classes_section_decl classes_section
                     ;

classes_section :   CLASS ID '{' 
                    { SymTable* w= new SymTable(current, $2); current = w; } 
                    class_decl '}' ';' 
                    { 
                         classes.push_back(current); current->printVars(); current=current->parent;
                         if(!current->existsId($2)) { current->addClass($2); }
                         else
                         {
                              errorCount++; 
                              yyerror("Class already defined");
                         }
                    }
                ;

class_decl : ACCESS ':' declarations_for_class 
           | ACCESS ':' declarations_for_class class_decl
           ;
    
declarations_for_class : decl_for_class        
	                  | declarations_for_class decl_for_class
	                  ;

decl_for_class : TYPE ID ';'
               {
                    if(!current->existsId($2)) { current->addVar($1,$2); } 
                    else 
                    {
                         errorCount++; 
                         yyerror("Variable already defined");
                    }
               }
               | TYPE ID ASSIGN expression ';'
               {
                    if(!current->existsId($2)) 
                    {
                         if ($1 == $4->type) { current->addVar2($1,$2,$4->value); } 
                         else 
                         {
                              errorCount++;
                              yyerror("Type mismatch in assignment");
                         }
                    } 
                    else 
                    {
                         errorCount++; 
                         yyerror("Variable already defined");
                    }
               }
               | TYPE ID ASSIGN STRING ';'
               {
                    if(!current->existsId($2)) 
                    {
                         if (strcmp($1, "string") == 0) 
                         {
                              string val;
                              int len = strlen($4);
                              for(int i = 1; i < len - 1; i++) 
                              {
                                   val += $4[i]; 
                              }
                              Value x(val.c_str(), "string");
                              current->addVar2($1,$2,x); 
                         } 
                         else 
                         {
                              errorCount++;
                              yyerror("Type mismatch in assignment");
                         }
                    } 
                    else 
                    {
                         errorCount++; 
                         yyerror("Variable already defined");
                    }
               }
               | TYPE ID ASSIGN CHAR ';'
               {
                    if(!current->existsId($2)) 
                    {
                         if (strcmp($1, "char") == 0)
                         {
                              char aux = $4[1];
                              Value x(aux, "char");
                              current->addVar2($1,$2,x); 
                         } 
                         else 
                         {
                              errorCount++;
                              yyerror("Type mismatch in assignment");
                         }
                    } 
                    else 
                    {
                         errorCount++; 
                         yyerror("Variable already defined");
                    }
               } 
               | TYPE ID  '[' INTEGER ']' ';'
               {
                    if(!current->existsId($2)) { current->addVector($1,$2, $4); } 
                    else 
                    {
                         errorCount++; 
                         yyerror("Vector already defined");
                    }
               }
               | TYPE ID '(' 
               {
                    SymTable* w= new SymTable(current, $2);
                    current = w;
                    copie = w;
                    
               } 
               list_param ')' 
               {
                    copie = copie->parent; 
                    if(!copie->existsId($2)){ copie->addFunction($1, $2, $5); }
                    else
                    {
                         errorCount++; 
                         yyerror("Function already defined");
                    }
               }
               '{' list '}' 
               { current->printVars(); current=current->parent; } 
               | TYPE ID '(' 
               {
                    if(!current->existsId($2)){
                         current->addFunction($1, $2, nullptr);
                    }
                    else{
                         errorCount++; 
                         yyerror("Function already defined");
                    }
               
                    SymTable* w= new SymTable(current, $2);
                    current = w;

               } 
               ')' '{' list '}' 
               { current->printVars(); current=current->parent; }
               ; 

var_section_decl : var_section
                 | var_section_decl var_section
                 ;

var_section : TYPE ID ';'
               {
                    if(!current->existsId($2)) { current->addVar($1,$2); } 
                    else 
                    {
                         errorCount++; 
                         yyerror("Variable already defined");
                    }
               }
               | TYPE ID ASSIGN expression ';'
               {
                    if(!current->existsId($2)) 
                    {
                         if ($1 == $4->type) { current->addVar2($1,$2,$4->value); } 
                         else 
                         {
                              errorCount++;
                              yyerror("Type mismatch in assignment");
                         }
                    } 
                    else 
                    {
                         errorCount++; 
                         yyerror("Variable already defined");
                    }
               }
               | TYPE ID ASSIGN STRING ';'
               {
                    if(!current->existsId($2)) 
                    {
                         if (strcmp($1, "string") == 0) 
                         {
                              string val;
                              int len = strlen($4);
                              for(int i = 1; i < len - 1; i++) {
                                   val += $4[i]; 
                              }
                              Value x(val.c_str(), "string");
                              current->addVar2($1,$2,x);
                         } 
                         else 
                         {
                              errorCount++;
                              yyerror("Type mismatch in assignment");
                         }
                    } 
                    else 
                    {
                         errorCount++; 
                         yyerror("Variable already defined");
                    }
               }
               | TYPE ID ASSIGN CHAR ';'
               {
                    if(!current->existsId($2)) 
                    {
                         if (strcmp($1, "char") == 0) 
                         {    
                              Value x($4[1], "char");
                              current->addVar2($1,$2,x);
                         } 
                         else 
                         {
                              errorCount++;
                              yyerror("Type mismatch in assignment");
                         }
                    } 
                    else
                    {
                         errorCount++; 
                         yyerror("Variable already defined");
                    }
               } 
               | TYPE ID '[' INTEGER ']' ';' 
               {
                    if(!current->existsId($2)) { current->addVector($1,$2, $4); } 
                    else 
                    {
                         errorCount++; 
                         yyerror("Variable already defined");
                    }
               }
              | ID ID ';' 
              {
                    if(!current->existsIdParents(current, $1)) 
                    {
                         errorCount++;
                         yyerror("Type not declared");
                    }
                    else
                    {
                         class SymTable* global;
                         global = current->scope(current, $1);
                         string idType = global->ids[$1].idType;
                         if (idType != "class") 
                         {
                              errorCount++;
                              yyerror("Identifier is not a class");
                         }
                         else 
                         {

                              if(!current->existsId($2)) 
                              {
                                   current->addVar($1,$2);
                                   class SymTable* clasa;
                                   clasa = current->findClass(classes, $1);
                                   string var;
                                   for (auto &v : clasa->ids)
                                   {
                                        var = $2; var += "."; var += v.second.name;
                                        if (v.second.idType == "var")
                                        { 
                                             current->addVar2(v.second.type.c_str(), var.c_str(), v.second.valoare); 
                                        }
                                        else if (v.second.idType == "function")
                                        { 
                                             current->addFunction(v.second.type.c_str(), var.c_str(), &v.second.params); 
                                        }
                                        else if (v.second.idType == "vec")
                                        { 
                                             current->addVector(v.second.type.c_str(), var.c_str(), v.second.size); 
                                        }
                                   }
                              } else 
                              {
                                   errorCount++; 
                                   yyerror("Variable already defined");
                              }
                         }
                    }
               }
              ;

function_section_decl : function_section
                      | function_section_decl function_section
                      ;

function_section : TYPE ID '(' 
                    {
                         SymTable* w= new SymTable(current, $2);
                         current = w;
                         copie = w;   
                    } 
                    list_param ')' 
                    {
                         copie = copie->parent; 
                         if(!copie->existsId($2)) { copie->addFunction($1, $2, $5); }
                         else
                         {
                              errorCount++; 
                              yyerror("Function already defined");
                         }
                    }
                    '{' list '}' { current->printVars(); current=current->parent;} 
                    | TYPE ID '(' 
                    {
                         if(!current->existsId($2)){ current->addFunction($1, $2, nullptr); }
                         else
                         {
                              errorCount++; 
                              yyerror("Function already defined");
                         }
                    
                         SymTable* w= new SymTable(current, $2);
                         current = w;
                    } 
                    ')' '{' list '}' { current->printVars(); current=current->parent;}
                    ;

list_param : param 
          {    
               $$ = new ParamList();
               $$->params.push_back(*$1);
          }
          | param ',' list_param 
          {
               $$ = $3;
               $$->params.insert($$->params.begin(), *$1);
          }
          ;          

param : TYPE ID  
     { 
          $$ = new ParamInfo($1, $2);
          if(!current->existsId($2)) { current->addVar($1, $2); }
          else
          {
               errorCount++; 
               yyerror("Param already defined");
          }
     }
     | TYPE ID '[' INTEGER ']' 
     {
          $$ = new ParamInfo($1, $2);
          if(!current->existsId($2)){ current->addVar($1, $2); }
          else
          {
               errorCount++; 
               yyerror("Param already defined");
          }
     }
     | ID ID 
     {
          $$ = new ParamInfo($1, $2);
          if(!current->existsId($2)){ current->addVar($1, $2); }
          else
          {
               errorCount++; 
               yyerror("Param already defined");
          }
     }
     ;

return_fct : RETURN expression ';'
          {
               $$ = $2->type.c_str();
          }
          | RETURN STRING ';'
          {
               $$ = "string";
          }
          | RETURN CHAR ';'
          {
               $$ = "char";
          }
          ;

var : ID 
     {
          IdInfo* obj = current->existsIdWithTypeAll(current, $1, "var");
          if (!obj) 
          {
               errorCount++;
               yyerror("Variable not declared");
          }
          else
          {
               $$ = obj;
          }
     }
     | ID '[' INTEGER ']'
     {
          IdInfo* obj = current->existsIdWithTypeAll(current, $1, "vec");
          if (!obj) 
          {
               errorCount++;
               yyerror("Vector not declared");
          }
          else
          {
               if (obj->size < $3)
               {
                    errorCount++;
                    yyerror("Out of bound at vector");
                    exit(0);
               }
               else
               {
                    $$ = obj;
               }
          }
     }
     | ID '.' ID 
     {
          if (!current->existsIdWithTypeAll(current, $1, "var"))
          {
               errorCount++;
               yyerror("Variable not declared");
          }
          else 
          {
               string var;
               var = $1; var += "."; var += $3;
               IdInfo* obj = current->existsIdWithTypeAll(current, var.c_str(), "var");
               if(!obj)
               {
                    errorCount++;
                    yyerror("Variable is not a field of the class");
               }
               else
               {
                    $$ = obj;
               }
          }

     }
     | ID '.' ID '[' INTEGER ']'
     {
          if (!current->existsIdWithTypeAll(current, $1, "var"))
          {
               errorCount++;
               yyerror("Variable not declared");
          }
          else 
          {
               string var;
               var = $1; var += "."; var += $3;
               IdInfo* obj = current->existsIdWithTypeAll(current, var.c_str(), "vec");
               if(!obj)
               {
                    errorCount++;
                    yyerror("Variable is not a field of the class");
               }
               else
               {
                    if (obj->size < $5)
                    {
                         errorCount++;
                         yyerror("Out of bound at vector");
                         exit(0);
                    }
                    else
                    {
                         $$ = obj;
                    }
               }
          }
     }
     ;

check : TRUE
      { $$ = new ASTNode("bool", Value(true, "bool")); }
      | FALSE
      { $$ = new ASTNode("bool", Value(false, "bool")); }
      ;

main : BGIN 
     {
          SymTable* w= new SymTable(current, "main");
          current = w;
     } 
     list END 
     {
          current->printVars(); 
          current = current->parent;
     } 
     ;
     
list : statement 
     | list statement
     ;

statement : predef_funct
          | var ASSIGN expression ';'
          {
               if ($1->type == $3->type) { $1->valoare = $3->value; } 
               else 
               {
                    errorCount++;
                    yyerror("Type mismatch in assignment");
               }
          }
          | var ASSIGN CHAR ';'
          {

               if ($1->type == "char") { $1->valoare = Value($3[1], "char"); }
               else 
               {
                    errorCount++;
                    yyerror("Type mismatch in assignment");
               }
          }
          | var ASSIGN STRING ';'
          { 
               string val;
               int len = strlen($3);
               for(int i = 1; i < len - 1; i++) 
               {
                    val += $3[i]; 
               }
               if ($1->type == "string") { $1->valoare = Value(val.c_str(), "string"); } 
               else 
               {
                    errorCount++;
                    yyerror("Type mismatch in assignment");
               }
          }
          | var ADD_ASSIGN STRING ';'
          {
               string val;
               int len = strlen($3);
               for(int i = 1; i < len - 1; i++) 
               {
                    val += $3[i]; 
               }
               if($1->type == "string")
               {
                    $1->valoare.stringValue += val;
               }
          }
          | var assign_op e ';'
          {
               Value val;
               val = $3->evaluate();
               if ($1->type == val.type && $1->type == "int") 
               {
                    const char* op = $2;

                    if (strcmp(op, "+=") == 0) 
                    {
                         $1->valoare.intValue += val.intValue;
                    } 
                    else if (strcmp(op, "*=") == 0) 
                    {
                         $1->valoare.intValue *= val.intValue;
                    } 
                    else if (strcmp(op, "-=") == 0) 
                    {
                         $1->valoare.intValue -= val.intValue;
                    } 
                    else if (strcmp(op, "/=") == 0) 
                    {
                         if(val.intValue == 0)
                         {
                              errorCount++;
                              yyerror("Cannot /= because the r_value is 0");
                              exit(0);
                         }
                         else
                         {
                              $1->valoare.intValue /= val.intValue;
                         }
                    } 
                    else if (strcmp(op, "%=") == 0) 
                    {
                         if(val.intValue == 0.0)
                         {
                              errorCount++;
                              yyerror("Cannot %= because the r_value is 0");
                              exit(0);
                         }
                         else
                         {
                              $1->valoare.intValue %= val.intValue;
                         }
                    }
               } 
               else if ($1->type == val.type && $1->type == "float") 
               {
                    const char* op = $2;

                    if (strcmp(op, "+=") == 0) 
                    {
                         $1->valoare.floatValue += val.floatValue;
                    } 
                    else if (strcmp(op, "*=") == 0) 
                    {
                         $1->valoare.floatValue *= val.floatValue;
                    } 
                    else if (strcmp(op, "-=") == 0) 
                    {
                         $1->valoare.floatValue -= val.floatValue;
                    } 
                    else if (strcmp(op, "/=") == 0) 
                    {
                         if(val.floatValue == 0.0)
                         {
                              errorCount++;
                              yyerror("Cannot /= because the r_value is 0.0");
                              exit(0);
                         }
                         else
                         {
                              $1->valoare.floatValue /= val.floatValue;
                         }
                    } 
                    else if (strcmp(op, "%=") == 0) 
                    {
                        errorCount++;
                        yyerror("Cannot %= on floats");
                        exit(0);
                    }
               }
               else
               {
                    errorCount++;
                    yyerror("Type mismatch in assignment");
               }
          }
          | var unary_op ';'
          {
               if ($1->type != "int")
               {
                    errorCount++;
                    yyerror("Increment and decrement only on int");
               }
               else
               {
                   if (strcmp($2, "++") == 0) 
                    {
                         $1->valoare.intValue++;
                    } 
                    else if (strcmp($2, "--") == 0) 
                    {
                         $1->valoare.intValue--;
                    }  
               }
          }
          | var_section
          | if_statement
          | return_fct
          {
               SymTable *functie;
               functie = current->scopeFunction(current);

               SymTable *aux;
               aux = current->scope(current,functie->name.c_str());

               if (aux->ids[functie->name].type != $1)
               {
                    errorCount++;
                    yyerror("Return type is wrong");
               }
          }
          | while_statement
          | for_statement
          | ID '(' call_list ')' ';'
          {
               IdInfo* obj = current->existsIdWithTypeAll(current, $1, "function");
               if (!obj) 
               {
                    errorCount++;
                    yyerror("Variable not declared");
                    exit(0);
               }
               else
               {    
                    if ($3->params.size() != obj->params.params.size()) 
                    {
                         errorCount++;
                         yyerror("Param list size is wrong");
                         exit(0);
                    }
                    else
                    { 
                         bool ok = 1;
                         for (size_t i = 0; i < $3->params.size(); ++i) 
                         {
                              if ($3->params[i].type != obj->params.params[i].type) 
                              {
                                   ok = 0;
                              }
                         }
                         if(ok == 0)
                         {
                              errorCount++;
                              yyerror("Params type is wrong");
                              exit(0);
                         }
                    }
               }
          }
          ;

predef_funct : PRINT '(' expression ')' ';'
               {
                    cout << "la functia print la " << $3->label << " valoarea ";
                    if ($3->type == "int")
                    {
                         cout << $3->value.intValue <<endl;
                    }
                    else if ($3->type == "float")
                    {
                         cout << $3->value.floatValue <<endl;
                    }
                    else if ($3->type == "string")
                    {
                         cout << $3->value.stringValue <<endl;
                    }
                    else if ($3->type == "char")
                    {
                         cout << $3->value.charValue <<endl;
                    }
                    else
                    {
                         cout << $3->value.boolValue <<endl;
                    }
               }
               | TYPEOF '(' expression')' ';'
               {
                    cout << "la functia typeof " << $3->label << " are tipul " << $3->type << endl;
               }
               | PRINT '(' STRING ')' ';'
               {
                    cout << "la functia print " << $3 <<endl;
               }
               | TYPEOF '(' STRING ')' ';'
               {
                    cout << "la functia typeof " << $3 << " are tipul string" << endl;
               }
               | PRINT '(' CHAR ')' ';'
               {
                    cout << "la functia print " << $3 <<endl;
               }
               | TYPEOF '(' CHAR ')' ';'
               {
                    cout << "la functia typeof " << $3 << " are tipul char" << endl;
               }
               ;

expression : e
           {
               $$ = $1;
               $$->value = $1->evaluate();
           }
           | e_bool 
           {
               $$ = $1;
               $$->value = $1->evaluate();
           }
           ;

unary_op : INCREMENT 
          {
             $$ = $1;
          }
          | DECREMENT
          {
            $$ = $1;
          }
          ;

assign_op : ADD_ASSIGN 
          {
            $$=$1;
          }
          | MUL_ASSIGN 
          {
            $$=$1;
          }
          | MINUS_ASSIGN 
          {
            $$=$1;
          }
          | DIV_ASSIGN 
          {
            $$=$1;
          }
          | MOD_ASSIGN
          {
            $$=$1;
          }
          ;

for_statement : FOR '(' for_assign ';' e_bool ';' for_opp ')' block
              | FOR '(' for_assign ';' e_bool ';' for_opp ')' '{' '}'
              ;

for_assign : var ASSIGN e
          {
               if ($1->type == $3->type) { $1->valoare = $3->value; } 
               else 
               {
                    errorCount++;
                    yyerror("Type mismatch in assignment");
               }
          }
          | var ASSIGN STRING
          {
               string val;
               int len = strlen($3);
               for(int i = 1; i < len - 1; i++) 
               {
                    val += $3[i]; 
               }
               if ($1->type == "string") { $1->valoare = Value(val.c_str(), "string"); } 
               else 
               {
                    errorCount++;
                    yyerror("Type mismatch in assignment");
               }
          }
          | var ASSIGN CHAR 
          {
               if ($1->type == "char") { $1->valoare = Value($3[1], "char"); } 
               else 
               {
                    errorCount++;
                    yyerror("Type mismatch in assignment");
               }
          }
          ;

for_opp : var assign_op e
          {
               Value val;
               val = $3->evaluate();
               if ($1->type == val.type && $1->type == "int") 
               {
                    const char* op = $2;

                    if (strcmp(op, "+=") == 0) 
                    {
                         $1->valoare.intValue += val.intValue;
                    } 
                    else if (strcmp(op, "*=") == 0) 
                    {
                         $1->valoare.intValue *= val.intValue;
                    } 
                    else if (strcmp(op, "-=") == 0) 
                    {
                         $1->valoare.intValue -= val.intValue;
                    } 
                    else if (strcmp(op, "/=") == 0) 
                    {
                         if(val.intValue == 0)
                         {
                              errorCount++;
                              yyerror("Cannot /= because the r_value is 0");
                              exit(0);
                         }
                         else
                         {
                              $1->valoare.intValue /= val.intValue;
                         }
                    } 
                    else if (strcmp(op, "%=") == 0) 
                    {
                         if(val.intValue == 0.0)
                         {
                              errorCount++;
                              yyerror("Cannot %= because the r_value is 0");
                              exit(0);
                         }
                         else
                         {
                              $1->valoare.intValue %= val.intValue;
                         }
                    }
               } 
               else if ($1->type == val.type && $1->type == "float") 
               {
                    const char* op = $2;

                    if (strcmp(op, "+=") == 0) 
                    {
                         $1->valoare.floatValue += val.floatValue;
                    } 
                    else if (strcmp(op, "*=") == 0) 
                    {
                         $1->valoare.floatValue *= val.floatValue;
                    } 
                    else if (strcmp(op, "-=") == 0) 
                    {
                         $1->valoare.floatValue -= val.floatValue;
                    } 
                    else if (strcmp(op, "/=") == 0) 
                    {
                         if(val.floatValue == 0.0)
                         {
                              errorCount++;
                              yyerror("Cannot /= because the r_value is 0.0");
                              exit(0);
                         }
                         else
                         {
                              $1->valoare.floatValue /= val.floatValue;
                         }
                    } 
                    else if (strcmp(op, "%=") == 0) 
                    {
                         errorCount++;
                         yyerror("Cannot %= on floats");
                         exit(0);
                    }
               }
               else
               {
                    errorCount++;
                    yyerror("Type mismatch in assignment");
               }
          }
          | var ADD_ASSIGN STRING
          {
               string val;
               int len = strlen($3);
               for(int i = 1; i < len - 1; i++) 
               {
                    val += $3[i]; 
               }
               if($1->type == "string")
               {
                    $1->valoare.stringValue += val;
               }
          }
          | var unary_op
          {
               if ($1->type != "int")
               {
                    errorCount++;
                    yyerror("Increment and decrement only on int");
               }
               else
               {
                    if (strcmp($2, "++") == 0) 
                    {
                         $1->valoare.intValue++;
                    } 
                    else if (strcmp($2, "--") == 0) 
                    {
                         $1->valoare.intValue--;
                    }  
               }
          }
          ;

while_statement : WHILE '(' e_bool ')' block
                | WHILE '(' e_bool ')' '{'  '}'
                ;

if_statement : IF '(' e_bool ')' block
             | IF '(' e_bool ')' block ELSE block 
             | IF '(' e_bool ')' '{' '}'
             | IF '(' e_bool ')' '{' '}' ELSE block
             ;

block : '{' 
     {
          SymTable* w= new SymTable(current, "block");
          current = w;
     } 
     list '}' 
     {
          current->printVars(); 
          current = current->parent;
     } 
     ;

e_bool : e_bool AND e_bool
     {  
          $$ = new ASTNode("&&", "operator", nullptr);
          $$->left = $1;
          $$->right = $3;
     }
     | e_bool OR e_bool
     {  
          $$ = new ASTNode("||", "operator", nullptr);
          $$->left = $1;
          $$->right = $3;
     }
     | e_bool EQ e_bool
     {  
          $$ = new ASTNode("==", "operator", nullptr);
          $$->left = $1;
          $$->right = $3;
     }
     | e_bool NE e_bool
     {  
          $$ = new ASTNode("!=", "operator", nullptr);
          $$->left = $1;
          $$->right = $3;
     }
     | compare_e
     { $$ = $1; }
     | '(' e_bool ')'
     { $$ = $2; }
     | NOT e_bool
     {  
          $$ = new ASTNode("!", "operator", nullptr);
          $$->left = $2;
     }
     | check
     { $$ = $1; }
     ;    

compare_e : e EQ e
          {  
               $$ = new ASTNode("==", "operator", nullptr);
               $$->left = $1;
               $$->right = $3;
          }
          | e NE e
          {  
               $$ = new ASTNode("!=", "operator", nullptr);
               $$->left = $1;
               $$->right = $3;
          }
          | e LT e
          {  
               $$ = new ASTNode("<", "operator", nullptr);
               $$->left = $1;
               $$->right = $3;
          }
          | e LE e
          {  
               $$ = new ASTNode("<=", "operator", nullptr);
               $$->left = $1;
               $$->right = $3;
          }
          | e GT e
          {  
               $$ = new ASTNode(">", "operator", nullptr);
               $$->left = $1;
               $$->right = $3;
          }
          | e GE e 
          {  
               $$ = new ASTNode(">=", "operator", nullptr);
               $$->left = $1;
               $$->right = $3;
          }
          | e EQ check
          {  
               $$ = new ASTNode("==", "operator", nullptr);
               $$->left = $1;
               $$->right = $3;
          }
          | e NE check
          {  
               $$ = new ASTNode("!=", "operator", nullptr);
               $$->left = $1;
               $$->right = $3;
          }
          ;      

call_list : expression
          {    
               ParamInfo var = ParamInfo($1->type.c_str(), "expr");
               $$ = new ParamList();
               $$->params.push_back(var);
          }
          | expression ',' call_list 
          {
               $$ = $3;
               ParamInfo var =  ParamInfo($1->type.c_str(), "expr");
               $$->params.insert($$->params.begin(), var);
          }
          | STRING
          {
               ParamInfo var = ParamInfo("string", "str");
               $$ = new ParamList();
               $$->params.push_back(var);
          }
          | STRING ',' call_list 
          {
               $$ = $3;
               ParamInfo var =  ParamInfo("string", "str");
               $$->params.insert($$->params.begin(), var);
          }
          | CHAR
          {
               ParamInfo var = ParamInfo("char", "caracter");
               $$ = new ParamList();
               $$->params.push_back(var);
          }
          | CHAR ',' call_list
          {
               $$ = $3;
               ParamInfo var =  ParamInfo("char", "caracter");
               $$->params.insert($$->params.begin(), var);
          }
          ;


e : e '+' e
     {  
          $$ = new ASTNode("+", "operator", nullptr);
          $$->left = $1;
          $$->right = $3;
     }
     | e '*' e
     {
          $$ = new ASTNode("*", "operator", nullptr);
          $$->left = $1;
          $$->right = $3;
     }
     | e '-' e
     {
          $$ = new ASTNode("-", "operator", nullptr);
          $$->left = $1;
          $$->right = $3;
     }
     | e '/' e
     {
          $$ = new ASTNode("/", "operator", nullptr);
          $$->left = $1;
          $$->right = $3;
     }
     | e '%' e
     {
          $$ = new ASTNode("%", "operator", nullptr);
          $$->left = $1;
          $$->right = $3;
     }
     | '(' e ')'
     { $$ = $2; }
     | ID '(' call_list ')'
     {
          IdInfo* obj = current->existsIdWithTypeAll(current, $1, "function");
          if (!obj) 
          {
               errorCount++;
               yyerror("Variable not declared");
               exit(0);
          }
          else
          {    
               if ($3->params.size() != obj->params.params.size()) 
               {
                    errorCount++;
                    yyerror("Param list size is wrong");
                    exit(0);
               }
               else
               { 
                    bool ok = 1;
                    for (size_t i = 0; i < $3->params.size(); ++i) 
                    {
                         if ($3->params[i].type != obj->params.params[i].type) 
                         {
                              ok = 0;
                         }
                    }
                    if(ok == 0)
                    {
                         errorCount++;
                         yyerror("Params type is wrong");
                         exit(0);
                    }
                    else
                    { 
                         $$  = new ASTNode(obj->name.c_str(), obj->type.c_str(), "function");
                    }
               }
          }
     }
     | ID '.' ID '(' call_list ')'
      {
          if (!current->existsIdWithTypeAll(current, $1, "var"))
          {
               errorCount++;
               yyerror("Variable not declared");
          }
          else 
          {
               class SymTable* local;
               local = current->scope(current, $1);

               string idType = local->ids[$1].type; 
               class SymTable* global;
               global = local->scope(local, idType.c_str());

               if (global->ids[idType].idType != "class") 
               {
                    errorCount++;
                    yyerror("Variable is not an object of a class");
               }
               else
               {
                    class SymTable* clasa;
                    clasa = current->findClass(classes, idType.c_str());
                    IdInfo* obj = clasa->existsIdWithTypeCurr($3, "function");
                    if(!obj)
                    {
                         errorCount++;
                         yyerror("Variable is not a field of the class");
                    }
                    else
                    {    

                         if ($5->params.size() != obj->params.params.size()) 
                         {
                              errorCount++;
                              yyerror("Param list size is wrong");
                              exit(0);
                         }
                         else
                         { 
                              bool ok = 1;
                              for (size_t i = 0; i < $5->params.size(); ++i) 
                              {
                                        if ($5->params[i].type != obj->params.params[i].type) 
                                        {
                                             ok = 0;
                                        }
                              }
                              if(ok == 0)
                              {
                                   errorCount++;
                                   yyerror("Param type is wrong");
                                   exit(0);
                              }
                              else
                              {  
                                   $$  = new ASTNode(obj->name.c_str(), obj->type.c_str(), "function");
                              }
                         }
                    }

               }
          }
     }
     | ID '(' ')'
     {
          IdInfo* obj = current->existsIdWithTypeAll(current, $1, "function");
          if (!obj) 
          {
               errorCount++;
               yyerror("Variable not declared");
          }
          else
          {
               if (obj->params.params.size() != 0)
               {
                    errorCount++;
                    yyerror("Param list size is wrong");
                    exit(0);
               } 
               else
               {
                    $$ = new ASTNode(obj->name.c_str(), obj->type.c_str()); 
               }   
               
          }
     }
     | ID '.' ID '(' ')'
      {
          if (!current->existsIdWithTypeAll(current, $1, "var"))
          {
               errorCount++;
               yyerror("Variable not declared");
          }
          else 
          {
               class SymTable* local;
               local = current->scope(current, $1);
               string idType = local->ids[$1].type; 

               class SymTable* global;
               global = local->scope(local, idType.c_str());
               if (global->ids[idType].idType != "class") 
               {
                    errorCount++;
                    yyerror("Variable is not an object of a class");
               }
               else
               {
                    class SymTable* clasa;
                    clasa = current->findClass(classes, idType.c_str());
                    IdInfo* obj = clasa->existsIdWithTypeCurr($3, "function");
                    if(!obj)
                    {
                         errorCount++;
                         yyerror("Variable is not a field of the class");
                    }
                    else
                    {
                         if (obj->params.params.size() != 0)
                         {
                              errorCount++;
                              yyerror("Param list size is wrong");
                              exit(0);
                         }    
                         else
                         {
                              $$ = new ASTNode(obj->name.c_str(), obj->type.c_str());     
                         }
                         
                    }

               }
          }
     }
     | INTEGER 
     {
          $$ = new ASTNode("int", Value($1, "int"));
     }
     | FLOAT
     {
         $$ = new ASTNode("float", Value($1, "float"));
     }
     | var
     {
          $$ = new ASTNode($1->name.c_str(), $1->type.c_str(), nullptr);
          if($1->idType == "var")
          {
               $$->value = $1->valoare;
          }
          else 
          {
               $$->value = Value($1->type.c_str());
          }
     }
     ;


%%
void yyerror(const char * s)
{
     cout << "error:" << s << " at line: " << yylineno << endl;
}

int main(int argc, char** argv)
{

     yyin=fopen(argv[1],"r");
     current = new SymTable(nullptr, "global");
     yyparse();
}