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
  BDDNode() : pyes(NULL), no(NULL), recv_rule_(false) {}

  virtual ~BDDNode() {
    // if (NULL != pyes) {
    //   delete pyes;
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
    if (pyes != NULL) {
      pyes->Print();
    }
    std::cout << func_ << std::endl;
    if (no) {
      no->Print();
    }
    std::cout << std::endl;
  }

  BDDNode *pyes;
  BDDNode *no;

 private:
  bool recv_rule_;
  string func_;
};


class BDDTree {
 public:
  BDDTree() {}

  virtual ~BDDTree();

  bool Query(const string& query, vector<string>* receivers);
  void Parse(bdd root, const map<int, string>& bdd_id_to_meth);
  void Print() const;

 private:

  BDDNode *root_;
};

} // namespace compass
