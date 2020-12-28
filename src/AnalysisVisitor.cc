#include <vector>

#include "AST.h"
#include "AnalysisVisitable.h"
#include "AnalysisVisitor.h"
#include "Scope.h"

using namespace minipy;

AnalysisVisitor::~AnalysisVisitor() {}

Scope *AnalysisVisitor::getScope() { return rootScope; }

Scope *AnalysisVisitor::pushScope() {
  Scope *n = new Scope(rootScope);
  rootScope = n;
  return n;
}

void AnalysisVisitor::popScope() {
  Scope *popped = rootScope;
  rootScope = rootScope->parent;
  delete popped;
}

bool AnalysisVisitor::visit(AnalysisVisitable *vis) {
  return vis->accept(this);
}
