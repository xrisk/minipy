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
    if (ctx->expr()) {
      ExprStatement *node = new ExprStatement();

      ASTNode *temp = ctx->expr()->accept(this);
      node->expr = dynamic_cast<Expr *>(temp);
      return (ASTNode *)node;
    }
    if (ctx->return_stmt()) {
      Return *node = new Return();
      ASTNode *temp = ctx->return_stmt()->expr()->accept(this);
      node->expr = dynamic_cast<Expr *>(temp);
      return (ASTNode *)node;
    }
    assert(false && "unreachable code - simpleStatement");
  }

  Any visitStmt(MiniCParser::StmtContext *ctx) override {
    if (ctx->if_stmt()) {
      return (ASTNode *)ctx->if_stmt()->accept(this);
    }
    if (ctx->for_stmt()) {
      return (ASTNode *)ctx->for_stmt()->accept(this);
    }
    if (ctx->while_stmt()) {
      return (ASTNode *)ctx->while_stmt()->accept(this);
    }
    // simple statement
    return (ASTNode *)ctx->simple_stmt()->accept(this);
  }

  /* Any visitIf_stmt(MiniCParser::If_stmtContext *ctx) override {} */

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

    if (ctx->expr()) {
      ASTNode *t = ctx->expr()->accept(this);
      node->rval = dynamic_cast<Expr *>(t);
    }
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
      assert(false && "unknown datatype");
    if (node->base == BaseType::Void && ctx->expr().size() > 0)
      assert(false && "void type should not have dimensions");
    for (auto expr : ctx->expr()) {
      ASTNode *n = expr->accept(this);
      node->dims.push_back(dynamic_cast<Expr *>(n));
    }
    return (ASTNode *)node;
  }

  // TODO: refactor
  Any visitExpr(MiniCParser::ExprContext *ctx) override {

    if (ctx->LPAREN())
      return ctx->expr(0)->accept(this);

    if (ctx->loc()) {
      IdentifierExpr *node = new IdentifierExpr();
      node->id = ctx->loc()->IDENT()->getText();

      for (auto e : ctx->loc()->expr()) {
        ASTNode *temp = e->accept(this);
        node->idx.push_back(dynamic_cast<Expr *>(temp));
      }

      return (ASTNode *)node;
    }

    if (ctx->unary() || ctx->binop_exp() || ctx->binop1() || ctx->binop2() ||
        ctx->binop3() || ctx->binop4() || ctx->assign_expr()) {

      OperatorExpr *node = new OperatorExpr();
      if (ctx->unary()) {
        node->op = Op::NOT;
      } else if (ctx->binop_exp()) {
        node->op = Op::EXP;
      } else if (ctx->binop1()) {
        auto val = ctx->binop1();
        if (val->DIV()) {
          node->op = Op::DIV;
        } else if (val->MULT()) {
          node->op = Op::MUL;
        } else if (val->MODULO()) {
          node->op = Op::MODULO;
        } else
          assert(false && "unreachable code");
      } else if (ctx->binop2()) {
        auto val = ctx->binop2();
        if (val->ADD()) {
          node->op = Op::ADD;
        } else if (val->SUB()) {
          node->op = Op::SUB;
        } else
          assert(false && "unreachable code");
      } else if (ctx->binop3()) {
        auto val = ctx->binop3();
        if (val->LT())
          node->op = Op::LT;
        else if (val->GT())
          node->op = Op::GT;
        else if (val->LEQ())
          node->op = Op::LEQ;
        else if (val->GEQ())
          node->op = Op::GEQ;
        else if (val->NEQ())
          node->op = Op::NEQ;
        else if (val->EQ())
          node->op = Op::EQ;
        else
          assert(false && "unreachable code");
      } else if (ctx->binop4()) {
        auto val = ctx->binop4();
        if (val->AND())
          node->op = Op::AND;
        else if (val->OR())
          node->op = Op::OR;
        else
          assert(false && "unreachable code");
      } else if (ctx->assign_expr()) {
        node->op = Op::ASSIGN;
        IdentifierExpr *ident = new IdentifierExpr();
        ident->id = ctx->assign_expr()->loc()->IDENT()->getText();

        for (auto expr : ctx->assign_expr()->loc()->expr()) {
          ASTNode *temp = expr->accept(this);
          ident->idx.push_back(static_cast<Expr *>(temp));
        }

        node->args.push_back((Expr *)ident);

        ASTNode *temp = ctx->assign_expr()->expr()->accept(this);
        node->args.push_back(dynamic_cast<Expr *>(temp));
        return (ASTNode *)node;
      }

      for (auto e : ctx->expr()) {
        ASTNode *t = e->accept(this);
        Expr *arg = dynamic_cast<Expr *>(t);
        node->args.push_back(arg);
      }
      return (ASTNode *)node;
    }

    if (ctx->literal()) {
      return (ASTNode *)ctx->literal()->accept(this);
    }

    if (ctx->fn_call()) {
      return (ASTNode *)ctx->fn_call()->accept(this);
    }

    std::cout << ctx->getText() << '\n';
    assert(false && "unreachable code");
  }

  Any visitLiteral(MiniCParser::LiteralContext *val) override {
    if (val->INT_LITERAL()) {
      IntLiteral *n = new IntLiteral();
      n->value = atoi(val->INT_LITERAL()->getText().c_str());
      return (ASTNode *)n;
    }

    if (val->bool_literal()) {
      BoolLiteral *n = new BoolLiteral();
      if (val->bool_literal()->getText() == "True")
        n->value = true;
      else
        n->value = false;
      return (ASTNode *)n;
    }

    if (val->array_literal()) {
      ArrayLiteral *node = new ArrayLiteral();
      for (auto expr : val->array_literal()->expr()) {
        ASTNode *temp = expr->accept(this);
        node->vals.push_back(dynamic_cast<Expr *>(temp));
      }
      return (ASTNode *)node;
    }

    if (val->char_literal()) {
      CharLiteral *node = new CharLiteral();
      node->value = val->char_literal()->getText()[1];
      return (ASTNode *)node;
    }

    assert(false && "unreachable code");
  }

  Any visitFn_call(MiniCParser::Fn_callContext *ctx) override {
    OperatorExpr *node = new OperatorExpr();
    node->op = Op::FNCALL;
    IdentifierExpr *funcName = new IdentifierExpr();
    funcName->id = ctx->IDENT()->getText();
    node->args.push_back(funcName);
    for (auto exp : ctx->expr()) {
      ASTNode *temp = exp->accept(this);
      node->args.push_back(dynamic_cast<Expr *>(temp));
    }
    return (ASTNode *)node;
  }

  Any visitIf_stmt(MiniCParser::If_stmtContext *ctx) override {
    If *node = new If();
    ASTNode *temp = ctx->expr()->accept(this);
    node->condition = dynamic_cast<Expr *>(temp);
    for (auto stmt : ctx->block()->stmt()) {
      temp = stmt->accept(this);
      node->body.push_back(dynamic_cast<Statement *>(temp));
    }

    if (ctx->elif_block().size() > 0) {
      for (auto elif : ctx->elif_block()) {
        temp = elif->expr()->accept(this);
        Expr *temp2 = dynamic_cast<Expr *>(temp);
        std::vector<Statement *> temp3;
        for (auto stmt : elif->block()->stmt()) {
          temp = stmt->accept(this);
          temp3.push_back(dynamic_cast<Statement *>(temp));
        }
        node->elifs.push_back(std::make_pair(temp2, temp3));
      }
    }

    if (ctx->else_block()) {
      for (auto stmt : ctx->else_block()->block()->stmt()) {
        ASTNode *temp = stmt->accept(this);
        node->else_.push_back(dynamic_cast<Statement *>(temp));
      }
    }
    return (ASTNode *)node;
  }

  Any visitWhile_stmt(MiniCParser::While_stmtContext *ctx) override {
    While *node = new While();
    ASTNode *temp = ctx->expr()->accept(this);
    node->cond = dynamic_cast<Expr *>(temp);

    for (auto stmt : ctx->block()->stmt()) {
      temp = stmt->accept(this);
      node->body.push_back(dynamic_cast<Statement *>(temp));
    }
    return (ASTNode *)node;
  }

  Any visitFor_stmt(MiniCParser::For_stmtContext *ctx) override {
    ForNode *node = new ForNode();

    if (ctx->for_init()) {
      ASTNode *temp = ctx->for_init()->simple_stmt()->accept(this);
      node->init = dynamic_cast<Statement *>(temp);
    }

    if (ctx->for_cond()) {
      ASTNode *temp = ctx->for_cond()->expr()->accept(this);
      node->cond = dynamic_cast<Expr *>(temp);
    }

    if (ctx->for_update()) {
      ASTNode *temp = ctx->for_update()->simple_stmt()->accept(this);
      node->update = dynamic_cast<Statement *>(temp);
    }

    return (ASTNode *)node;
  }
};
