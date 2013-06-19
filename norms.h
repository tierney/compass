#pragma once

#include <iostream>
#include <vector>
#include <string>

class NStatement;
class NExpression;
class NVariableDeclaration;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;

class Norm {
 public:
  virtual ~Norm() {}
};

class NExpression : public Norm {
};

class NStatement : public Norm {
};

class NIdentifier : public NExpression {
 public:
  std::string name;
  NIdentifier(const std::string& name) : name(name) {}
};

class NMethodCall : public NExpression {
 public:
  const NIdentifier& id;
  ExpressionList arguments;
  NMethodCall(const NIdentifier& id, ExpressionList& arguments) :
      id(id), arguments(arguments) {}
  NMethodCall(const NIdentifier& id) : id(id) {}
};

class NBinaryOperator : public NExpression {
 public:
  NExpression& lhs;
  int op;
  NExpression& rhs;
  NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
      lhs(lhs), op(op), rhs(rhs) {}
};

class NBlock : public NExpression {
 public:
  StatementList statements;
  NBlock() {}
};

class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression) :
        expression(expression) {}
};

class NVariableDeclaration : public NStatement {
 public:
  const NIdentifier& type;
  NIdentifier& id;
  NExpression *assignmentExpr;

  NVariableDeclaration(const NIdentifier& type, NIdentifier& id) :
      type(type), id(id) {}

  NVariableDeclaration(const NIdentifier& type, NIdentifier& id,
                       NExpression *assignmentExpr) :
      type(type), id(id), assignmentExpr(assignmentExpr) {}
};

class NFunctionDeclaration : public NStatement {
 public:
  const NIdentifier& type;
  const NIdentifier& id;
  VariableList arguments;
  NBlock& block;
  NFunctionDeclaration(const NIdentifier& type, const NIdentifier& id,
                       const VariableList& arguments, NBlock& block) :
      type(type), id(id), arguments(arguments), block(block) {}
};
