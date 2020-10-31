
// Generated from MiniC.g4 by ANTLR 4.8

#pragma once

#include "AST.h"
#include "antlr4-runtime.h"
#include "generated/MiniCBaseVisitor.h"
#include <iostream>

using antlrcpp::Any;

class ParseTreeVisitor : public MiniCBaseVisitor {

  AST *ast;

public:
  ParseTreeVisitor() { ast = new AST(); }

  Any visitProg(MiniCParser::ProgContext *ctx) override {
    ProgNode *p = new ProgNode();

    for (auto func : ctx->function_decl()) {
      ASTNode *node = func->accept(this);
      p->body.push_back(dynamic_cast<FnDecl *>(node));
    }

    ast->root = p;

    return ast;
  }

  Any visitSimple_stmt(MiniCParser::Simple_stmtContext *ctx) override {
    if (ctx->decl()) {
      return (ASTNode *)ctx->decl()->accept(this);
    }
    throw std::exception();
  }

  Any visitStmt(MiniCParser::StmtContext *ctx) override {
    if (ctx->if_stmt()) {
      return (ASTNode *)ctx->if_stmt()->accept(this);
    }
    if (ctx->for_stmt()) {
      return (ASTNode *)ctx->while_stmt()->accept(this);
    }
    if (ctx->while_stmt()) {
      return (ASTNode *)ctx->while_stmt()->accept(this);
    }
    // simple statement
    return (ASTNode *)ctx->simple_stmt()->accept(this);
  }

  Any visitFunction_decl(MiniCParser::Function_declContext *ctx) override {

    FnDecl *node = new FnDecl();
    node->name = ctx->IDENT(0)->getText();

    ASTNode *temp = ctx->return_type()->datatype()->accept(this);
    node->returntype = dynamic_cast<Type *>(temp);

    for (auto stat : ctx->block()->stmt()) {
      ASTNode *n = stat->accept(this);
      node->body.push_back(dynamic_cast<Statement *>(n));
    }
    return (ASTNode *)node;
  }

  Any visitDecl(MiniCParser::DeclContext *ctx) override {
    Declaration *node = new Declaration();
    node->name = new std::string(ctx->IDENT()->getText());

    ASTNode *n = ctx->datatype()->accept(this);
    node->datatype = dynamic_cast<Type *>(n);

    n = ctx->expr()->accept(this);
    node->rval = dynamic_cast<Expr *>(n);
    return (ASTNode *)node;
  }

  Any visitDatatype(MiniCParser::DatatypeContext *ctx) override {
    Type *node = new Type();
    std::string base = ctx->datatype_base()->getText();
    if (base == "int32")
      node->base = BaseType::Int32;
    else if (base == "int8")
      node->base = BaseType::Int8;
    else if (base == "void")
      node->base = BaseType::Void;
    else
      throw std::exception();
    if (node->base == BaseType::Void && ctx->expr().size() > 0)
      assert(false && "void type should not have dimensions");
    for (auto expr : ctx->expr()) {
      ASTNode *n = expr->accept(this);
      node->dims.push_back(dynamic_cast<Expr *>(n));
    }
    return (ASTNode *)node;
  }

  Any visitExpr(MiniCParser::ExprContext *ctx) override {
    Expr *node = new Expr();
    node->val = new std::string(ctx->getText());
    return (ASTNode *)node;
  }
};
