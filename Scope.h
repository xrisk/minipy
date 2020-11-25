#pragma once

#include <map>
#include <string>

typedef std::string Identifier;
struct Type;

typedef std::map<Identifier, Type *> SymTable;

struct Scope {
  SymTable table;
  Scope *parent;

  Scope() : parent(nullptr) {}

  Scope(Scope *parent) : parent(parent) {}

  void insert(Identifier key, Type *t) { table[key] = t; }

  Type *lookup(Identifier key, bool recurse = true) {
    if (table.find(key) != table.end())
      return table[key];
    if (parent && recurse)
      return parent->lookup(key);
    else
      return nullptr;
  }
};
