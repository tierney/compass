#include <cassert>
#include <memory>
#include <set>
#include <vector>
#include <string>
#include <iostream>

#include "proposition.h"


int main() {
  assert(V("hello").evaluate({"hello"}));
  // assert(("hello"_var).evaluate({"hello"}));
  bool out = "hi"_var.evaluate({"hi"});
  assert(out);

  // assert(  ("foo"_var).evaluate({"foo"})); // trivially true
  // assert(  ("foo"_var).evaluate_all({"foo"})
  //          == set<set<string>> {{"foo"}} );

  // assert(  (!"foo"_var) .evaluate({})); // basic negation
  // assert(! (!"foo"_var) .evaluate({"foo"})); // basic negation
  // assert(  (!"foo"_var) .evaluate_all({"foo"})
  //          == set<set<string>> {{}} );

  // assert(  (!!"foo"_var) .evaluate({"foo"})); // double negation
  // assert(  (!!"foo"_var) .evaluate_all({"foo"})
  //          == set<set<string>> {{"foo"}} );

  // assert(  ("foo"_var && "bar"_var) .evaluate({"foo", "bar"})); // conjunction
  // assert(! ("foo"_var && "bar"_var) .evaluate({"bar"})); // conjunction
  // assert(! ("foo"_var && "bar"_var) .evaluate({"foo"})); // conjunction
  // assert(! ("foo"_var && "bar"_var) .evaluate({})); // conjunction
  // assert(  ("foo"_var && "bar"_var) .evaluate_all({"foo", "bar"})
  //          == set<set<string>>({{"foo", "bar"}}));

  // assert(  ("foo"_var || "bar"_var) .evaluate({"foo", "bar"})); // disjunction
  // assert(  ("foo"_var || "bar"_var) .evaluate({"bar"})); // disjunction
  // assert(  ("foo"_var || "bar"_var) .evaluate({"foo"})); // disjunction
  // assert(! ("foo"_var || "bar"_var) .evaluate({})); // disjunction
  // assert(  ("foo"_var || "bar"_var) .evaluate_all({"foo", "bar"})
  //          == set<set<string>>({{"foo", "bar"}, {"foo"}, {"bar"}}));

  // assert(  ("foo"_var.implies("bar"_var)) .evaluate({"foo", "bar"})); // implication
  // assert(  ("foo"_var.implies("bar"_var)) .evaluate({"bar"})); // implication
  // assert(! ("foo"_var.implies("bar"_var)) .evaluate({"foo"})); // implication
  // assert(  ("foo"_var.implies("bar"_var)) .evaluate({})); // implication
  // assert(  ("foo"_var.implies("bar"_var)) .evaluate_all({"foo", "bar"})
  //          == set<set<string>>({{"foo", "bar"}, {"bar"}, {}}));

  // assert(  ("foo"_var.iff("bar"_var)) .evaluate({"foo", "bar"})); // equivalence
  // assert(! ("foo"_var.iff("bar"_var)) .evaluate({"bar"})); // equivalence
  // assert(! ("foo"_var.iff("bar"_var)) .evaluate({"foo"})); //equivalence
  // assert(  ("foo"_var.iff("bar"_var)) .evaluate({})); // equivalence
  // assert(  ("foo"_var.iff("bar"_var)) .evaluate_all({"foo", "bar"})
  //          == set<set<string>>({{"foo", "bar"}, {}}));

  // cout << "((A and not B) implies C) and ((not A) iff (B and C)):" << endl << endl;

  // auto proposition = ("A"_var && !"B"_var).implies("C"_var) && (!"A"_var).iff("B"_var && "C"_var);

  auto prop0 = ("P"_var && "Q"_var);
  auto prop1 = (prop0 && "R"_var);
  auto prop2 = ("S"_var && "T"_var);
  auto prop3 = prop1 && prop2;
  auto proposition = prop3.implies("Q"_var && "S"_var);
  // auto proposition = ((("P"_var && "Q"_var) && "R"_var) && ("S"_var && "T"_var))
  //     .implies("Q"_var && "S"_var);
  auto truth_assignments = proposition.evaluate_all({"P", "Q", "R", "S", "T"});


  cout << "P    Q    R    S    T" << endl;
  cout << "---------------------" << endl;

  for (auto truth_assignment : truth_assignments) {
    for (auto variable : {"P", "Q", "R", "S", "T"})
      cout << (truth_assignment.count(variable) ? "1" : "0") << "    ";
    cout << endl;
  }
  return 0;
}
