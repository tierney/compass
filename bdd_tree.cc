#include "bdd_tree.h"

#include <string.h>
#include <cassert>
#include <sstream>
#include <string>
#include <map>
#include <typeinfo>
#include <functional>

#include <re2/re2.h>

using std::map;
using std::string;

namespace compass {

bool key_value(const string& actor, const string& role, leveldb::DB *db) {
  leveldb::Status s = db->Get(leveldb::ReadOptions(), "whatever", NULL);
  // assert(s.ok());
  return (rand() % 2 == 1);
}


bool subject(const Post& post, const BDDNode& node, leveldb::DB *db) {
  bool res = (post.q == node.arg0);
  return (rand() % 2 == 1 ? res : !res);
}

bool inrole(const Post& post, const BDDNode& node, leveldb::DB *db) {
  const string *query = NULL;
  if (node.arg0 == "p1") {
    query = &(post.p1);
  } else if (node.arg0 == "p2") {
    query = &(post.p2);
  }
  bool res = key_value(*query, node.arg1, db);
  return res;
}

bool attr(const Post& post, const BDDNode& node, leveldb::DB *db) {
  bool res = key_value(post.msg, node.arg1, db);
  return res;
}

BDDTree::BDDTree() : root_(NULL) {
  name_func_["inrole"] = &inrole;
  name_func_["subject"] = &subject;
  name_func_["attr"] = &attr;

  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db_);
  assert(status.ok());
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
        // std::cout << std::endl;
        // std::cout << root_->no->no->func() << std::endl;
        // std::cout << root_->no->no->yes->yes->yes->yes->accept << std::endl;
        // std::cout << std::endl;
        continue;
      case ':':
        // Do something with the value;
        if (!value.empty()) {
          // std::cout << stoi(value) << std::endl;
        }

        meth = bdd_id_to_meth[stoi(value)];
        // std::cout << meth << " " << std::endl;
        if (first) {
          if (NULL == root_) {
            new_node = new compass::BDDNode();
            // std::cout << new_node->func() << std::endl;
            root_ = new_node;
          }
          first = false;
          prev_node_state = &root_;
        }
        assert(NULL != *prev_node_state);
        // std::cout << "COLON " << (*prev_node_state)->func() << std::endl;
        arg0.clear();
        arg1.clear();
        matched = RE2::FullMatch(meth, "(\\w+)\\((\\w+),(\\w+)\\)", &function, &arg0, &arg1);
        if (matched) {
          // std::cout << function << " " << arg0 << " " << arg1 << std::endl;
        } else {
          matched = RE2::FullMatch(meth, "(\\w+)\\((\\w+)\\)", &function, &arg0);
          // std::cout << function << " " << arg0 << std::endl;
        }

        if (!((*prev_node_state)->func().empty())) {
          assert((*prev_node_state)->func() == function);
        } else {
          (*prev_node_state)->set_func(function);
          (*prev_node_state)->arg0 = arg0;
          if (!arg1.empty()) {
            (*prev_node_state)->arg1 = arg1;
          }
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


bool BDDTree::Query(const Post& post, vector<string>* receivers) {
  // BDDNode* node = root_->no->no->yes->yes->yes->yes;
  // std::cout << root_->func() << std::endl;
  // std::cout << node->accept << std::endl;

  return TreeQuery(post, *root_, receivers);
}

bool BDDTree::TreeQuery(const Post& post, const BDDNode& node, vector<string>* receivers) {
  BDDNode* next = NULL;
  // std::cout << "Func: " << node.func() << " " << node.arg0 << " " << node.arg1 << std::endl;
  bool accept = name_func_.at(node.func())(post, node, db_); // node.func()(post);
  next = (accept) ? node.yes : node.no;
  // std::cout << (accept ? " yes" : " no") << std::endl;

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

} // namespace compass
