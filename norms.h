#pragma once

#include <iostream>
#include <vector>
#include <string>

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;

enum SType {
  kNORM,
  kNEXPRESSION,
  kNSTATEMENT,
  kNIDENTIFIER,
  kNMETHODCALL,
  kNBINARYOPERATOR,
  kNBLOCK,
  kNNEGEXPRESSION,
  kNEXPRESSIONSTATEMENT,
  kNVARIABLEDECLARATION,
  kNFUNCTIONDECLARATION,
};

class Norm {
 public:
  virtual ~Norm() {}
  virtual SType stype() const { return stype_; }

 private:
  SType stype_;
};

class NExpression : public Norm {
 public:
  virtual SType stype() const { return stype_; }
 private:
  SType stype_;
};

class NStatement : public Norm {
 public:
  virtual SType stype() const { return stype_; }
 private:
  SType stype_;
};

class NIdentifier : public NExpression {
 public:
  std::string name;
  SType stype_;
  NIdentifier(const std::string& name) : name(name), stype_(kNIDENTIFIER) {}
  virtual SType stype() const { return stype_; }
};

class NMethodCall : public NExpression {
 public:
  const NIdentifier& id;
  ExpressionList arguments;
  SType stype_;

  NMethodCall(const NIdentifier& id, ExpressionList& arguments) :
      id(id), arguments(arguments), stype_(kNMETHODCALL) {}
  NMethodCall(const NIdentifier& id) : id(id), stype_(kNMETHODCALL) {}
  virtual SType stype() const { return stype_; }
};

class NBinaryOperator : public NExpression {
 public:
  NExpression& lhs;
  int op;
  NExpression& rhs;
  SType stype_;

  NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
      lhs(lhs), op(op), rhs(rhs), stype_(kNBINARYOPERATOR) {}
  virtual SType stype() const { return stype_; }
};

class NBlock : public NExpression {
 public:
  StatementList statements;
  SType stype_;

  NBlock() : stype_(kNBLOCK) {}
  virtual SType stype() const { return stype_; }
};

class NNegExpression : public NExpression {
 public:
  NExpression& exp;
  bool negated;
  SType stype_;

  NNegExpression(NExpression& exp, bool negated) :
      exp(exp), negated(negated), stype_(kNNEGEXPRESSION) {}
  virtual SType stype() const { return stype_; }
};

class NExpressionStatement : public NStatement {
public:
  NExpression& expression;
  SType stype_;

  NExpressionStatement(NExpression& expression) :
      expression(expression),
      stype_(kNEXPRESSIONSTATEMENT) {}

  virtual SType stype() const { return stype_; }
};

class NVariableDeclaration : public NStatement {
 public:
  const NIdentifier& type;
  NIdentifier& id;
  NExpression *assignmentExpr;
  SType stype_;

  NVariableDeclaration(const NIdentifier& type, NIdentifier& id) :
      type(type), id(id), stype_(kNVARIABLEDECLARATION) {}

  NVariableDeclaration(const NIdentifier& type, NIdentifier& id,
                       NExpression *assignmentExpr) :
      type(type), id(id), assignmentExpr(assignmentExpr),
      stype_(kNVARIABLEDECLARATION) {}
  virtual SType stype() const { return stype_; }
};

class NFunctionDeclaration : public NStatement {
 public:
  const NIdentifier& type;
  const NIdentifier& id;
  VariableList arguments;
  NBlock& block;
  SType stype_;

  NFunctionDeclaration(const NIdentifier& type, const NIdentifier& id,
                       const VariableList& arguments, NBlock& block) :
      type(type), id(id), arguments(arguments), block(block),
      stype_(kNFUNCTIONDECLARATION) {}
  virtual SType stype() const { return stype_; }
};
