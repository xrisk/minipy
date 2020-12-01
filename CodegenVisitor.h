#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "Scope.h"

struct ASTNode;

struct CodegenVisitor;

struct CodegenVisitable {
  virtual void *accept(CodegenVisitor *vis) { return nullptr; }
  virtual ~CodegenVisitable() {}
};

struct CodegenVisitor {

  std::unique_ptr<llvm::LLVMContext> Context;
  std::unique_ptr<llvm::Module> TheModule;
  std::unique_ptr<llvm::IRBuilder<>> Builder;
  std::map<std::string, llvm::AllocaInst *> NamedValues;

  CodegenVisitor() {

    Context = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("my cool jit", *Context);

    // Create a new builder for the module.
    Builder = std::make_unique<llvm::IRBuilder<>>(*Context);
  }
  virtual void *visit(CodegenVisitable *vis) { return vis->accept(this); }
  virtual ~CodegenVisitor(){};

  llvm::AllocaInst *CreateEntryBlockAllocation(llvm::Function *function,
                                               std::string *v, Type *t);
};
