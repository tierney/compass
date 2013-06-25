#include "bdd_tree.h"

#include <string.h>
#include <cassert>
#include <sstream>
#include <string>
#include <map>
#include <typeinfo>
#include <functional>

#include <re2/re2.h>

#include "compass_types.h"

using std::map;
using std::string;

namespace compass {

typedef bool (*PostFunc)(const Post&, const string&, const string&);
// typedef std::function<bool(void)> MyFunc;
// typedef std::function<bool(string,string)> StrFunc;
// union FuncTypes {
//   MyFunc no_arg;
//   StrFunc str_func;

//   FuncType(MyFunc func) : no_arg(func) {}

// };
typedef map<string, PostFunc> MyMap;


// bool bfoo() {
//   return true;
// }

// bool inrole(const string& actor, const string& role) {
//   return (actor.empty());
//   // return true;
// }

bool key_value(const string& actor, const string& role) {
  return true;
}


bool subject(const Post& post, const string& actor, const string& ignore) {
  (void)ignore;

  bool res = (post.q == actor);
  return res;
}

bool inrole(const Post& post, const string& actor, const string& role) {
  const string *query = NULL;
  if (actor == "p1") {
    query = &(post.p1);
  } else if (actor == "p2") {
    query = &(post.p2);
  }
  bool res = key_value(*query, role);
  return res;
}


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
          std::cout << "++yes" << std::endl;
          prev_node_state = &((*prev_node_state)->yes);
        } else {
          std::cout << "--no" << std::endl;
          prev_node_state = &((*prev_node_state)->no);
        }
        value.clear();
        if (*prev_node_state == NULL) {
          std::cout << "Creating new node!" << std::endl;
          *prev_node_state = new compass::BDDNode();

        } else {
          std::cout << "Already seen this next node" << std::endl;
        }

        // std::cout << "end of set/node" << std::endl;
       continue;
      case '>':
        accept = stoi(value);
        assert(accept == 0 || accept == 1);
        if (accept) {
          std::cout << "++yes" << std::endl;
          prev_node_state = &((*prev_node_state)->yes);
        } else {
          std::cout << "--no" << std::endl;
          prev_node_state = &((*prev_node_state)->no);
        }
        value.clear();
        if (*prev_node_state == NULL) {
          std::cout << "Creating new accept node!" << std::endl;
          *prev_node_state = new compass::BDDNode();
        }

        (*prev_node_state)->accept = true;
        prev_node_state = NULL;
        std::cout << std::endl;
        std::cout << root_->no->no->func() << std::endl;
        std::cout << root_->no->no->yes->yes->yes->yes->accept << std::endl;
        std::cout << std::endl;
        continue;
      case ':':
        // Do something with the value;
        if (!value.empty()) {
          // std::cout << stoi(value) << std::endl;
        }

        meth = bdd_id_to_meth[stoi(value)];
        std::cout << meth << " " << std::endl;
        if (first) {
          if (NULL == root_) {
            new_node = new compass::BDDNode();
            std::cout << new_node->func() << std::endl;
            root_ = new_node;
          }
          first = false;
          prev_node_state = &root_;
        }
        assert(NULL != *prev_node_state);
        std::cout << "COLON " << (*prev_node_state)->func() << std::endl;

        matched = RE2::FullMatch(meth, "(\\w+)\\((\\w+),(\\w+)\\)", &function, &arg0, &arg1);
        if (matched) {
          std::cout << function << " " << arg0 << " " << arg1 << std::endl;
        } else {
          matched = RE2::FullMatch(meth, "(\\w+)\\((\\w+)\\)", &function, &arg0);
          std::cout << function << " " << arg0 << std::endl;
        }

        if (!((*prev_node_state)->func().empty())) {
          assert((*prev_node_state)->func() == meth);
        } else {
          (*prev_node_state)->set_func(meth);
        }

        // if (func_to_node.find(meth) == func_to_node.end()) {
        //   new_node = new compass::BDDNode();
        //   std::cout << "New node " << meth << std::endl;
        //          new_node->set_func(meth);
        //   func_to_node[meth] = new_node;
        // } else {
        //   // std::cout << "Found previously created node." << std::endl;
        //   new_node = func_to_node.find(meth)->second;
        // }
        // if (NULL != prev_node_state) {
        //   // std::cout << "previous node set" << std::endl;
        //   *prev_node_state = new_node;
        // }

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
  MyMap mmap;//  = { {"bfoo", &bfoo},
  //                {"inrole", &inrole}
  // };
  // mmap["bfoo"] = &bfoo;
  mmap["inrole"] = &inrole;
  mmap["subject"] = &subject;

  // std::cout << mmap.at("bfoo")() << std::endl;
  // string temp;

  // std::cout << mmap.at("inrole")("", "hello") << std::endl;
  BDDNode* node = root_->no->no->yes->yes;
  std::cout << root_->func() << std::endl;
  std::cout << node << std::endl;

  return true;
}

namespace {

bool TreeQuery(const Post& post, const BDDNode& node, vector<string>* receivers) {
  BDDNode* next = NULL; // node.func()(post);

  if (next == NULL) {
    return false;
  }

  if (next->accept) {
    return true;
  }

  if (!node.p2.empty()) {
    receivers->push_back(post.p2);
  }

  return TreeQuery(post, *next, receivers);
}

} // namespace

} // namespace compass
