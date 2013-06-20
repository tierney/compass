%{

/*
 * Lexer.l file
 * To generate the lexical analyzer run: "flex Lexer.l"
 */

#include <cstdio>
#include <string>

#include "norms.h"
  // #include "expression.h"
#include "parser.hh"

#define SAVE_TOKEN yylval->string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval->token = t)
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

[ \t\n]                 ;
[a-zA-Z_][a-zA-Z0-9_]*  SAVE_TOKEN; return TIDENTIFIER;
"="                     return TOKEN(TEQUAL);
"=="                    return TOKEN(TCEQ);
"!="                    return TOKEN(TCNE);
"<"                     return TOKEN(TCLT);
"<="                    return TOKEN(TCLE);
">"                     return TOKEN(TCGT);
">="                    return TOKEN(TCGE);
"("                     return TOKEN(TLPAREN);
")"                     return TOKEN(TRPAREN);
"{"                     return TOKEN(TLBRACE);
"}"                     return TOKEN(TRBRACE);
"."                     return TOKEN(TDOT);
","                     return TOKEN(TCOMMA);
"+"                     return TOKEN(TPLUS);
"-"                     return TOKEN(TMINUS);
"*"                     return TOKEN(TMUL);
"/"                     return TOKEN(TDIV);
"&&"                    return TOKEN(TAND);
"||"                    return TOKEN(TOR);
"!"                     return TOKEN(TNEG);
.                       printf("Unknown token!\n"); yyterminate();

%%

/* int yyerror(const char *msg) { */
/*     fprintf(stderr,"Error:%s\n",msg); return 0; */
/* } */
