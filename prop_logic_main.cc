// (C) 2012, Andrew Tomazos <andrew@tomazos.com>.  Public domain.

#include <cassert>
#include <memory>
#include <set>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

struct Proposition;

// The expression...
//
//     "foo"_var
//
// ...creates an atomic proposition variable with the name 'foo'
Proposition operator"" _var (const char*, size_t);

// Represents a compound proposition
struct Proposition
{
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

    // A.evaluate(T): Given a set T of variable names that are true (a truth assignment),
    //     will return the truth {true, false} of the proposition
    bool evaluate(const set<string>& truth_assignment) const;

    // A.evaluate_all(S): Given a set S of variables,
    //     will return the set of truth assignments that make this proposition true
    set<set<string>> evaluate_all(const set<string>& variables) const;

private:

    struct Base { virtual bool evaluate(const set<string>& truth_assignment) const = 0; };

    typedef shared_ptr<Base> pointer;

    pointer value;
    Proposition(const pointer& value_) : value(value_) {}

    struct Variable : Base
    {
        string name;
        virtual bool evaluate(const set<string>& truth_assignment) const
        {
            return truth_assignment.count(name);
        }
    };

    struct Negation : Base
    {
        pointer target;
        bool evaluate(const set<string>& truth_assignment) const
        {
            return !target->evaluate(truth_assignment);
        }
    };

    struct Conjunction : Base
    {
        pointer first_conjunct, second_conjunct;

        bool evaluate(const set<string>& truth_assignment) const
        {
            return first_conjunct->evaluate(truth_assignment)
                && second_conjunct->evaluate(truth_assignment);
        }
    };

    struct Disjunction : Base
    {
        pointer first_disjunct, second_disjunct;

        bool evaluate(const set<string>& truth_assignment) const
        {
            return first_disjunct->evaluate(truth_assignment)
                || second_disjunct->evaluate(truth_assignment);
        }
    };

    friend Proposition operator"" _var (const char* name, size_t sz);
  friend Proposition V(const string& name);
};

Proposition V(const string& name) {
  auto variable = make_shared<Proposition::Variable>();
  variable->name = name;
  return { variable };
}

Proposition operator"" _var (const char* name, size_t sz)
{
    auto variable = make_shared<Proposition::Variable>();
    variable->name = string(name, sz);
    return { variable };
}

Proposition Proposition::implies(const Proposition& consequent) const
{
    return  (!*this) || consequent;
};

Proposition Proposition::iff(const Proposition& equivalent) const
{
    return this->implies(equivalent) && equivalent.implies(*this);
}

Proposition Proposition::operator!() const
{
    auto negation = make_shared<Negation>();
    negation->target = value;
    return { negation };
}

Proposition Proposition::operator&&(const Proposition& conjunct) const
{
    auto conjunction = make_shared<Conjunction>();
    conjunction->first_conjunct = value;
    conjunction->second_conjunct = conjunct.value;
    return { conjunction };
}

Proposition Proposition::operator||(const Proposition& disjunct) const
{
    auto disjunction = make_shared<Disjunction>();
    disjunction->first_disjunct = value;
    disjunction->second_disjunct = disjunct.value;
    return { disjunction };
}

bool Proposition::evaluate(const set<string>& truth_assignment) const
{
    return value->evaluate(truth_assignment);
}

set<set<string>> Proposition::evaluate_all(const set<string>& variables) const
{
    set<set<string>> truth_assignments;

    vector<string> V(variables.begin(), variables.end());

    size_t N = V.size();

    for (size_t i = 0; i < (size_t(1) << N); ++i)
    {
        set<string> truth_assignment;

        for (size_t j = 0; j < N; ++j)
            if (i & (1 << j))
                truth_assignment.insert(V[j]);

        if (evaluate(truth_assignment))
            truth_assignments.insert(truth_assignment);
    }

    return truth_assignments;
}

int main() {
  assert(V("hello").evaluate({"hello"}));

    assert(  ("foo"_var) .evaluate({"foo"})); // trivially true
    assert(  ("foo"_var) .evaluate_all({"foo"})
             == set<set<string>> {{"foo"}} );

    assert(  (!"foo"_var) .evaluate({})); // basic negation
    assert(! (!"foo"_var) .evaluate({"foo"})); // basic negation
    assert(  (!"foo"_var) .evaluate_all({"foo"})
             == set<set<string>> {{}} );

    assert(  (!!"foo"_var) .evaluate({"foo"})); // double negation
    assert(  (!!"foo"_var) .evaluate_all({"foo"})
             == set<set<string>> {{"foo"}} );

    assert(  ("foo"_var && "bar"_var) .evaluate({"foo", "bar"})); // conjunction
    assert(! ("foo"_var && "bar"_var) .evaluate({"bar"})); // conjunction
    assert(! ("foo"_var && "bar"_var) .evaluate({"foo"})); // conjunction
    assert(! ("foo"_var && "bar"_var) .evaluate({})); // conjunction
    assert(  ("foo"_var && "bar"_var) .evaluate_all({"foo", "bar"})
             == set<set<string>>({{"foo", "bar"}}));

    assert(  ("foo"_var || "bar"_var) .evaluate({"foo", "bar"})); // disjunction
    assert(  ("foo"_var || "bar"_var) .evaluate({"bar"})); // disjunction
    assert(  ("foo"_var || "bar"_var) .evaluate({"foo"})); // disjunction
    assert(! ("foo"_var || "bar"_var) .evaluate({})); // disjunction
    assert(  ("foo"_var || "bar"_var) .evaluate_all({"foo", "bar"})
             == set<set<string>>({{"foo", "bar"}, {"foo"}, {"bar"}}));

    assert(  ("foo"_var.implies("bar"_var)) .evaluate({"foo", "bar"})); // implication
    assert(  ("foo"_var.implies("bar"_var)) .evaluate({"bar"})); // implication
    assert(! ("foo"_var.implies("bar"_var)) .evaluate({"foo"})); // implication
    assert(  ("foo"_var.implies("bar"_var)) .evaluate({})); // implication
    assert(  ("foo"_var.implies("bar"_var)) .evaluate_all({"foo", "bar"})
             == set<set<string>>({{"foo", "bar"}, {"bar"}, {}}));

    assert(  ("foo"_var.iff("bar"_var)) .evaluate({"foo", "bar"})); // equivalence
    assert(! ("foo"_var.iff("bar"_var)) .evaluate({"bar"})); // equivalence
    assert(! ("foo"_var.iff("bar"_var)) .evaluate({"foo"})); //equivalence
    assert(  ("foo"_var.iff("bar"_var)) .evaluate({})); // equivalence
    assert(  ("foo"_var.iff("bar"_var)) .evaluate_all({"foo", "bar"})
             == set<set<string>>({{"foo", "bar"}, {}}));

    cout << "((A and not B) implies C) and ((not A) iff (B and C)):" << endl << endl;

    // auto proposition = ("A"_var && !"B"_var).implies("C"_var) && (!"A"_var).iff("B"_var && "C"_var);

    auto proposition = ((("P"_var && "Q"_var) && "R"_var) && ("S"_var && "T"_var))
        .implies("Q"_var && "S"_var);
    auto truth_assignments = proposition.evaluate_all({"P", "Q", "R", "S", "T"});

    cout << "A    B    C" << endl;
    cout << "-----------" << endl;

    for (auto truth_assignment : truth_assignments)
    {
      for (auto variable : {"P", "Q", "R", "S", "T"})
            cout << (truth_assignment.count(variable) ? "1" : "0") << "    ";
        cout << endl;
    }
}
