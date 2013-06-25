#pragma once

#include <map>
#include <string>
#include <vector>

#include "bdd.h"

using std::map;
using std::string;
using std::vector;

namespace compass {
class BDDNode {
 public:
  BDDNode() : yes(NULL), no(NULL), accept(false), recv_rule_(false) {}

  virtual ~BDDNode() {
    // if (NULL != yes) {
    //   delete yes;
    // }
    // if (NULL != no) {
    //   delete no;
    // }
  }

  void set_func(const string& func) {
    func_.clear();
    func_ = func;
  }

  string func() const {
    return func_;
  }

  void Print() const {
    std::cout << " ";
    if (yes != NULL) {
      yes->Print();
    }
    std::cout << func_ << std::endl;
    if (no) {
      no->Print();
    }
    std::cout << std::endl;
  }

  BDDNode *yes;
  BDDNode *no;
  bool accept;
  string p2;

 private:
  bool recv_rule_;
  string func_;
};


class BDDTree {
 public:
  BDDTree() : root_(NULL) {}

  virtual ~BDDTree();

  bool Query(const string& query, vector<string>* receivers);
  void Parse(bdd root, const map<int, string>& bdd_id_to_meth);
  void Print() const;

 private:

  BDDNode *root_;
};

} // namespace compass
