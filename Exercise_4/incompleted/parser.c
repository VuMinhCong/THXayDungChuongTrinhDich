/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"

Token *currentToken;
Token *lookAhead;

void scan(void) {
  Token* tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

void eat(TokenType tokenType) {
  if (lookAhead->tokenType == tokenType) {
    printToken(lookAhead);
    scan();
  } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void) {
  assert("Parsing a Program ....");
  eat(KW_PROGRAM);
  eat(TK_IDENT);
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_PERIOD);
  assert("Program parsed!");
}

void compileBlock(void) {
  assert("Parsing a Block ....");
  if (lookAhead->tokenType == KW_CONST) {
    eat(KW_CONST);
    compileConstDecl();
    compileConstDecls();
    compileBlock2();
  } 
  else compileBlock2();
  assert("Block parsed!");
}

void compileBlock2(void) {
  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);
    compileTypeDecl();
    compileTypeDecls();
    compileBlock3();
  } 
  else compileBlock3();
}

void compileBlock3(void) {
  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    compileVarDecl();
    compileVarDecls();
    compileBlock4();
  } 
  else compileBlock4();
}

void compileBlock4(void) {
  compileSubDecls();
  compileBlock5();
}

void compileBlock5(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileConstDecls(void) {            //Compile const declarations
  if (lookAhead->tokenType == TK_IDENT) {
    compileConstDecl();
    compileConstDecls();
  }
}

void compileConstDecl(void) {             //Compile const declaration
  eat(TK_IDENT);
  eat(SB_EQ);
  compileConstant();
  eat(SB_SEMICOLON);
}

void compileTypeDecls(void) {             //Compile type declarations
  if (lookAhead->tokenType == TK_IDENT) {
    compileTypeDecl();
    compileTypeDecls();
  }
}

void compileTypeDecl(void) {              //Compile type declaration
  eat(TK_IDENT);
  eat(SB_EQ);
  compileType();
  eat(SB_SEMICOLON);
}

void compileVarDecls(void) {              //Compile type declarations
  if (lookAhead->tokenType == TK_IDENT) {
    compileVarDecl();
    compileVarDecls();
  }
}

void compileVarDecl(void) {               //Compile type declaration
  eat(TK_IDENT);
  eat(SB_COLON);
  compileType();
  eat(SB_SEMICOLON);
}

void compileSubDecls(void) {
  assert("Parsing subtoutines ....");
  if (lookAhead->tokenType == KW_FUNCTION) {
    compileFuncDecl();
  }
  else if (lookAhead->tokenType == KW_PROCEDURE) {
    compileProcDecl();
  }
  assert("Subtoutines parsed ....");
}

void compileFuncDecl(void) {
  assert("Parsing a function ....");
  eat(KW_FUNCTION);
  eat(TK_IDENT);
  compileParams();
  eat(SB_COLON);
  compileBasicType();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  compileSubDecls();
  assert("Function parsed ....");
}

void compileProcDecl(void) {
  assert("Parsing a procedure ....");
  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  compileParams();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  compileSubDecls();
  assert("Procedure parsed ....");
}

//============================================================================================

void compileUnsignedConstant(void) {
  switch (lookAhead->tokenType)
  {
    case TK_NUMBER:
      eat(TK_NUMBER);
      break;
    
    case TK_CHAR:
      eat(TK_CHAR);
      break;
    
    case TK_IDENT:
      eat(TK_IDENT);
      break;

    case TK_STRING:
      eat(TK_STRING);
      break;

    default:
      break;
  }
}

void compileConstant(void) {                  // Const ::= SB_PLUS Constant2
  if (lookAhead->tokenType == SB_PLUS) {
    eat(SB_PLUS);
    compileConstant2();
  }
  else if (lookAhead->tokenType == SB_MINUS) {
    eat(SB_MINUS);
    compileConstant2();
  }
  else if (lookAhead->tokenType == TK_CHAR) {
    eat(TK_CHAR);
  }
  else {
    compileConstant2();
  }
}

void compileConstant2(void) {                 // Constant2 ::= ConstIdent || Number
  if (lookAhead->tokenType == TK_IDENT) eat(TK_IDENT);
  else if (lookAhead->tokenType == TK_NUMBER) eat(TK_NUMBER);
  else (error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo));
}

void compileType(void) {                      // Type ::= TypeIdent || TypeArray || BasicType
  if (lookAhead->tokenType == TK_IDENT) eat(TK_IDENT);
  else if (lookAhead->tokenType == KW_ARRAY) {
    eat(KW_ARRAY);
    eat(SB_LSEL);
    eat(TK_NUMBER);
    eat(SB_RSEL);
    eat(KW_OF);
    compileBasicType();
  }
  else compileBasicType();
}

void compileBasicType(void) {                 // BasicType ::= INTEGER || CHAR
  if (lookAhead->tokenType == KW_CHAR) eat(KW_CHAR);
  else if (lookAhead->tokenType == KW_INTEGER) eat(KW_INTEGER);
  else if (lookAhead->tokenType == KW_STRING) eat(KW_STRING);
  else eat(TK_IDENT);
}

void compileParams(void) {
  if (lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileParam();
    compileParams2();
    eat(SB_RPAR);
  }
}

void compileParams2(void) {
  if (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileParam();
    compileParams2();
  }
}

void compileParam(void) {
  if (lookAhead->tokenType == TK_IDENT) {
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
  }
  else if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
  }
}

//=========================================================================================

void compileStatements(void) {
  compileStatement();
  compileStatements2();
}

void compileStatements2(void) {
  if (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileStatement();
    compileStatements2();
  }
}

void compileStatement(void) {                       //Trường hợp | statement ::= <epsinol> ---> Khi đọc được : ';', 'END', 'ELSE'
  switch (lookAhead->tokenType) {
    case TK_IDENT:                                  // Done
      compileAssignSt();
      break;
    case KW_CALL:                                   // Done
      compileCallSt();
      break;
    case KW_BEGIN:                                  // Done
      compileGroupSt();
      break;
    case KW_IF:                                     // Done
      compileIfSt();
      break;
    case KW_WHILE:                                  // Done
      compileWhileSt();
      break;
    case KW_FOR:                                    // Done
      compileForSt();
      break;
    case KW_DO:
      compileDoWhileSt();
      break;
      // EmptySt needs to check FOLLOW tokens
    case SB_SEMICOLON:
    case KW_END:
    case KW_ELSE:
      break;
      // Error occurs
    default:
      error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
      break;
  }
}

void compileAssignSt(void) {
  assert("Parsing an assign statement ....");
  eat(TK_IDENT);
  if (lookAhead->tokenType == SB_LSEL) compileIndexes();
  eat(SB_ASSIGN);
  compileExpression();
  assert("Assign statement parsed ....");
}

void compileCallSt(void) {
  assert("Parsing a call statement ....");
  eat(KW_CALL);
  eat(TK_IDENT);
  compileArguments();
  assert("Call statement parsed ....");
}

void compileGroupSt(void) {
  assert("Parsing a group statement ....");
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
  assert("Group statement parsed ....");
}

void compileIfSt(void) {
  assert("Parsing an if statement ....");
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE) 
    compileElseSt();
  assert("If statement parsed ....");
}

void compileElseSt(void) {
  eat(KW_ELSE);
  compileStatement();
}

void compileWhileSt(void) {
  assert("Parsing a while statement ....");
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
  assert("While statement pased ....");
}

void compileForSt(void) {
  assert("Parsing a for statement ....");
  eat(KW_FOR);
  eat(TK_IDENT);
  eat(SB_ASSIGN);
  compileExpression();
  eat(KW_TO);
  compileExpression();
  eat(KW_DO);
  compileStatement();
  assert("For statement parsed ....");
}

void compileDoWhileSt() {
  assert("Parsing a do-while statement ....");
  eat(KW_DO);
  compileStatement();
  eat(KW_WHILE);
  compileCondition();
  assert("Do-while statement parsed ....");
}

void compileArguments(void) {
  if (lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileExpression();
    compileArguments2();
    eat(SB_RPAR);
  }
}

void compileArguments2(void) {
  if (lookAhead->tokenType == SB_COMMA) {
    eat(SB_COMMA);
    compileExpression();
    compileArguments2();
  }
}

void compileCondition(void) {
  compileExpression();
  compileCondition2();
}

void compileCondition2(void) {
  switch (lookAhead->tokenType) {
    case SB_EQ:
      eat(SB_EQ);
      compileExpression();
      break;

    case SB_NEQ:
      eat(SB_NEQ);
      compileExpression();
      break;

    case SB_LE:
      eat(SB_LE);
      compileExpression();
      break;

    case SB_LT:
      eat(SB_LT);
      compileExpression();
      break;

    case SB_GE:
      eat(SB_GE);
      compileExpression();
      break;

    case SB_GT:
      eat(SB_GT);
      compileExpression();
      break;

    default:
      break;
  }
}

void compileExpression(void) {
  assert("Parsing an expression");
  if (lookAhead->tokenType == SB_PLUS) {
    eat(SB_PLUS);
    compileExpression2();
  }
  else if (lookAhead->tokenType == SB_MINUS) {
    eat(SB_MINUS);
    compileExpression2();
  }
  else compileExpression2();
  assert("Expression parsed");
}

void compileExpression2(void) {
  compileTerm();
  compileExpression3();
}


void compileExpression3(void) {
  if (lookAhead->tokenType == SB_PLUS) {
    eat(SB_PLUS);
    compileTerm();
    compileExpression3();
  }
  else if (lookAhead->tokenType == SB_MINUS) {
    eat(SB_MINUS);
    compileTerm();
    compileExpression3();
  }
}

void compileTerm(void) {
  compileFactor();
  compileTerm2();
}

void compileTerm2(void) {
  switch (lookAhead->tokenType) {
    case SB_TIMES :
      eat(SB_TIMES);
      compileFactor();
      compileTerm2();
      break;

    case SB_SLASH :
      eat(SB_SLASH);
      compileFactor();
      compileTerm2();
      break;

    case SB_MODULO :
      eat(SB_MODULO);
      compileFactor();
      compileTerm2();
      break;

    default:
      break;
  }
}

void compileFactor(void) {
  switch (lookAhead->tokenType)
  {
    case SB_LPAR :
      eat(SB_LPAR);
      compileExpression();
      eat(SB_RPAR);
      break;

    case TK_IDENT :
      eat(TK_IDENT);
      if (lookAhead->tokenType == SB_LSEL) compileIndexes();          //Chỉ nên cho chạy 1 trong 2
      else compileArguments();                                        //----
      break;
  
    default:
      compileUnsignedConstant();
      break;
  }
}

void compileIndexes(void) {
  if (lookAhead->tokenType == SB_LSEL) {
    eat(SB_LSEL);
    compileExpression();
    eat(SB_RSEL);
    compileIndexes();
  }
}

int compile(char *fileName) {
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  compileProgram();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;

}
