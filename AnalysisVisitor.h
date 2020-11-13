#pragma once

#include <map>

struct Scope;
struct ASTNode;

struct AnalysisVisitor;

struct AnalysisVisitable {
  virtual bool accept(AnalysisVisitor *vis) { return true; }
  virtual ~AnalysisVisitable() {}
};

struct AnalysisVisitor {

  std::map<ASTNode *, Scope *> scopes;

  Scope *getScope(ASTNode *node);

  virtual ~AnalysisVisitor() {}

  bool virtual visit(AnalysisVisitable *vis) { return vis->accept(this); }
};
