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
      p->body.push_back(func->accept(this));
    }

    ast->root = p;

    return ast;
  }

  Any visitSimple_stmt(MiniCParser::Simple_stmtContext *ctx) override {
    if (ctx->decl()) {
      return (Statement *)((Declaration *)((ctx->decl()->accept(this))));
    }
    if (ctx->expr()) {
      ExprStatement *node = new ExprStatement();
      node->expr = ctx->expr()->accept(this);
      return (Statement *)node;
    }
    if (ctx->return_stmt()) {
      Return *node = new Return();
      if (ctx->return_stmt()->expr())
        node->expr = ctx->return_stmt()->expr()->accept(this);
      else
        node->expr = nullptr;
      return (Statement *)node;
    }
    assert(false && "unreachable code - simpleStatement");
  }

  Any visitStmt(MiniCParser::StmtContext *ctx) override {
    if (ctx->if_stmt()) {
      return (Statement *)(If *)ctx->if_stmt()->accept(this);
    }
    if (ctx->for_stmt()) {
      return (Statement *)(ForNode *)ctx->for_stmt()->accept(this);
    }
    if (ctx->while_stmt()) {
      return (Statement *)(While *)ctx->while_stmt()->accept(this);
    }
    // simple statement
    return (Statement *)ctx->simple_stmt()->accept(this);
  }

  /* Any visitIf_stmt(MiniCParser::If_stmtContext *ctx) override {} */

  Any visitFunction_decl(MiniCParser::Function_declContext *ctx) override {

    FnDecl *node = new FnDecl();
    node->name = ctx->IDENT()->getText();

    node->returntype = ctx->return_type()->datatype()->accept(this);

    for (auto arg : ctx->arg()) {
      Type *t = arg->datatype()->accept(this);
      Identifier i = arg->IDENT()->getText();
      node->args.push_back(std::make_pair(t, i));
    }

    for (auto stat : ctx->block()->stmt()) {
      node->body.push_back(stat->accept(this));
    }
    return node;
  }

  Any visitDecl(MiniCParser::DeclContext *ctx) override {
    Declaration *node = new Declaration();
    node->name = new std::string(ctx->IDENT()->getText());

    node->datatype = ctx->datatype()->accept(this);

    if (ctx->expr()) {
      node->rval = ctx->expr()->accept(this);
    }
    return node;
  }

  Any visitDatatype(MiniCParser::DatatypeContext *ctx) override {
    Type *node;
    std::string base = ctx->datatype_base()->getText();
    if (base == "int32")
      node = new Type(Int32);
    else if (base == "int8")
      node = new Type(BaseType::Int8);
    else if (base == "void")
      node = new Type(BaseType::Void);
    else if (base == "bool")
      node = new Type(BaseType::Bool);
    else
      assert(false && "unknown datatype");
    if (node->base == BaseType::Void && ctx->expr().size() > 0)
      assert(false && "void type should not have dimensions");
    for (auto expr : ctx->expr()) {
      node->dims.push_back(expr->accept(this));
    }
    return node;
  }

  // TODO: refactor
  Any visitExpr(MiniCParser::ExprContext *ctx) override {

    if (ctx->LPAREN())
      return ctx->expr(0)->accept(this);

    if (ctx->loc()) {
      IdentifierExpr *node = new IdentifierExpr();
      node->id = ctx->loc()->IDENT()->getText();

      for (auto e : ctx->loc()->expr()) {
        node->idx.push_back(e->accept(this));
      }

      return (Expr *)node;
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
          ident->idx.push_back(expr->accept(this));
        }

        node->args.push_back((Expr *)ident);
        node->args.push_back(ctx->assign_expr()->expr()->accept(this));

        return (Expr *)node;
      }

      for (auto e : ctx->expr()) {
        node->args.push_back(e->accept(this));
      }
      return (Expr *)node;
    }

    if (ctx->literal()) {
      LiteralExpr *temp = ctx->literal()->accept(this);
      return (Expr *)temp;
      /* return (Expr *)ctx->literal()->accept(this); */
    }

    if (ctx->fn_call()) {
      return (Expr *)(OperatorExpr *)ctx->fn_call()->accept(this);
    }

    std::cout << ctx->getText() << '\n';
    assert(false && "unreachable code");
  }

  Any visitLiteral(MiniCParser::LiteralContext *val) override {
    if (val->INT_LITERAL()) {
      IntLiteral *n = new IntLiteral();
      n->value = atoi(val->INT_LITERAL()->getText().c_str());
      return (LiteralExpr *)n;
    }

    if (val->bool_literal()) {
      BoolLiteral *n = new BoolLiteral();
      if (val->bool_literal()->getText() == "True")
        n->value = true;
      else
        n->value = false;
      return (LiteralExpr *)n;
    }

    if (val->array_literal()) {
      ArrayLiteral *node = new ArrayLiteral();
      for (auto expr : val->array_literal()->expr()) {
        node->vals.push_back(expr->accept(this));
      }
      return (LiteralExpr *)node;
    }

    if (val->char_literal()) {
      CharLiteral *node = new CharLiteral();
      node->value = val->char_literal()->getText()[1];
      return (LiteralExpr *)node;
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
      node->args.push_back(exp->accept(this));
    }
    return node;
  }

  Any visitIf_stmt(MiniCParser::If_stmtContext *ctx) override {
    If *node = new If();
    node->condition = ctx->expr()->accept(this);
    for (auto stmt : ctx->block()->stmt()) {
      node->body.push_back(stmt->accept(this));
    }

    if (ctx->elif_block().size() > 0) {
      for (auto elif : ctx->elif_block()) {
        Expr *temp2 = elif->expr()->accept(this);

        std::vector<Statement *> temp3;
        for (auto stmt : elif->block()->stmt()) {
          temp3.push_back(stmt->accept(this));
        }
        node->elifs.push_back(std::make_pair(temp2, temp3));
      }
    }

    if (ctx->else_block()) {
      for (auto stmt : ctx->else_block()->block()->stmt()) {
        node->else_.push_back(stmt->accept(this));
      }
    }
    return node;
  }

  Any visitWhile_stmt(MiniCParser::While_stmtContext *ctx) override {
    While *node = new While();
    node->cond = ctx->expr()->accept(this);

    for (auto stmt : ctx->block()->stmt()) {
      node->body.push_back(stmt->accept(this));
    }
    return node;
  }

  Any visitFor_stmt(MiniCParser::For_stmtContext *ctx) override {
    ForNode *node = new ForNode();

    if (ctx->for_init()) {
      node->init = ctx->for_init()->simple_stmt()->accept(this);
    }

    if (ctx->for_cond()) {
      node->cond = ctx->for_cond()->expr()->accept(this);
    }

    if (ctx->for_update()) {
      node->update = ctx->for_update()->simple_stmt()->accept(this);
    }

    for (auto stmt : ctx->block()->stmt()) {
      node->body.push_back(stmt->accept(this));
    }

    return node;
  }
};
