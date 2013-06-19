#include "proposition.h"

Proposition V(const string& name) {
  auto variable = make_shared<Proposition::Variable>();
  variable->name = name;
  return { variable };
}

Proposition operator"" _var (const char* name, size_t sz) {
  auto variable = make_shared<Proposition::Variable>();
  variable->name = string(name, sz);
  return { variable };
}

Proposition Proposition::implies(const Proposition& consequent) const {
  return  (!*this) || consequent;
};

Proposition Proposition::iff(const Proposition& equivalent) const {
  return this->implies(equivalent) && equivalent.implies(*this);
}

Proposition Proposition::operator!() const {
  auto negation = make_shared<Negation>();
  negation->target = value;
  return { negation };
}

Proposition Proposition::operator&&(const Proposition& conjunct) const {
  auto conjunction = make_shared<Conjunction>();
  conjunction->first_conjunct = value;
  conjunction->second_conjunct = conjunct.value;
  return { conjunction };
}

Proposition Proposition::operator||(const Proposition& disjunct) const {
  auto disjunction = make_shared<Disjunction>();
  disjunction->first_disjunct = value;
  disjunction->second_disjunct = disjunct.value;
  return { disjunction };
}

bool Proposition::evaluate(const set<string>& truth_assignment) const {
  return value->evaluate(truth_assignment);
}

set<set<string>> Proposition::evaluate_all(const set<string>& variables) const {
  set<set<string>> truth_assignments;

  vector<string> V(variables.begin(), variables.end());

  size_t N = V.size();

  for (size_t i = 0; i < (size_t(1) << N); ++i) {
    set<string> truth_assignment;

    for (size_t j = 0; j < N; ++j) {
      if (i & (1 << j)) {
        truth_assignment.insert(V[j]);
      }
    }

    if (evaluate(truth_assignment)) {
      truth_assignments.insert(truth_assignment);
    }
  }

  return truth_assignments;
}
