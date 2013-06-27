// Example usage:
// ./wiki_main && python dot_label_to_var.py  > label.dot && dot -Tpng label.dot > toss.png

#include <time.h>
#include <cassert>
#include <cstdlib>
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
#include "compass_types.h"

extern NBlock* normBlock;
extern int yyparse(void*);

using std::map;
using std::set;
using std::string;
using std::vector;


// template <typename... Args>
// typedef bool (*BFunc)(Args...);

#include <tuple>

template <typename... Args>
struct variadic_typedef
{
    // this single type represents a collection of types,
    // as the template arguments it took to define it
};

template <typename... Args>
struct convert_in_tuple
{
    // base case, nothing special,
    // just use the arguments directly
    // however they need to be used
    typedef std::tuple<Args...> type;
};

template <typename... Args>
struct convert_in_map {
  typedef std::map<string, Args...> type;
};

template <typename... Args>
struct convert_in_tuple<variadic_typedef<Args...>>
{
    // expand the variadic_typedef back into
    // its arguments, via specialization
    // (doesn't rely on functionality to be provided
    // by the variadic_typedef struct itself, generic)
    typedef typename convert_in_tuple<Args...>::type type;
};

template <typename... Args>
struct convert_in_map<variadic_typedef<Args...> > {
  typedef typename convert_in_map<Args...>::type type;
};

typedef bool (*OneArg)(const string&);
typedef bool (*TwoArg)(const string&, const string&);
typedef bool (*ThreeArgs)(const string&, const string&, const string&);
typedef variadic_typedef<OneArg, TwoArg, ThreeArgs> FuncTypes;

typedef convert_in_tuple<FuncTypes>::type FuncTuple;

typedef variadic_typedef<int, float> myTypes;
typedef convert_in_tuple<myTypes>::type int_float_tuple;



timespec diff(timespec start, timespec end);

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

      timespec start, end;

      bdd_printorder();
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
      bdd_reorder(BDD_REORDER_SIFTITE);
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
      std::cout << "REORDER " << diff(start,end).tv_sec << ":" << diff(start,end).tv_nsec << std::endl;
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
      compass::Post post;
      post.q = "untenfac";
      const int queries = 5000000;


      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
      bool accept = false;
      for (int i = 0; i < queries; i++)
        accept = tree.Query(post, &recvs);
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
      std::cout << diff(start,end).tv_sec << ":" << diff(start,end).tv_nsec << std::endl;
      int64_t millis = (diff(start,end).tv_sec * 1000000000 + diff(start,end).tv_nsec) / 1000000;

      std::cout << "time (ms) to query " << millis << std::endl;
      std::cout << "QPS: " <<  queries / (millis / 1000.) << std::endl;
      std::cout << "Accept? " << (accept ? "yes" : "no") << std::endl;

      bdd_done();
      return;
  }
}
timespec diff(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

bool toss(const string& onearg) {
  return true;
}

int main(int argc, char **argv) {
  srand (time(NULL));

  // map<string, FuncTuple> mymap;
  // mymap["inrole"] = toss;

  const char *expr =
      "(inrole(p1,instructor) && inrole(p2,adboard) && subject(student) && attr(msg, discip)) ||"
      "(inrole(p1,chair) && inrole(p2,factencom) && subject(untenfac) && attr(msg, tencase)) ||"
      "(inrole(p1,instructor) && inrole(p2,admin) && subject(student) && attr(msg,grades)) ||"
      "(inrole(p1,student) && inrole(p2,admin) && subject(instructor) && attr(msg,courserating))";

  // const char *expr =
  //     "(inrole(p1,instructor) && inrole(p2,student) && attr(msg,announcement)) ||"
  //     "(inrole(p1,instructor) && inrole(p2,teamXmember) && attr(msg,teamX)) ||"
  //     "(inrole(p1,teamXmember) && inrole(p2,teamXmember) && attr(msg,teamX)) || "
  //     "(inrole(p1,student) && inrole(p2,instructor) && subject(student) && attr(msg,grades)) ||"
  //     "(inrole(p1,student) && inrole(p2,student) && attr(msg,instructor))";

  // const char *expr =
  //     "(inrole(p1,generation0) && inrole(p2,elder) && subject(p1) && attr(msg, geneticDisease)) ||"
  //     "(inrole(p1,generation0) && inrole(p2,generation0) && attr(msg, finances)) ||"
  //     "(inrole(p1,generation1) && inrole(p2,generation1) && attr(msg, lowAcademicPerf)) ||"
  //     "(inrole(p1,generation1) && inrole(p2,generation1) && attr(msg, parties))";


  // const char *expr = "inrole(p2, friend) || inrole(p2, fof)";

  // const char *expr = "(inrole(p1, circleCreator) && inrole(p2, circleMember)) || "
  //     "(inrole(p1, circleMember) && attr(msg, limited))";

  yyscan_t scanner;
  YY_BUFFER_STATE state;

  if (yylex_init(&scanner)) {
    // couldn't initialize
    return 0;
  }

  timespec start, end;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
  state = yy_scan_string(expr, scanner);

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
  std::cout << "LEX " << diff(start,end).tv_sec << ":" << diff(start,end).tv_nsec << std::endl;
  int64_t millis = (diff(start,end).tv_sec * 1000000000 + (float)diff(start,end).tv_nsec) / 1000000;
  std::cout << "LEX time (ms) to query " << millis << std::endl;

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
  yyparse(scanner);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
  std::cout << "PARSE" << diff(start,end).tv_sec << ":" << diff(start,end).tv_nsec << std::endl;
  millis = (diff(start,end).tv_sec * 1000000000 + diff(start,end).tv_nsec) / 1000000;
  std::cout << "PARSE time (ms) to query " << millis << std::endl;


  std::cout << normBlock << std::endl;

  for (auto stmt : normBlock->statements) {
    evaluate(*stmt);
  }

  return 0;
}
