%{
#include <string>

#include "norms.h"
  // /#include "expression.h"
#include "parser.hh"
#include "lexer.hh"

/* int yyerror(yyscan_t scanner, SExpression **expression, const char *msg) { */
/*   fprintf(stderr,"SPECIAL Error:%s %s\n",msg, (*expression)->string); */
/*   return 0; */
/* } */
/* int yyerror(SExpression **expression, yyscan_t scanner, const char *msg) { */
/*   fprintf(stderr,"SPECIAL Error:%s %s\n",msg, (*expression)->string); */
/*   return 0; */
/* } */

NBlock *normBlock;
extern int yylex();
// void yyerror(const char *s) { printf("ERROR: %s\n", s); }
void yyerror(const char *s, const char *msg) { printf("ERROR: %s (%s)\n", s, msg); }
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
// %parse-param { SExpression **expression }
%parse-param { yyscan_t scanner }

%union {
  int value;
  // SExpression *expression;

  Norm *node;
  NBlock *block;
  NExpression *expr;
  NStatement *stmt;
  NIdentifier *ident;
  NVariableDeclaration *var_decl;
  std::vector<NVariableDeclaration*> *varvec;
  std::vector<NExpression*> *exprvec;
  std::string *string;
  int token;
}

%left '+' TOKEN_PLUS
%left '*' TOKEN_MULTIPLY

%left "&&" TAND
%left "||" TOR

%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token TOKEN_PLUS
%token TOKEN_MULTIPLY
%token TOKEN_AND
%token TOKEN_OR

%token <value> TOKEN_NUMBER
%token <string> TOKEN_STRING TIDENTIFIER
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT
%token <token> TPLUS TMINUS TMUL TDIV TAND TOR TNEG

%type <ident> ident
%type <expr> expr
%type <varvec> func_decl_args
%type <exprvec> call_args
%type <block> program stmts block
%type <stmt> stmt var_decl func_decl
%type <token> comparison
%type <token> conjunction

%start program

%%

program : stmts { normBlock = $1; }

stmts : stmt { $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
      | stmts stmt { $1->statements.push_back($<stmt>2); }
      ;

stmt : var_decl | func_decl
     | expr { $$ = new NExpressionStatement(*$1); }
     ;

block : TLBRACE stmts TRBRACE { $$ = $2; }
      | TLBRACE TRBRACE { $$ = new NBlock(); }
      ;

var_decl : ident ident { $$ = new NVariableDeclaration(*$1, *$2); }
         | ident ident TEQUAL expr { $$ = new NVariableDeclaration(*$1, *$2, $4); }
         ;

func_decl : ident ident TLPAREN func_decl_args TRPAREN block
            { $$ = new NFunctionDeclaration(*$1, *$2, *$4, *$6); delete $4; }
          ;

func_decl_args : /*blank*/  { $$ = new VariableList(); }
          | var_decl { $$ = new VariableList(); $$->push_back($<var_decl>1); }
          | func_decl_args TCOMMA var_decl { $1->push_back($<var_decl>3); }
          ;

ident : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
      ;

expr : ident TLPAREN call_args TRPAREN { $$ = new NMethodCall(*$1, *$3); delete $3; }
     | ident { $<ident>$ = $1; }
     | expr comparison expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
     | expr conjunction expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
     | TLPAREN expr TRPAREN { $$ = $2; }
     | TNEG expr { $$ = new NNegExpression(*$2, true); }
     ;

call_args : /*blank*/  { $$ = new ExpressionList(); }
          | expr { $$ = new ExpressionList(); $$->push_back($1); }
          | call_args TCOMMA expr  { $1->push_back($3); }
          ;

comparison : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE
           | TPLUS | TMINUS | TMUL | TDIV
           ;

conjunction : TAND | TOR
            ;
%%
