#pragma once

#include <vector>

#include "Scope.h"

struct ASTNode;

struct AnalysisVisitor;

struct AnalysisVisitable {
  virtual bool accept(AnalysisVisitor *vis) { return true; }
  virtual ~AnalysisVisitable() {}
};

struct AnalysisVisitor {

  Scope *rootScope = nullptr;

  virtual ~AnalysisVisitor() {}

  Scope *getScope() { return rootScope; }

  Scope *pushScope() {
    Scope *n = new Scope(rootScope);
    rootScope = n;
    return n;
  }

  void popScope() {
    Scope *popped = rootScope;
    rootScope = rootScope->parent;
    delete popped;
  }

  bool virtual visit(AnalysisVisitable *vis) { return vis->accept(this); }
};
