%{

/*
 * Parser.y file
 * To generate the parser run: "bison Parser.y"
 */

#include <string>

#include "expression.h"
#include "parser.hh"
#include "lexer.hh"

namespace {
int yyerror(yyscan_t scanner,
                   SExpression **expression,
                   const char *msg) {
  fprintf(stderr,"SPECIAL Error:%s %s\n",msg, (*expression)->string); return 0;
}
int yyerror(
                   SExpression **expression,
           yyscan_t scanner,        const char *msg) {
  fprintf(stderr,"SPECIAL Error:%s %s\n",msg, (*expression)->string); return 0;
}
}
%}

%code requires {

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

}

%output  "parser.cc"
%defines "parser.hh"

%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { SExpression **expression }
%parse-param { yyscan_t scanner }

%union {
  char *string;
  int value;
  SExpression *expression;
}

%left '+' TOKEN_PLUS
%left '*' TOKEN_MULTIPLY
%left "&&" TOKEN_AND
%left "||" TOKEN_OR

%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token TOKEN_PLUS
%token TOKEN_MULTIPLY
%token TOKEN_AND
%token TOKEN_OR

%token <value> TOKEN_NUMBER
%token <string> TOKEN_STRING

%type <expression> expr

%%

input
    : expr { *expression = $1; }
    ;

expr
    : expr TOKEN_PLUS expr { $$ = createOperation( ePLUS, $1, $3 ); }
    | expr TOKEN_MULTIPLY expr { $$ = createOperation( eMULTIPLY, $1, $3 ); }
    | expr TOKEN_AND expr { $$ = createOperation( eAND, $1, $3 ); }
    | expr TOKEN_OR expr { $$ = createOperation( eOR, $1, $3 ); }
    | TOKEN_LPAREN expr TOKEN_RPAREN { $$ = $2; }
    | TOKEN_NUMBER { $$ = createNumber($1); }
    | TOKEN_STRING { $$ = createString($1); }
    ;
%%
