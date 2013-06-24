#include "bdd_tree.h"

#include <string.h>
#include <cassert>
#include <sstream>
#include <string>
#include <map>

#include <re2/re2.h>

using std::map;
using std::string;

namespace compass {

BDDTree::~BDDTree() {
  delete root_;
}

void BDDTree::Parse(bdd res, const map<int, string>& bdd_id_to_meth) {
  std::ostringstream oss;
  oss << res;
  std::cout << oss.str() << std::endl;

  string value;
  bool accept;
  bool first = false;
  compass::BDDNode *new_node = NULL;
  compass::BDDNode **prev_node_state = NULL;
  map<string, compass::BDDNode *> func_to_node;
  string meth;
  bool matched = false;
  string function;
  string arg0, arg1;
  for (char mychar : oss.str()) {
    // std::cout << "--> " << mychar << std::endl;
    switch (mychar) {
      case ' ':
        continue;
      case '<':
        // std::cout << "new set" << std::endl;
        first = true;
        continue;
      case ',':
        // Do something with the value;
        accept = stoi(value);
        assert(accept == 0 || accept == 1);
        if (accept) {
          prev_node_state = &(new_node->pyes);
        } else {
          prev_node_state = &(new_node->no);
        }
        value.clear();
        // std::cout << "end of set/node" << std::endl;
       continue;
      case '>':
        prev_node_state = NULL;
        continue;
      case ':':
        // Do something with the value;
        if (!value.empty()) {
          // std::cout << stoi(value) << std::endl;
        }

        meth = bdd_id_to_meth[stoi(value)];
        if (func_to_node.find(meth) == func_to_node.end()) {
          new_node = new compass::BDDNode();
          std::cout << "New node " << meth << std::endl;
          matched = RE2::FullMatch(meth, "(\\w+)\\((\\w+),(\\w+)\\)", &function, &arg0, &arg1);
          if (matched) {
            std::cout << function << " " << arg0 << " " << arg1 << std::endl;
          } else {
            matched = RE2::FullMatch(meth, "(\\w+)\\((\\w+)\\)", &function, &arg0);
            std::cout << function << " " << arg0 << std::endl;
          }
          new_node->set_func(meth);
          func_to_node[meth] = new_node;
        } else {
          std::cout << "Found previously created node." << std::endl;
          new_node = func_to_node.find(meth)->second;
        }
        if (NULL != prev_node_state) {
          std::cout << "previous node set" << std::endl;
          *prev_node_state = new_node;
        }
        if (first) {
          root_ = new_node;
          first = false;
        }
        value.clear();
        continue;
      default:
        value += mychar;
        continue;
    }
  }
}

void BDDTree::Print() const {
  root_->Print();
}

bool BDDTree::Query(const string& query, vector<string>* receivers) {
  return true;
}

} // namespace compass
