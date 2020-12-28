#pragma once

#include "AnalysisVisitable.h"
#include "Scope.h"

namespace minipy {
struct AnalysisVisitable;
struct Scope;
struct AnalysisVisitor {
  Scope *rootScope = nullptr;
  virtual ~AnalysisVisitor();

  Scope *getScope();
  Scope *pushScope();
  void popScope();
  bool virtual visit(minipy::AnalysisVisitable *vis);
};
} // namespace minipy
