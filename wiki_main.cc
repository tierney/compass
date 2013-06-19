#include <iostream>
#include <cstdio>

#include "expression.h"
#include "parser.hh"
#include "lexer.hh"

int yyparse(SExpression **expression, yyscan_t scanner);

SExpression *getAST(const char *expr) {
  SExpression *expression;
  yyscan_t scanner;
  YY_BUFFER_STATE state;

  if (yylex_init(&scanner)) {
    // couldn't initialize
    return NULL;
  }

  state = yy_scan_string(expr, scanner);

  if (yyparse(&expression, scanner)) {
    // error parsing
    return NULL;
  }

  yy_delete_buffer(state, scanner);

  yylex_destroy(scanner);

  return expression;
}

int evaluate(SExpression *e) {
  switch (e->type) {
    case eSTRING:
      printf("String: %s\n", e->string);
      return 1;
    case eVALUE:
      return e->value;
    case eMULTIPLY:
      return evaluate(e->left) * evaluate(e->right);
    case ePLUS:
      return evaluate(e->left) + evaluate(e->right);
    case eAND:
      return evaluate(e->left) && evaluate(e->right);
    case eOR:
      return evaluate(e->left) || evaluate(e->right);
    default:
      // shouldn't be here
      return 0;
  }
}

int main(int argc, char **argv) {
  SExpression *e = NULL;
  // char test[]=" 4 + 2*10 + 3*( 5 + 1 )";
  char test[] = "4 + makeItHappen + hello";
  int result = 0;

  e = getAST(test);

  result = evaluate(e);

  printf("Result of '%s' is %d\n", test, result);

  deleteExpression(e);

  return 0;
}
