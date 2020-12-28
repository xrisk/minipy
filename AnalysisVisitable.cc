#include "AnalysisVisitable.h"

using namespace minipy;

bool AnalysisVisitable::accept(AnalysisVisitor *vis) { return true; }

AnalysisVisitable::~AnalysisVisitable() {}
