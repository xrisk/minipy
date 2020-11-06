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

struct Expr : ASTNode {};

struct Type : ASTNode {
  BaseType base;
  std::vector<Expr *> dims;

  void print(int indent = 0) override {
    cout << mkindent(indent) << "Type:\n";
    cout << mkindent(indent + 1) << "base:\n";
    cout << mkindent(indent + 2) << base << '\n';
    cout << mkindent(indent + 1) << "dims:\n";
    for (auto expr : dims) {
      expr->print(indent + 1);
    }
  }
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

std::ostream &operator<<(std::ostream &o, Op &op) {
  switch (op) {
  case AND:
    o << "and";
    return o;
  case OR:
    o << "or";
    return o;
  case NOT:
    o << "!";
    return o;
  case EXP:
    o << "**";
    return o;
  case ADD:
    o << "+";
    return o;
  case SUB:
    o << "-";
    return o;
  case MUL:
    o << "*";
    return o;
  case DIV:
    o << "/";
    return o;
  case MODULO:
    o << "%";
    return o;
  case FNCALL:
    o << "fncall";
    return o;
  case EQ:
    o << "==";
    return o;
  case LT:
    o << "<";
    return o;
  case GT:
    o << ">";
    return o;
  case LEQ:
    o << "<=";
    return o;
  case GEQ:
    o << ">=";
    return o;
  case NEQ:
    o << "!=";
    return o;
  case ASSIGN:
    o << "=";
    return o;
  }
}

struct OperatorExpr : Expr {
  Op op;
  std::vector<Expr *> args;

  void print(int indent = 0) override {
    std::cout << mkindent(indent) << "OperatorExpr:\n";
    std::cout << mkindent(indent + 1) << "op:\n";
    std::cout << mkindent(indent + 2) << op << '\n';
    std::cout << mkindent(indent + 1) << "args(" << args.size() << ")"
              << ":\n";
    for (auto expr : args) {
      expr->print(indent + 2);
    }
  }
};

struct IdentifierExpr : Expr {
  Identifier id;
  void print(int indent = 0) override {
    std::cout << mkindent(indent) << "IdentifierExpr:\n";
    std::cout << mkindent(indent + 1) << id << '\n';
  }
};

struct LiteralExpr : Expr {};

struct IntLiteral : LiteralExpr {
  int value;

  void print(int indent = 0) override {
    std::cout << mkindent(indent) << "IntLiteral:\n";
    std::cout << mkindent(indent + 1) << value << '\n';
  }
};

struct BoolLiteral : LiteralExpr {
  bool value;
};

struct ArrayLiteral : LiteralExpr {
  std::vector<Expr *> vals;

  void print(int indent = 0) {
    cout << mkindent(indent) << "ArrayLiteral:\n";
    for (auto expr : vals) {
      expr->print(indent + 1);
    }
  }
};

struct CharLiteral : LiteralExpr {
  char value;
  void print(int indent = 0) {
    cout << mkindent(indent) << "CharLiteral:\n";
    cout << mkindent(indent + 1) << "value:\n";
    cout << mkindent(indent + 2) << value << '\n';
  }
};

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
    cout << mkindent(indent + 1) << "body:\n";
    for (auto stmt : body) {
      stmt->print(indent + 2);
    }
  }
};

struct Declaration : Statement {
  Identifier *name;
  Type *datatype;
  Expr *rval = nullptr;

  void print(int indent = 0) override {
    std::cout << mkindent(indent) << "Declaration\n";
    std::cout << mkindent(indent + 1) << "name: " << *name << '\n';
    std::cout << mkindent(indent + 1) << "datatype:\n";
    datatype->print(indent + 2);
    if (rval != nullptr) {
      std::cout << mkindent(indent + 1) << "rval:\n";
      rval->print(indent + 2);
    }
  }
};

struct If : Statement {
  Expr *condition;
  std::vector<Statement *> body;

  std::vector<std::pair<Expr *, std::vector<Statement *>>> elifs;

  std::vector<Statement *> else_;

  void print(int indent = 0) override {
    std::cout << mkindent(indent) << "If:\n";
    std::cout << mkindent(indent + 1) << "condition:\n";
    condition->print(indent + 2);
    std::cout << mkindent(indent + 1) << "body:\n";
    for (auto stmt : body) {
      stmt->print(indent + 2);
    }
    if (elifs.size() > 0) {
      std::cout << mkindent(indent + 1) << "elifs:\n";
      for (auto &p : elifs) {
        std::cout << mkindent(indent + 2) << "cond:\n";
        p.first->print(indent + 3);
        std::cout << mkindent(indent + 2) << "body:\n";
        for (auto stmt : p.second) {
          stmt->print(indent + 3);
        }
      }
    }
    if (else_.size() != 0) {
      std::cout << mkindent(indent + 1) << "else:\n";
      for (auto stmt : else_) {
        stmt->print(indent + 3);
      }
    }
  }
};

struct ExprStatement : Statement {
  Expr *expr;

  void print(int indent = 0) {
    cout << mkindent(indent) << "ExprStatement:\n";
    cout << mkindent(indent + 1) << "expr:\n";
    expr->print(indent + 2);
  }
};

struct While : Statement {
  Expr *cond;
  std::vector<Statement *> body;

  void print(int indent = 0) {
    cout << mkindent(indent) << "While:\n";
    cout << mkindent(indent + 1) << "cond:\n";
    cond->print(indent + 2);
    cout << mkindent(indent + 1) << "body:\n";
    for (auto stmt : body) {
      stmt->print(indent + 2);
    }
  }
};

struct Return : Statement {
  Expr *expr;

  void print(int indent = 0) {
    cout << mkindent(indent) << "Return:\n";
    cout << mkindent(indent + 1) << "expr:\n";
    expr->print(indent + 2);
  }
};

struct ForNode : Statement {
  Statement *init = nullptr;
  Expr *cond = nullptr;
  Statement *update = nullptr;

  void print(int indent = 0) {
    cout << mkindent(indent) << "ForNode:\n";
    cout << mkindent(indent + 1) << "init:\n";
    init->print(indent + 2);
    cout << mkindent(indent + 1) << "cond:\n";
    cond->print(indent + 2);
    cout << mkindent(indent + 1) << "update:\n";
    update->print(indent + 2);
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
