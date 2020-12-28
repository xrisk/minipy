#include <iostream>
#include <map>

#include "AST.h"
#include "AnalysisVisitor.h"
#include "Scope.h"

using namespace minipy;

typedef std::map<Identifier, Type *> SymTable;

bool ProgNode::accept(AnalysisVisitor *vis) {
  Scope *sc = vis->pushScope();

  for (auto fn : this->body) {
    if (sc->lookup(fn->name) != nullptr) {
      std::cout << "function redefinition: " << fn->name << "\n";
      return false;
    }

    Type *newFn = new Type(BaseType::Fn);
    newFn->fnTypes.push_back(fn->returntype);
    for (auto arg : fn->args) {
      newFn->fnTypes.push_back(arg.first);
    }

    sc->insert(fn->name, newFn);

    if (!fn->accept(vis)) {
      return false;
    }
  }
  return true;
}

bool FnDecl::accept(AnalysisVisitor *vis) {
  Scope *sc = vis->pushScope();

  for (auto arg : this->args) {
    sc->insert(arg.second, arg.first);
  }

  for (auto stmt : this->body) {
    if (!stmt->accept(vis))
      return false;
    Return *r = dynamic_cast<Return *>(stmt);
    if (r != nullptr) {
      if (!(*this->returntype == *r->expr->getType(vis))) {
        std::cerr << "return type invalid\n";
        return false;
      }
    }
  }

  vis->popScope();
  return true;
}

bool Declaration::accept(AnalysisVisitor *vis) {
  Scope *sc = vis->getScope();

  if (sc->lookup(this->name, false)) {
    std::cout << "variable already declared: " << this->name << '\n';
    return false;
  }

  if (this->rval) {

    if (!this->rval->accept(vis))
      return false;

    Type *rhs = this->rval->getType(vis);
    assert(rhs != nullptr);

    if (*this->datatype != *rhs) {
      std::cerr << "incompatible data type for declaration: " << this->name
                << '\n';
      return false;
    }
  }

  sc->insert(this->name, this->datatype);
  return true;
}

Type *IdentifierExpr::getType(AnalysisVisitor *vis) {
  Scope *sc = vis->getScope();
  auto T = sc->lookup(this->id);
  if (this->idx.size() == T->dims.size())
    return new Type(T->base);
  return T;
}

Type *IntLiteral::getType(AnalysisVisitor *vis) {
  return new Type(BaseType::Int32);
}

Type *BoolLiteral::getType(AnalysisVisitor *vis) {
  return new Type(BaseType::Bool);
}

Type *CharLiteral::getType(AnalysisVisitor *vis) {
  return new Type(BaseType::Char);
}

Type *ArrayLiteral::getType(AnalysisVisitor *vis) {
  auto type = this->vals[0]->getType(vis);
  assert("only 1d literals allowed" && type->dims.size() == 0);

  Type *t = new Type(type->base);
  for (auto val : this->vals) {
    t->dims.push_back(val);
  }

  return t;
}

bool OperatorExpr::accept(AnalysisVisitor *vis) {

  if (op == FNCALL) {
    auto fnName = static_cast<IdentifierExpr *>(this->args[0])->id;
    auto fn = vis->getScope()->lookup(fnName);
    if (fn == nullptr) {
      std::cerr << fnName << "is not declared\n";
      return false;
    }
    if (fn == nullptr || fn->base != BaseType::Fn) {
      std::cerr << fnName << " is not a function!\n";
      return false;
    }
    int argcount = this->args.size();

    // XXX: this->args has the fn name at the head
    // // fn->fnTypes has the return type at the head
    if (this->args.size() != fn->fnTypes.size()) {
      std::cout << this->args.size() << ' ' << fn->fnTypes.size() << '\n';
      std::cerr << "incorrect function call arity\n";
      return false;
    }
    // idx 0 holds the function name
    for (int i = 1; i < argcount; ++i) {
      if (!this->args[i]->accept(vis)) {
        return false;
      }
      auto actual = this->args[i]->getType(vis);
      if (actual == nullptr) {
        std::cerr << "unknown type -- probably a missing variable\n";
        return false;
      }
      if (!(*actual == *fn->fnTypes[i])) {
        std::cout << "invalid type passed to function call" << '\n';
        actual->print();
        fn->fnTypes[i]->print();
        return false;
      }
    }
    return true;
  }

  int arity = getOperatorArity(this->op);
  if (this->args.size() != arity) {
    std::cerr << "incorrect operator arity";
    return false;
  }

  Type *expectedType;

  if (this->op == AND || this->op == OR || this->op == NOT)
    expectedType = new Type(BaseType::Bool);
  else {
    if (!this->args[0]->accept(vis))
      return false;
    expectedType = this->args[0]->getType(vis);
  }

  assert(expectedType != nullptr);

  for (int i = 0; i < arity; ++i) {
    if (!this->args[i]->accept(vis)) {
      return false;
    }
    Type *actual = this->args[i]->getType(vis);
    if (!(*expectedType == *actual)) {
      std::cerr << "invalid type passed to operator" << op << '\n';
      expectedType->print();
      actual->print();
      return false;
    }
  }

  return true;
}

Type *OperatorExpr::getType(AnalysisVisitor *vis) {
  if (op == NOT || op == AND | op == OR || op == LT || op == LEQ || op == GT ||
      op == GEQ || op == EQ || op == NEQ)
    return new Type(BaseType::Bool);

  else if (op == FNCALL) {
    auto ident = (IdentifierExpr *)(this->args[0]);
    Type *t = vis->getScope()->lookup(ident->id);
    assert("not a function" && t->base == BaseType::Fn);
    return t->fnTypes[0];
  } else if (op == ASSIGN) {
    auto ident = (IdentifierExpr *)(this->args[0]);
    Type *t = vis->getScope()->lookup(ident->id);
    return t;
  } else
    return new Type(BaseType::Int32);
}

bool ExprStatement::accept(AnalysisVisitor *vis) {
  return this->expr->accept(vis);
}

bool IdentifierExpr::accept(AnalysisVisitor *vis) {
  Scope *sc = vis->getScope();
  auto t = sc->lookup(this->id);
  if (t == nullptr) {
    std::cerr << "unknown variable " << this->id << '\n';
    return false;
  }
  return true;
}

bool While::accept(AnalysisVisitor *vis) {
  vis->pushScope();
  if (!this->cond->accept(vis))
    return false;

  Type *t = this->cond->getType(vis);
  if (t->base != BaseType::Bool || t->dims.size() != 0) {
    std::cerr << "while conditions must evaluate to bools\n";
    return false;
  }

  for (auto stmt : this->body) {
    if (!stmt->accept(vis))
      return false;
  }

  return true;
}

bool Return::accept(AnalysisVisitor *vis) {
  if (this->expr && !this->expr->accept(vis))
    return false;
  return true;
}

bool If::accept(AnalysisVisitor *vis) {

  vis->pushScope();
  if (!condition->accept(vis))
    return false;

  Type *cond = this->condition->getType(vis);
  assert(cond != nullptr);

  if (cond->base != BaseType::Bool || cond->dims.size() != 0) {
    std::cerr << "if condition must evaluate to bool\n";
    return false;
  }

  for (auto stmt : this->body) {
    if (!stmt->accept(vis))
      return false;
  }

  for (auto p : elifs) {
    if (!p.first->accept(vis))
      return false;
    cond = p.first->getType(vis);

    if (cond->base != BaseType::Bool || cond->dims.size() != 0) {
      std::cerr << "if condition must evaluate to bool\n";
      return false;
    }

    for (auto stmt : this->body) {
      if (!stmt->accept(vis))
        return false;
    }
  }

  for (auto stmt : this->else_) {
    if (!stmt->accept(vis))
      return false;
  }

  vis->popScope();

  return true;
}

bool ForNode::accept(AnalysisVisitor *vis) {
  vis->pushScope();
  if (this->init && !this->init->accept(vis))
    return false;
  if (this->cond && !this->cond->accept(vis))
    return false;
  if (this->update && !this->update->accept(vis))
    return false;

  for (auto stmt : this->body) {
    if (!stmt->accept(vis))
      return false;
  }

  return true;
}
