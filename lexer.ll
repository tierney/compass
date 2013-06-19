%{

/*
 * Lexer.l file
 * To generate the lexical analyzer run: "flex Lexer.l"
 */

#include "expression.h"
#include "parser.hh"

#include <cstdio>
#include <string>

/* #define YY_DECL extern "C" int yylex() */

%}
%option outfile="lexer.cc" header-file="lexer.hh"

%option warn nodefault

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

LPAREN      "("
RPAREN      ")"
PLUS        "+"
MULTIPLY    "*"
AND         "&&"
OR          "||"

NUMBER      [0-9]+
WS          [ \r\n\t]*
STRING      [a-zA-Z0-9]+

%%

{WS}            { /* Skip blanks. */ }
{NUMBER}        { sscanf(yytext, "%d", &yylval->value); return TOKEN_NUMBER; }

{STRING}        { yylval->string = strdup(yytext); return TOKEN_STRING; }

{MULTIPLY}      { return TOKEN_MULTIPLY; }
{PLUS}          { return TOKEN_PLUS; }
{LPAREN}        { return TOKEN_LPAREN; }
{RPAREN}        { return TOKEN_RPAREN; }
{AND}           { return TOKEN_AND; }
{OR}            { return TOKEN_OR; }
.               {  }

%%

int yyerror(const char *msg) {
    fprintf(stderr,"Error:%s\n",msg); return 0;
}
