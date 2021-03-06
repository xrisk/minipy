/* Copyright (c) 2011-2017 The ANTLR Project. All rights reserved.
 * Use of this file is governed by the BSD 3-clause license that
 * can be found in the LICENSE.txt file in the project root.
 */

//
//  main.cpp
//  antlr4-cpp-demo
//
//  Created by Mike Lischke on 13.03.16.
//

#include <fstream>
#include <iostream>

#include <ANTLRInputStream.h>
#include <antlr4-runtime.h>
#include <llvm/IR/Verifier.h>

#include "generated/MiniCLexer.h"
#include "generated/MiniCParser.h"

#include "AST.h"
#include "AnalysisVisitor.h"
#include "CodegenVisitor.h"
#include "ParseTreeVisitor.h"

using namespace antlr4;
using namespace minipy;

int main(int argc, char **argv) {

  if (argc != 2) {
    std::cout << "Usage: ./minipy <filename>\n";
    return 1;
  }

  std::ifstream file(argv[1]);

  ANTLRInputStream input(file);
  MiniCLexer lexer(&input);
  CommonTokenStream tokens(&lexer);

  tokens.fill();

  MiniCParser parser(&tokens);

  MiniCParser::ProgContext *ctx = parser.prog();

  auto err = parser.getNumberOfSyntaxErrors();

  if (err != 0)
    return 1;

  ParseTreeVisitor vis(argv[1]);
  AST *ast = vis.visitProg(ctx).as<AST *>();

  /* ast->print(); */

  AnalysisVisitor phase2;

  if (!ast->root->accept(&phase2)) {
    std::cout << "Semantically invalid!";
    return 1;
  }

  CodegenVisitor phase3;
  ast->root->accept(&phase3);
  phase3.TheModule->print(llvm::outs(), nullptr);
  bool success = llvm::verifyModule(*phase3.TheModule, &llvm::errs());
  assert("verification failed" && !success);
}
