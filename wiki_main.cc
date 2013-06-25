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

#include "bdd_tree.h"
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
      map<int, string> bdd_id_to_meth;
      int count = 0;
      std::ofstream fh_vars("vars.py");
      fh_vars << "var_to_label = {";
      for (auto var : vars) {
        fh_vars << count << " : \"" << var << "\"," << std::endl;
        meth_to_bdd[var] = bdd_ithvar(count);
        bdd_id_to_meth[count] = var;
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

      // bdd_printset(res);
      bdd_printtable(res);

      compass::BDDTree tree;
      tree.Parse(res, bdd_id_to_meth);
      tree.Print();
      vector<string> recvs;
      tree.Query("hello", &recvs);

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
