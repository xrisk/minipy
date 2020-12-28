#include <cassert>
#include <iostream>
#include <vector>

#include "AST.h"
#include "AnalysisVisitor.h"
#include "CodegenVisitor.h"

using std::cout;
using namespace minipy;

typedef std::string Identifier;

inline std::string mkindent(int count) {
  std::string s;
  for (int i = 0; i < 2 * count; ++i) {
    s += " ";
  }
  return s;
}

void ASTNode::print(int indent) { cout << "ASTNode" << '\n'; }
ASTNode::~ASTNode() {}

bool ASTNode::accept(AnalysisVisitor *vis) { return true; }
void *ASTNode::accept(CodegenVisitor *vis) { return nullptr; }

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
  case BaseType::Bool:
    o << "Bool";
    return o;
  case BaseType::Char:
    o << "Char";
    return o;
  case BaseType::Fn:
    o << "Fn";
    return o;
  }
}

bool Type::operator==(Type other) const {
  if (base != other.base)
    return false;
  assert((((void)"type has both array and fn kind",
           !(dims.size() > 0 && fnTypes.size() > 0))));
  assert(((((void)"type has both array and fn kind",
            !(other.dims.size() > 0 && other.fnTypes.size() > 0)))));
  return dims.size() == other.dims.size() &&
         fnTypes.size() == other.fnTypes.size();
}

bool Type::operator!=(Type other) const { return !(*this == other); }

Type::Type(BaseType base) : base(base) {}

void Type::print(int indent) {
  cout << mkindent(indent) << "Type:\n";
  cout << mkindent(indent + 1) << "base:\n";
  cout << mkindent(indent + 2) << base << '\n';
  cout << mkindent(indent + 1) << "dims:\n";
  for (auto expr : dims) {
    expr->print(indent + 1);
  }
}

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

int minipy::getOperatorArity(minipy::Op o) {
  if (o == FNCALL)
    return -1;
  else if (o == NOT)
    return 1;
  else
    return 2;
}

void OperatorExpr::print(int indent) {
  std::cout << mkindent(indent) << "OperatorExpr:\n";
  std::cout << mkindent(indent + 1) << "op:\n";
  std::cout << mkindent(indent + 2) << op << '\n';
  std::cout << mkindent(indent + 1) << "args(" << args.size() << ")"
            << ":\n";
  for (auto expr : args) {
    expr->print(indent + 2);
  }
}

void IdentifierExpr::print(int indent) {
  std::cout << mkindent(indent) << "IdentifierExpr:\n";
  std::cout << mkindent(indent + 1) << id << '\n';

  if (idx.size() > 0) {
    cout << mkindent(indent + 1) << "idx:\n";
    for (auto expr : idx) {
      expr->print(indent + 2);
    }
  }
}

void IntLiteral::print(int indent) {
  std::cout << mkindent(indent) << "IntLiteral:\n";
  std::cout << mkindent(indent + 1) << value << '\n';
}

void BoolLiteral::print(int indent) {
  cout << mkindent(indent) << "BoolLiteral:\n";
  cout << mkindent(indent + 1) << (value ? "True" : "false") << '\n';
}

void ArrayLiteral::print(int indent) {
  cout << mkindent(indent) << "ArrayLiteral:\n";
  for (auto expr : vals) {
    expr->print(indent + 1);
  }
}

void CharLiteral::print(int indent) {
  cout << mkindent(indent) << "CharLiteral:\n";
  cout << mkindent(indent + 1) << "value:\n";
  cout << mkindent(indent + 2) << value << '\n';
}

void FnDecl::print(int indent) {
  cout << mkindent(indent) << "FnDecl" << '\n';
  cout << mkindent(indent + 1) << "name: " << name << '\n';
  cout << mkindent(indent + 1) << "returntype:\n";
  returntype->print(indent + 2);
  cout << mkindent(indent + 1) << "args:\n";
  for (auto arg : args) {
    cout << mkindent(indent + 2) << "type:\n";
    arg.first->print(indent + 3);
    cout << mkindent(indent + 2) << "name:\n";
    cout << mkindent(indent + 3) << arg.second << '\n';
  }
  cout << mkindent(indent + 1) << "body:\n";
  for (auto stmt : body) {
    stmt->print(indent + 2);
  }
}

void Declaration::print(int indent) {
  std::cout << mkindent(indent) << "Declaration\n";
  std::cout << mkindent(indent + 1) << "name: " << name << '\n';
  std::cout << mkindent(indent + 1) << "datatype:\n";
  datatype->print(indent + 2);
  if (rval != nullptr) {
    std::cout << mkindent(indent + 1) << "rval:\n";
    rval->print(indent + 2);
  }
}

void If::print(int indent) {
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

void ExprStatement::print(int indent) {
  cout << mkindent(indent) << "ExprStatement:\n";
  cout << mkindent(indent + 1) << "expr:\n";
  expr->print(indent + 2);
}

void While::print(int indent) {
  cout << mkindent(indent) << "While:\n";
  cout << mkindent(indent + 1) << "cond:\n";
  cond->print(indent + 2);
  cout << mkindent(indent + 1) << "body:\n";
  for (auto stmt : body) {
    stmt->print(indent + 2);
  }
}

void Return::print(int indent) {
  cout << mkindent(indent) << "Return:\n";
  cout << mkindent(indent + 1) << "expr:\n";
  expr->print(indent + 2);
}

void ForNode::print(int indent) {
  cout << mkindent(indent) << "ForNode:\n";
  cout << mkindent(indent + 1) << "init:\n";
  init->print(indent + 2);
  cout << mkindent(indent + 1) << "cond:\n";
  cond->print(indent + 2);
  cout << mkindent(indent + 1) << "update:\n";
  update->print(indent + 2);
}

void ProgNode::print(int indent) {
  cout << mkindent(indent);
  cout << "ProgNode" << '\n';
  for (auto fn : body) {
    fn->print(indent + 1);
  }
}

void AST::print(int indent) { root->print(); }
