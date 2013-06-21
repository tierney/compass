// Example usage:
// ./wiki_main && python dot_label_to_var.py  > label.dot && dot -Tpng label.dot > toss.png

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <set>
#include <string>
#include <vector>
#include <map>

#include <bdd.h>

#include "norms.h"
// #include "expression.h"
#include "parser.hh"
#include "lexer.hh"

#include "proposition.h"

extern NBlock* normBlock;
extern int yyparse(void*);

using std::map;
using std::set;
using std::string;
using std::vector;

// int yyparse(SExpression **expression, yyscan_t scanner);

// SExpression *getAST(const char *expr) {
//   SExpression *expression;
//   yyscan_t scanner;
//   YY_BUFFER_STATE state;

//   if (yylex_init(&scanner)) {
//     // couldn't initialize
//     return NULL;
//   }

//   state = yy_scan_string(expr, scanner);

//   if (yyparse(&expression, scanner)) {
//     // error parsing
//     return NULL;
//   }

//   yy_delete_buffer(state, scanner);

//   yylex_destroy(scanner);

//   return expression;
// }

// int evaluate(SExpression *e) {
//   switch (e->type) {
//     case eSTRING:
//       printf("String: %s\n", e->string);
//       assert(V(e->string).evaluate({e->string}));
//       return 1;
//     case eVALUE:
//       return e->value;
//     case eMULTIPLY:
//       return evaluate(e->left) * evaluate(e->right);
//     case ePLUS:
//       return evaluate(e->left) + evaluate(e->right);
//     case eAND:
//       return evaluate(e->left) && evaluate(e->right);
//     case eOR:
//       return evaluate(e->left) || evaluate(e->right);
//     default:
//       // shouldn't be here
//       return 0;
//   }
// }

// Proposition prop_evaluate(SExpression* e, set<string>* vars) {
//   assert(vars != NULL);
//   switch (e->type) {
//     case eSTRING:
//       printf("String: %s\n", e->string);
//       vars->insert(e->string);
//       return V(e->string);
//     case eAND:
//       return prop_evaluate(e->left, vars) && prop_evaluate(e->right, vars);
//     case eOR:
//       return prop_evaluate(e->left, vars) || prop_evaluate(e->right, vars);
//     default:
//       assert(false);
//   }
// }

// int main(int argc, char **argv) {
//   SExpression *e = NULL;
//   // char test[]=" 4 + 2*10 + 3*( 5 + 1 )";
//   char test[] = "makeItHappen || hello";
//   int result = 0;

//   e = getAST(test);

//   result = evaluate(e);

//   printf("Result of '%s' is %d\n", test, result);

//   set<string> vars;
//   Proposition prop = prop_evaluate(e, &vars);
//   auto truth_assignments = prop.evaluate_all(vars);
//   for (auto truth_assignment : truth_assignments) {
//     for (auto variable : vars)
//       cout << (truth_assignment.count(variable) ? "1" : "0") << "    ";
//     cout << endl;
//   }

//   deleteExpression(e);

//   return 0;
// }

string method_to_str(NMethodCall* pmc) {
  vector<string> args;
  for (auto expr : pmc->arguments) {
    args.push_back(static_cast<NIdentifier*>(expr)->name);
  }

  string variable;
  variable = pmc->id.name + "(" + args[0];
  args.erase(args.begin());
  for (auto arg : args) {
    variable += "," + arg;
  }
  variable += ")";
  return variable;
}

bdd expression(NExpression& expr, set<string>* vars,
               map<string, bdd>* meth_to_bdd) {
  NBinaryOperator *pbp = NULL;
  NNegExpression *pne = NULL;
  NIdentifier *pi = NULL;
  NMethodCall *pmc = NULL;
  string ms;

  switch(expr.stype()) {
    case kNBINARYOPERATOR:
      std::cout << "binary" << std::endl;
      pbp = static_cast<NBinaryOperator*>(&expr);
      switch(pbp->op) {
        case (TOR):
          std::cout << "op: ||" << std::endl;
          return expression(pbp->lhs, vars, meth_to_bdd) |
              expression(pbp->rhs, vars, meth_to_bdd);
        case (TAND):
          std::cout << "op: &&" << std::endl;
          return expression(pbp->lhs, vars, meth_to_bdd) &
              expression(pbp->rhs, vars, meth_to_bdd);
        default:
          std::cout << "unknown operator" << std::endl;
          assert(false);
      }

    case kNMETHODCALL:
      pmc = static_cast<NMethodCall*>(&expr);
      ms.clear();
      ms = method_to_str(pmc);
      std::cout << ms << std::endl;
      vars->insert(ms);
      if (!meth_to_bdd) {
        bdd toss;
        return toss;
      }
      return meth_to_bdd->find(method_to_str(pmc))->second;

    case kNNEGEXPRESSION:
      std::cout << "neg!" << std::endl;
      pne = static_cast<NNegExpression*>(&expr);
      return !expression(pne->exp, vars, meth_to_bdd);

    // case kNIDENTIFIER:
    //   pi = static_cast<NIdentifier*>(&expr);
    //   std::cout << pi->name << std::endl;
    //   return;

    default:
      std::cout << expr.stype() << std::endl;
      std::cout << typeid(expr).name() << std::endl;
      assert(false);
  }
}

void evaluate(NStatement& stmt) {
  switch (stmt.stype()) {
    case kNEXPRESSIONSTATEMENT:
      std::cout << "expression" << std::endl;
      set<string> vars;
      // First pass gathers the variables (basically, the method calls).
      expression(static_cast<NExpressionStatement*>(&stmt)->expression,
                 &vars, NULL);
      for (auto var : vars) {
        std::cout << var << std::endl;
      }

      bdd_init(10000,1000);
      bdd_setvarnum(vars.size() + 1);
      std::cout << "Vars: " << vars.size() << std::endl;

      map<string, bdd> meth_to_bdd;
      int count = 0;
      std::ofstream fh_vars("vars.py");
      fh_vars << "var_to_label = {";
      for (auto var : vars) {
        fh_vars << count << " : \"" << var << "\"," << std::endl;
        meth_to_bdd[var] = bdd_ithvar(count);
        count++;
      }
      fh_vars << "}";
      fh_vars.close();

      bdd_varblockall();

      // Second pass plugs in the variables to generate the BDD.
      bdd res = expression(
          static_cast<NExpressionStatement*>(&stmt)->expression,
          &vars, &meth_to_bdd);

      bdd_printorder();
      bdd_reorder(BDD_REORDER_SIFTITE);
      bdd_printorder();

      FILE* fdot = fopen("toss.dot", "w");
      bdd_fprintdot(fdot, res);
      fclose(fdot);
      bdd_printset(res);

      std::ostringstream oss;
      oss << res;

      bdd_done();
      return;
  }
}

int main(int argc, char **argv) {
  const char *expr =
      "(inrole(p1,instructor) && inrole(p2,adboard) && subject(student) && attr(msg, discip)) ||"
      "(inrole(p1,chair) && inrole(p2,factencom) && subject(untenfac) && attr(msg, tencase)) ||"
      "(inrole(p1,instructor) && inrole(p2,admin) && subject(student) && attr(msg,grades)) ||"
      "(inrole(p1,student) && inrole(p2,admin) && subject(instructor) && attr(msg,courserating))";

  yyscan_t scanner;
  YY_BUFFER_STATE state;

  if (yylex_init(&scanner)) {
    // couldn't initialize
    return 0;
  }

  state = yy_scan_string(expr, scanner);

  yyparse(scanner);

  std::cout << normBlock << std::endl;

  for (auto stmt : normBlock->statements) {
    evaluate(*stmt);
  }

  return 0;
}
