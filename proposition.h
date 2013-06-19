// (C) 2012, Andrew Tomazos <andrew@tomazos.com>.  Public domain.

#pragma once

#include <cassert>
#include <memory>
#include <set>
#include <vector>
#include <string>
#include <iostream>

using std::set;
using std::string;
using std::shared_ptr;
using std::make_shared;
using std::vector;
using std::cout;
using std::endl;

class Proposition;

// The expression...
//
//     "foo"_var
//
// ...creates an atomic proposition variable with the name 'foo'
Proposition operator"" _var (const char*, size_t);
Proposition V(const string& name);

// Represents a compound proposition
class Proposition {
 public:
  // A.implies(B): means that A (antecendant) implies ==> B (consequent)
  Proposition implies(const Proposition& consequent) const;

  // A.iff(B): implies that A and B form an equivalence. A <==> B
  Proposition iff(const Proposition& equivalent) const;

  // !A: the negation of target A
  Proposition operator!() const;

  // A && B: the conjunction of A and B
  Proposition operator&&(const Proposition& conjunct) const;

  // A || B: the disjunction of A and B
  Proposition operator||(const Proposition& disjunct) const;

  // A.evaluate(T): Given a set T of variable names that are true (a truth
  //     assignment), will return the truth {true, false} of the proposition
  bool evaluate(const set<string>& truth_assignment) const;

  // A.evaluate_all(S): Given a set S of variables,
  //     will return the set of truth assignments that make this proposition true
  set<set<string>> evaluate_all(const set<string>& variables) const;

 private:

  struct Base {
    virtual bool evaluate(const set<string>& truth_assignment) const = 0;
  };

  typedef shared_ptr<Base> pointer;
  pointer value;

  Proposition(const pointer& value_) : value(value_) {}

  struct Variable : Base {
    string name;
    virtual bool evaluate(const set<string>& truth_assignment) const {
      return truth_assignment.count(name);
    }
  };

  struct Negation : Base {
    pointer target;
    bool evaluate(const set<string>& truth_assignment) const {
      return !target->evaluate(truth_assignment);
    }
  };

  struct Conjunction : Base {
    pointer first_conjunct, second_conjunct;

    bool evaluate(const set<string>& truth_assignment) const {
      return first_conjunct->evaluate(truth_assignment)
          && second_conjunct->evaluate(truth_assignment);
    }
  };

  struct Disjunction : Base {
    pointer first_disjunct, second_disjunct;

    bool evaluate(const set<string>& truth_assignment) const {
      return first_disjunct->evaluate(truth_assignment)
          || second_disjunct->evaluate(truth_assignment);
    }
  };

  friend Proposition operator"" _var (const char* name, size_t sz);
  friend Proposition V(const string& name);
};
