#pragma once

#include <map>

#include "AST.h"
#include "AnalysisVisitor.h"

typedef std::map<Identifier, Type *> SymTable;

struct Scope {
  Scope *outer;

  SymTable table;

  Type *lookup(Identifier key) {
    if (table.find(key) != table.end())
      return table[key];
    if (outer)
      return outer->lookup(key);
    else
      return nullptr;
  }
};

Scope *AnalysisVisitor::getScope(ASTNode *node) {
  if (scopes.find(node) == scopes.end())
    scopes[node] = new Scope();
  return scopes[node];
}

bool ProgNode::accept(AnalysisVisitor *vis) {
  Scope *sc = vis->getScope(this);
  sc->outer = nullptr;

  for (auto fn : this->body) {

    if (sc->lookup(fn->name) != nullptr) {
      std::cout << "function redefinition: " << fn->name << "\n";
      return false;
    }

    vis->getScope(fn)->outer = vis->getScope(this);
    if (!fn->accept(vis)) {
      return false;
    }
    sc->table[fn->name] = new Type();
  }
  return true;
}

bool FnDecl::accept(AnalysisVisitor *vis) {
  Scope *sc = vis->getScope(this);

  for (auto stmt : this->body) {

    vis->scopes[stmt] = sc;

    if (!stmt->accept(vis))
      return false;
  }
  return true;
}

bool Declaration::accept(AnalysisVisitor *vis) {
  Scope *sc = vis->getScope(this);

  if (sc->table.find(*this->name) != sc->table.end()) {
    std::cout << "variable already declared: " << *this->name << '\n';
    return false;
  }
  if (!this->rval->accept(vis))
    return false;
  sc->table[*this->name] = new Type();
  return true;
}
