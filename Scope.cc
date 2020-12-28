#include "Scope.h"

using namespace minipy;

Scope::Scope() : parent(nullptr) {}
Scope::Scope(Scope *parent) : parent(parent) {}
void Scope::insert(Identifier key, Type *t) { table[key] = t; }
Type *Scope::lookup(Identifier key, bool recurse) {

  if (table.find(key) != table.end())
    return table[key];
  if (parent && recurse)
    return parent->lookup(key);
  else
    return nullptr;
}
