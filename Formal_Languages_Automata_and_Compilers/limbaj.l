%{
#include <stdio.h>
#include "limbaj.tab.h"
%}
%option noyywrap
nr 0|[1-9][0-9]*
fr [0-9]*
str [^"]*
%%
"int"|"float"|"char"|"string"|"bool" {yylval.string = strdup(yytext);return TYPE;}
"begin_progr" {return BGIN;}
"end_progr" {return END;}
"Class" {return CLASS;}
"private"|"public" {return ACCESS;}
"if" {return IF;}
"else" {return ELSE;}
"while" {return WHILE;}
"for" {return FOR;}
"Print" {return PRINT;}
"TypeOf" {return TYPEOF;}
"true" {yylval.string = strdup(yytext); return TRUE;}
"false" {yylval.string = strdup(yytext); return FALSE;}
"return" {return RETURN;}
[a-zA-Z][a-zA-Z0-9]* {yylval.string = strdup(yytext); return ID;}
"=" {return ASSIGN;}
"+=" {yylval.string = strdup(yytext); return ADD_ASSIGN;}
"-=" {yylval.string = strdup(yytext); return MINUS_ASSIGN;}
"/=" {yylval.string = strdup(yytext); return DIV_ASSIGN;}
"%=" {yylval.string = strdup(yytext); return MOD_ASSIGN;}
"*=" {yylval.string = strdup(yytext); return MUL_ASSIGN;}
"++" {yylval.string = strdup(yytext);return INCREMENT;}
"--" {yylval.string = strdup(yytext);return DECREMENT;}
[-]?{nr} {yylval.intval = atoi(yytext); return INTEGER;}
[-]?{nr}"."{fr} {yylval.floatval = strtof(yytext, NULL); return FLOAT;}
['][^']['] {yylval.string = strdup(yytext);return CHAR;}
["]{str}["] {yylval.string = strdup(yytext);return STRING;}
"!" {return NOT;}
"<=" {return LE; }
">=" {return GE; }
"==" {return EQ; }
"!=" {return NE; }
"<"	 {return LT; }
">"	 {return GT; }
"&&" {return AND; }
"||" {return OR; }
[ \t] ;
\n {yylineno++;}
. {return yytext[0];}
%%