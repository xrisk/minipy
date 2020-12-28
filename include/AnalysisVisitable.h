#pragma once

namespace minipy {
struct AnalysisVisitor;
struct AnalysisVisitable {
  virtual bool accept(minipy::AnalysisVisitor *vis);
  virtual ~AnalysisVisitable();
};
} // namespace minipy
