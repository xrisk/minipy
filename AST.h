#pragma once

#include <iostream>
#include <vector>

using std::cout;

typedef std::string Identifier;

inline std::string mkindent(int count) {
  std::string s;
  for (int i = 0; i < 2 * count; ++i) {
    s += " ";
  }
  return s;
}

struct ASTNode {
  virtual void print(int indent = 0) { cout << "ASTNode" << '\n'; }
  virtual ~ASTNode() {}
};

enum BaseType { Int8, Int32, Void };

std::ostream &operator<<(std::ostream &o, BaseType &t) {
  switch (t) {
  case BaseType::Int8:
    o << "Int8";
    return o;
  case BaseType::Int32:
    o << "Int32";
    return o;
  case BaseType::Void:
    o << "Void";
    return o;
  }
}

struct Expr;

struct Type : ASTNode {
  BaseType base;
  std::vector<Expr *> dims;

  void print(int indent = 0) override {
    cout << mkindent(indent) << "Type {base = " << base << "}\n";
  }
};

struct Expr : ASTNode {
  std::string *val;

  void print(int indent = 0) override {
    cout << mkindent(indent) << "Expr {val = " << *val << "}\n";
  }
};

struct BinOp : Expr {};

struct Literal : Expr {};

struct Statement : ASTNode {};

struct FnDecl : ASTNode {
  Identifier name;
  Type *returntype;
  std::vector<Statement *> body;

  void print(int indent = 0) {
    cout << mkindent(indent) << "FnDecl" << '\n';
    cout << mkindent(indent + 1) << "name: " << name << '\n';
    cout << mkindent(indent + 1) << "returntype:\n";
    returntype->print(indent + 2);
    for (auto stmt : body) {
      stmt->print(indent + 1);
    }
  }
};

struct Declaration : Statement {
  Identifier *name;
  Type *datatype;
  Expr *rval;

  void print(int indent = 0) override {
    std::cout << mkindent(indent) << "Declaration\n";
    std::cout << mkindent(indent + 1) << "name: " << *name << '\n';
    std::cout << mkindent(indent + 1) << "datatype:\n";
    datatype->print(indent + 2);
    std::cout << mkindent(indent + 1) << "rval:\n";
    rval->print(indent + 2);
  }
};

struct ProgNode : ASTNode {
  std::vector<FnDecl *> body;

  void print(int indent = 0) {
    cout << mkindent(indent);
    cout << "ProgNode" << '\n';
    for (auto fn : body) {
      fn->print(indent + 1);
    }
  }
};

struct AST {
  ASTNode *root;

  void print(int indent = 0) { root->print(); }
};
