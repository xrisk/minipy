#pragma once

#include "AnalysisVisitable.h"
#include "AnalysisVisitor.h"
#include "CodegenVisitor.h"

namespace minipy {

typedef std::string Identifier;

enum BaseType { Int8, Int32, Void, Bool, Char, Fn };

struct ASTNode : AnalysisVisitable, CodegenVisitable {
  virtual void print(int indent = 0);
  virtual ~ASTNode();

  bool accept(AnalysisVisitor *vis) override;
  void *accept(CodegenVisitor *vis) override;
};

struct Type;

struct Expr : ASTNode {
  /* bool accept(AnalysisVisitor *vis) override; */

  virtual Type *getType(AnalysisVisitor *vis) = 0;
};

struct Type : ASTNode {
  BaseType base;
  std::vector<Expr *> dims;
  std::vector<Type *> fnTypes;

  bool operator==(Type other) const;

  bool operator!=(Type other) const;

  Type(BaseType base);

  void print(int indent = 0) override;
};

enum Op {
  NOT,
  EXP,
  ADD,
  SUB,
  MUL,
  DIV,
  MODULO,
  FNCALL,
  LT,
  GT,
  LEQ,
  GEQ,
  NEQ,
  EQ,
  AND,
  OR,
  ASSIGN
};

int getOperatorArity(Op o);

struct OperatorExpr : Expr {
  Op op;
  std::vector<Expr *> args;

  Type *getType(AnalysisVisitor *vis) override;

  bool accept(AnalysisVisitor *vis) override;

  void print(int indent = 0) override;
  void *accept(CodegenVisitor *vis) override;
};

struct IdentifierExpr : Expr {
  Identifier id;
  std::vector<Expr *> idx;

  bool accept(AnalysisVisitor *vis) override;
  void *accept(CodegenVisitor *vis) override;
  Type *getType(AnalysisVisitor *vis) override;
  void print(int indent = 0) override;
};

struct LiteralExpr : Expr {};

struct IntLiteral : LiteralExpr {
  int value;
  Type *getType(AnalysisVisitor *vis) override;
  void print(int indent = 0) override;
  void *accept(CodegenVisitor *vis) override;
};

struct BoolLiteral : LiteralExpr {
  bool value;
  Type *getType(AnalysisVisitor *vis) override;
  void *accept(CodegenVisitor *vis) override;
  void print(int indent = 0) override;
};

struct ArrayLiteral : LiteralExpr {
  std::vector<Expr *> vals;
  Type *getType(AnalysisVisitor *vis) override;
  void print(int indent = 0) override;
};

struct CharLiteral : LiteralExpr {
  char value;
  Type *getType(AnalysisVisitor *vis) override;
  void print(int indent = 0) override;
};

struct Statement : ASTNode {};

struct FnDecl : ASTNode {
  Identifier name;
  Type *returntype;

  std::vector<std::pair<Type *, Identifier>> args;
  std::vector<Statement *> body;

  bool accept(AnalysisVisitor *vis) override;
  void *accept(CodegenVisitor *vis) override;

  void print(int indent = 0) override;
};

struct Declaration : Statement {
  Identifier name;
  Type *datatype;
  Expr *rval = nullptr;

  bool accept(AnalysisVisitor *vis) override;
  void *accept(CodegenVisitor *vis) override;
  void print(int indent = 0) override;
};

struct If : Statement {
  Expr *condition;
  std::vector<Statement *> body;

  std::vector<std::pair<Expr *, std::vector<Statement *>>> elifs;

  std::vector<Statement *> else_;
  bool accept(AnalysisVisitor *vis) override;
  void *accept(CodegenVisitor *vis) override;
  void print(int indent = 0) override;
};

struct ExprStatement : Statement {
  Expr *expr;

  bool accept(AnalysisVisitor *vis) override;
  void print(int indent = 0) override;
  void *accept(CodegenVisitor *vis) override;
};

struct While : Statement {

  Expr *cond;
  std::vector<Statement *> body;

  bool accept(AnalysisVisitor *vis) override;
  void print(int indent = 0) override;
};

struct Return : Statement {
  Expr *expr;

  bool accept(AnalysisVisitor *vis) override;
  void *accept(CodegenVisitor *vis) override;

  void print(int indent = 0) override;
};

struct ForNode : Statement {
  Statement *init = nullptr;
  Expr *cond = nullptr;
  Statement *update = nullptr;

  std::vector<Statement *> body;

  bool accept(AnalysisVisitor *vis) override;
  void *accept(CodegenVisitor *vis) override;
  void print(int indent = 0) override;
};

struct ProgNode : ASTNode {
  std::vector<FnDecl *> body;

  bool accept(AnalysisVisitor *vis) override;
  void *accept(CodegenVisitor *vis) override;
  void print(int indent = 0) override;
};

struct AST {
  ASTNode *root;
  void print(int indent = 0);
};
} // namespace minipy
