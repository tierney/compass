#include <cassert>
#include <iostream>
#include <cstdio>
#include <set>
#include <string>

#include "norms.h"
#include "expression.h"
#include "parser.hh"
#include "lexer.hh"

#include "proposition.h"

using std::set;
using std::string;

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
      assert(V(e->string).evaluate({e->string}));
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

Proposition prop_evaluate(SExpression* e, set<string>* vars) {
  assert(vars != NULL);
  switch (e->type) {
    case eSTRING:
      printf("String: %s\n", e->string);
      vars->insert(e->string);
      return V(e->string);
    case eAND:
      return prop_evaluate(e->left, vars) && prop_evaluate(e->right, vars);
    case eOR:
      return prop_evaluate(e->left, vars) || prop_evaluate(e->right, vars);
    default:
      assert(false);
  }
}

int main(int argc, char **argv) {
  SExpression *e = NULL;
  // char test[]=" 4 + 2*10 + 3*( 5 + 1 )";
  char test[] = "makeItHappen || hello";
  int result = 0;

  e = getAST(test);

  result = evaluate(e);

  printf("Result of '%s' is %d\n", test, result);

  set<string> vars;
  Proposition prop = prop_evaluate(e, &vars);
  auto truth_assignments = prop.evaluate_all(vars);
  for (auto truth_assignment : truth_assignments) {
    for (auto variable : vars)
      cout << (truth_assignment.count(variable) ? "1" : "0") << "    ";
    cout << endl;
  }

  deleteExpression(e);

  return 0;
}
