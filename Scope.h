#pragma once

#include <map>
#include <string>

#include "AST.h"

namespace minipy {

struct Type;

typedef std::string Identifier;

typedef std::map<Identifier, Type *> SymTable;

struct Scope {
  SymTable table;
  Scope *parent;

  Scope();
  Scope(Scope *parent);

  void insert(Identifier key, Type *t);
  Type *lookup(Identifier key, bool recurse = true);
};
}; // namespace minipy
