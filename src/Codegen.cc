#include "AST.h"
#include "CodegenVisitor.h"

#include <iostream>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <set>

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

using namespace minipy;

llvm::AllocaInst *
CodegenVisitor::CreateEntryBlockAllocation(llvm::Function *function,
                                           std::string *v, minipy::Type *t) {
  std::string varName = *v;
  llvm::IRBuilder<> temp(&function->getEntryBlock(),
                         function->getEntryBlock().begin());
  llvm::AllocaInst *allocate_instruction = nullptr;
  if (t->base == Int32) {
    allocate_instruction =
        temp.CreateAlloca(llvm::Type::getInt32Ty(*this->Context), 0, varName);
  } else if (t->base == Bool) {
    allocate_instruction =
        temp.CreateAlloca(llvm::Type::getInt1Ty(*this->Context), 0, varName);
  }
  return allocate_instruction;
}

void *ProgNode::accept(CodegenVisitor *vis) {
  for (auto fn : this->body) {
    fn->accept(vis);
  }
  return nullptr;
}

llvm::Value *visitBlock(std::vector<Statement *> block, CodegenVisitor *vis) {

  llvm::Value *val;

  for (Statement *stmt : block) {
    val = (llvm::Value *)stmt->accept(vis);

    if (dynamic_cast<Return *>(stmt) != nullptr) {
      return val;
    }
  }

  return nullptr;
}

void *Extern::accept(CodegenVisitor *vis) {
  std::vector<llvm::Type *> argument_types;
  for (auto arg : this->args) {
    if (arg.first->base == Int32) {
      argument_types.push_back(llvm::Type::getInt32Ty(*vis->Context));
    } else if (arg.first->base == Bool) {
      argument_types.push_back(llvm::Type::getInt1Ty(*vis->Context));
    } else {
      assert(false && "unreachable code");
      return nullptr;
    }
  }

  llvm::Type *returnType;
  if (this->returnType->base == Int32) {
    returnType = llvm::Type::getInt32Ty(*vis->Context);
  } else if (this->returnType->base == Bool) {
    returnType = llvm::Type::getInt1Ty(*vis->Context);
  } else if (this->returnType->base == Void) {
    returnType = llvm::Type::getVoidTy(*vis->Context);
  } else {
    assert(false && "unreachable code");
    return nullptr;
  }

  llvm::FunctionType *FT =
      llvm::FunctionType::get(returnType, argument_types, false);
  llvm::Function *F = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, this->name, *vis->TheModule);

  auto arg_it = this->args.begin();
  auto llvm_arg_it = F->arg_begin();
  while (arg_it != this->args.end() and llvm_arg_it != F->arg_end()) {
    auto arg = *arg_it;
    llvm_arg_it->setName(arg.second);
    arg_it++;
    llvm_arg_it++;
  }

  return F;
}

void *FnDecl::accept(CodegenVisitor *vis) {
  std::vector<llvm::Type *> argument_types;
  for (auto arg : this->args) {
    if (arg.first->base == Int32) {
      argument_types.push_back(llvm::Type::getInt32Ty(*vis->Context));
    } else if (arg.first->base == Bool) {
      argument_types.push_back(llvm::Type::getInt1Ty(*vis->Context));
    } else {
      assert(false && "unreachable code");
      return nullptr;
    }
  }

  llvm::Type *returnType;
  if (this->returntype->base == Int32) {
    returnType = llvm::Type::getInt32Ty(*vis->Context);
  } else if (this->returntype->base == Bool) {
    returnType = llvm::Type::getInt1Ty(*vis->Context);
  } else if (this->returntype->base == Void) {
    returnType = llvm::Type::getVoidTy(*vis->Context);
  } else {
    assert(false && "unreachable code");
    return nullptr;
  }

  llvm::FunctionType *FT =
      llvm::FunctionType::get(returnType, argument_types, false);
  llvm::Function *F = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, this->name, *vis->TheModule);

  auto arg_it = this->args.begin();
  auto llvm_arg_it = F->arg_begin();
  while (arg_it != this->args.end() and llvm_arg_it != F->arg_end()) {
    auto arg = *arg_it;
    llvm_arg_it->setName(arg.second);
    arg_it++;
    llvm_arg_it++;
  }

  llvm::BasicBlock *BB = llvm::BasicBlock::Create(*vis->Context, "entry", F);
  vis->Builder->SetInsertPoint(BB);

  auto oldNamedValues = vis->NamedValues;

  arg_it = this->args.begin();
  llvm_arg_it = F->arg_begin();
  while (arg_it != this->args.end() and llvm_arg_it != F->arg_end()) {
    auto my_arg_ptr = *arg_it;
    llvm::AllocaInst *alloca = vis->CreateEntryBlockAllocation(
        F, &my_arg_ptr.second, my_arg_ptr.first);
    vis->Builder->CreateStore(llvm_arg_it, alloca);
    vis->NamedValues[my_arg_ptr.second] = alloca;
    arg_it++;
    llvm_arg_it++;
  }

  llvm::Value *retVal = visitBlock(this->body, vis);

  vis->Builder->CreateRet(retVal);

  llvm::verifyFunction(*F);
  return F;
}

void *IntLiteral::accept(CodegenVisitor *vis) {
  llvm::Value *v = llvm::ConstantInt::get(
      *vis->Context, llvm::APInt(32, static_cast<uint64_t>(this->value), true));
  return v;
}

void *OperatorExpr::accept(CodegenVisitor *vis) {
  std::vector<llvm::Value *> vals;

  int arity = getOperatorArity(this->op);

  if (this->op == FNCALL) {
    std::string fnName = ((IdentifierExpr *)this->args[0])->id;
    auto Callee = vis->TheModule->getFunction(fnName);

    std::vector<llvm::Value *> callArgs;
    for (int idx = 1; idx < this->args.size(); ++idx) {
      auto arg = this->args[idx];
      callArgs.push_back((llvm::Value *)arg->accept(vis));
    }

    auto r = Callee->getReturnType();

    if (r->isVoidTy())
      return vis->Builder->CreateCall(Callee, callArgs);
    else
      return vis->Builder->CreateCall(Callee, callArgs, "calltmp");
  }

  if (arity == 2) {
    llvm::Value *left;
    if (this->op == ASSIGN) {
      left = vis->NamedValues[((IdentifierExpr *)this->args[0])->id];
    } else {
      left = (llvm::Value *)vis->visit(this->args[0]);
    }
    llvm::Value *right = (llvm::Value *)vis->visit(this->args[1]);

    switch (this->op) {
    case ADD:
      return vis->Builder->CreateAdd(left, right, "addtmp");
    case SUB:
      return vis->Builder->CreateSub(left, right, "subtmp");
    case MUL:
      return vis->Builder->CreateMul(left, right, "multmp");
    case DIV:
      return vis->Builder->CreateSDiv(left, right, "divtmp");
    case MODULO:
      return vis->Builder->CreateSRem(left, right, "modtmp");
    case LT:
      return vis->Builder->CreateICmpSLT(left, right, "ltcmptmp");
    case GT:
      return vis->Builder->CreateICmpSGT(left, right, "gtcmptmp");
    case LEQ:
      return vis->Builder->CreateICmpSLE(left, right, "lecmptmp");
    case GEQ:
      return vis->Builder->CreateICmpSGE(left, right, "gecmptmp");
    case EQ:
      return vis->Builder->CreateICmpEQ(left, right, "equalcmptmp");
    case NEQ:
      return vis->Builder->CreateICmpNE(left, right, "notequalcmptmp");
    case AND:
      return vis->Builder->CreateAnd(left, right, "andtmp");
    case OR:
      return vis->Builder->CreateOr(left, right, "ortmp");
    case ASSIGN: {
      llvm::Value *left =
          vis->NamedValues[((IdentifierExpr *)this->args[0])->id];
      vis->Builder->CreateStore(right, left);
      return right;
    }
    default:
      assert(false && "unreachable code");
    }
  }

  return nullptr;
}

void *ExprStatement::accept(CodegenVisitor *vis) {
  return vis->visit(this->expr);
}

void *IdentifierExpr::accept(CodegenVisitor *vis) {
  llvm::Value *V = vis->NamedValues[this->id];

  assert(V != nullptr);
  return vis->Builder->CreateLoad(V, this->id.c_str());
  /* return vis->NamedValues[this->id]; */
}

void *Return::accept(CodegenVisitor *vis) {
  if (expr)
    return vis->visit(this->expr);
  else
    return nullptr;
}

void *Declaration::accept(CodegenVisitor *vis) {
  std::string name = this->name;

  if (*this->datatype == Int32) {
    vis->NamedValues[name] = vis->Builder->CreateAlloca(
        llvm::Type::getInt32Ty(*vis->Context), 0, name);
  } else if (*this->datatype == Bool) {
    vis->NamedValues[name] = vis->Builder->CreateAlloca(
        llvm::Type::getInt1Ty(*vis->Context), 0, name);
  } else {
    assert(false && "not implemented");
  }

  if (this->rval) {
    return vis->Builder->CreateStore((llvm::Value *)this->rval->accept(vis),
                                     vis->NamedValues[name]);
  }
  return vis->NamedValues[name];
}

void *If::accept(CodegenVisitor *vis) {
  auto oldNamedValues = vis->NamedValues;

  llvm::Value *condition = (llvm::Value *)this->condition->accept(vis);

  // Basic blocks for if then else
  llvm::Function *F = vis->Builder->GetInsertBlock()->getParent();
  llvm::BasicBlock *if_block = llvm::BasicBlock::Create(*vis->Context, "if", F);
  llvm::BasicBlock *else_block =
      llvm::BasicBlock::Create(*vis->Context, "else");
  llvm::BasicBlock *next_block =
      llvm::BasicBlock::Create(*vis->Context, "next");
  llvm::BasicBlock *other_block = else_block;

  /* bool ret_if = if_stmt.main_block->has_return(), ret_else = false; */

  // There may not be an else block
  if (this->else_.size() == 0)
    other_block = next_block;

  vis->Builder->CreateCondBr(condition, if_block, other_block);

  // Generate code for the if block
  vis->Builder->SetInsertPoint(if_block);
  llvm::Value *if_val = visitBlock(this->body, vis);

  if (if_val == nullptr)
    vis->Builder->CreateBr(next_block);
  else
    vis->Builder->CreateRet(if_val);

  if_block = vis->Builder->GetInsertBlock();

  if (this->else_.size() != 0) {
    F->getBasicBlockList().push_back(else_block);
    vis->Builder->SetInsertPoint(else_block);

    llvm::Value *else_val = visitBlock(this->else_, vis);

    if (else_val == nullptr)
      vis->Builder->CreateBr(next_block);
    else
      vis->Builder->CreateRet(else_val);
  }

  // Create break for next part of code
  F->getBasicBlockList().push_back(next_block);
  vis->Builder->SetInsertPoint(next_block);
  if (this->body.size() > 0 and this->else_.size() > 0) {
    llvm::Type *ret_type =
        vis->Builder->GetInsertBlock()->getParent()->getReturnType();
    if (ret_type == llvm::Type::getVoidTy(*vis->Context)) {
      vis->Builder->CreateRetVoid();
    } else {
      vis->Builder->CreateRet(
          llvm::ConstantInt::get(*vis->Context, llvm::APInt(32, 0)));
    }
  }
  llvm::Value *v = llvm::ConstantInt::get(*vis->Context, llvm::APInt(32, 0));

  vis->NamedValues = oldNamedValues;

  return v;
}

void *ForNode::accept(CodegenVisitor *vis) {

  llvm::Function *F = vis->Builder->GetInsertBlock()->getParent();

  llvm::BasicBlock *PreBB = llvm::BasicBlock::Create(*vis->Context, "pre", F);
  llvm::BasicBlock *LoopBB = llvm::BasicBlock::Create(*vis->Context, "loop", F);

  llvm::BasicBlock *AfterBB =
      llvm::BasicBlock::Create(*vis->Context, "after", F);

  vis->Builder->CreateBr(PreBB);

  vis->Builder->SetInsertPoint(PreBB);
  this->init->accept(vis);

  llvm::Value *PreCondVar = (llvm::Value *)this->cond->accept(vis);
  vis->Builder->CreateCondBr(PreCondVar, LoopBB, AfterBB);

  vis->Builder->SetInsertPoint(LoopBB);

  llvm::Value *bodyVar = (llvm::Value *)visitBlock(this->body, vis);

  if (bodyVar != nullptr)
    vis->Builder->CreateBr(AfterBB);

  this->update->accept(vis);

  llvm::Value *CondVar = (llvm::Value *)this->cond->accept(vis);

  vis->Builder->CreateCondBr(CondVar, LoopBB, AfterBB);

  vis->Builder->SetInsertPoint(AfterBB);

  return nullptr;
}

void *BoolLiteral::accept(CodegenVisitor *vis) {

  llvm::Value *v =
      llvm::ConstantInt::get(*vis->Context, llvm::APInt(1, this->value, true));
  return v;
}
