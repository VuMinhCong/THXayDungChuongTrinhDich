/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  //printf("Skip blank\n");
  while (charCodes[currentChar = readChar()] == CHAR_SPACE && currentChar != EOF);
}

void skipComment() {
  int previousChar = currentChar;
  while (charCodes[previousChar] != CHAR_TIMES || charCodes[currentChar] != CHAR_RPAR) {
    previousChar = currentChar;
    readChar();
    if (currentChar == EOF) {
      error(ERR_ENDOFCOMMENT, lineNo, colNo);
    }
  }
  readChar();
}

Token* readIdentKeyword(void) {
  int line = lineNo, col = colNo;
  char *currIdent = (char*)calloc(MAX_IDENT_LEN + 1, sizeof(char));                           //curIdent là xâu kí tự đang đọc, đến khi gặp kí tự space
  Token *currToken;
  TokenType currTokenType;
  int loct = 0;

  do {
    *(currIdent + loct++) = (char)currentChar;
  } while (charCodes[currentChar = readChar()] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT);
  if ((currTokenType = checkKeyword(currIdent)) != TK_NONE)                                  //Lưu TokenType đọc được vào currTokenType
    currToken = makeToken(currTokenType, line, col);
  else {
    currToken = makeToken(TK_IDENT, line, col);
    strcpy(currToken->string, currIdent);
  }

  //printf("CurrentIdent = %s\n", currIdent);

  return currToken;
}

Token* readNumber(int sign) {
  int line = lineNo, col = colNo;
  char *currNumber = (char*)calloc(MAX_IDENT_LEN + 1, sizeof(char));
  Token *currToken;
  int loct = 0, kt = 1;
  if (sign == 3 || sign == 4) {
    *(currNumber + loct++) = '0';
  }

  do {
    if (charCodes[currentChar] == CHAR_PERIOD) 
      if (kt == 0) error(ERR_INVALIDSYMBOL, lineNo, colNo);
      else kt = 0;
    if (loct >= MAX_IDENT_LEN + 1) error(ERR_IDENTTOOLONG, lineNo, colNo);
    *(currNumber + loct++) = (char)currentChar;
  } while (charCodes[currentChar = readChar()] == CHAR_DIGIT || charCodes[currentChar] == CHAR_PERIOD);
  currToken = makeToken(TK_NUMBER, line, col);
  currToken->value = atof(currNumber);
  if (sign == 1 || sign == 4) {
    currToken->value = -(currToken->value);
  }
  return currToken;
}

Token* readConstChar(void) {
  int line = lineNo, col = colNo;
  Token* currToken;
  readChar();
  if (charCodes[currentChar] == CHAR_UNKNOWN) error(ERR_INVALIDCHARCONSTANT, line, col);
  else {
    currToken = makeToken(TK_CHAR, line, col);
    currToken->value = currentChar;
    readChar();
    if (charCodes[currentChar] != CHAR_SINGLEQUOTE) error(ERR_INVALIDCHARCONSTANT, line, col);
    readChar();
  }
  return currToken;
}

Token* readString(void) {
  readChar();
  int line = lineNo, col = colNo, loct = 0;
  Token* currToken;
  char *currIdent = (char*)calloc(MAX_IDENT_LEN + 1, sizeof(char));;
  currToken = makeToken(TK_STRING, line, col);
  do {
    *(currIdent + loct++) = (char)currentChar;
  } while (charCodes[currentChar = readChar()] != CHAR_DOUBLEQUOTE && loct < MAX_IDENT_LEN);
  readChar();
  strcpy(currToken->string, currIdent);
  return currToken;
}

Token* getToken(void) {
  Token *token;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
    case CHAR_SPACE: skipBlank(); return getToken();                      //Còn trống thì skip, chú ý vị trí con trỏ đọc file sau khi skip
    case CHAR_LETTER: return readIdentKeyword();
    case CHAR_DIGIT: return readNumber(0);
    case CHAR_PLUS: 
      token = makeToken(SB_PLUS, lineNo, colNo);
      readChar();
      return token;
    case CHAR_LPAR:
      readChar();
      if (charCodes[currentChar] == CHAR_TIMES) skipComment(); else
      if (charCodes[currentChar] == CHAR_PERIOD) {
        token = makeToken(SB_LSEL, lineNo, colNo-1);
        readChar();
        return token;
      }
      else {
        token = makeToken(SB_LPAR, lineNo, colNo-1);
        return token;
      }
      return getToken();
    case CHAR_SINGLEQUOTE:
      return readConstChar();

    case CHAR_SEMICOLON:
      token = makeToken(SB_SEMICOLON, lineNo, colNo);
      readChar();
      return token;

    case CHAR_COLON:
      readChar();
      if (charCodes[currentChar] == CHAR_EQ) {
        token = makeToken(SB_ASSIGN, lineNo, colNo-1);
        readChar();
      }
      else {
        token = makeToken(SB_COLON, lineNo, colNo);
      }
      return token;
    
    case CHAR_RPAR:
      readChar();
      token = makeToken(SB_RPAR, lineNo, colNo);
      return token;
    
    case CHAR_PERIOD:
      readChar();
      if (charCodes[currentChar] == CHAR_RPAR) {
        token = makeToken(SB_RSEL, lineNo, colNo-1);
        readChar();
        return token;
      } else 
      if (charCodes[currentChar] == CHAR_DIGIT) return readNumber(3);
      else token = makeToken(SB_PERIOD, lineNo, colNo);
      return token;
    
    case CHAR_COMMA:
      token = makeToken(SB_COMMA, lineNo, colNo);
      readChar();
      return token;
    
    case CHAR_EQ:
      token = makeToken(SB_EQ, lineNo, colNo);
      readChar();
      return token;
    
    case CHAR_EXCLAIMATION:
      readChar();
      if (charCodes[currentChar] == CHAR_EQ) {
        token = makeToken(SB_NEQ, lineNo, colNo-1);
        readChar();
      } else error(ERR_INVALIDSYMBOL, lineNo, colNo);
      return token;
    
    case CHAR_LT:
      readChar();
      if (charCodes[currentChar] == CHAR_EQ) {
        token = makeToken(SB_LE, lineNo, colNo-1);
        readChar();
      } else token = makeToken(SB_LT, lineNo, colNo);
      return token;
    
    case CHAR_GT:
      readChar();
      if (charCodes[currentChar] == CHAR_EQ) {
        token = makeToken(SB_GE, lineNo, colNo-1);
        readChar();
      } else token = makeToken(SB_GT, lineNo, colNo);
      return token;
    
    case CHAR_MINUS:
      readChar();
      if (charCodes[currentChar] == CHAR_DIGIT) return readNumber(1);
      else if (charCodes[currentChar] == CHAR_PERIOD) return readNumber(4);
      else {
        token = makeToken(SB_MINUS, lineNo, colNo);
      }
      return token;
    
    case CHAR_TIMES:
      token = makeToken(SB_TIMES, lineNo, colNo);
      readChar();
      return token;

    case CHAR_SLASH:
      token = makeToken(SB_SLASH, lineNo, colNo);
      readChar();
      return token;

    case CHAR_DOUBLEQUOTE: return readString();
      
    default:
      token = makeToken(TK_NONE, lineNo, colNo);
      error(ERR_INVALIDSYMBOL, lineNo, colNo);
      readChar();
      return token;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%f)\n", token->value); break;
  case TK_CHAR: printf("TK_CHAR(\'%c\')\n", (char)token->value); break;
  case TK_EOF: printf("TK_EOF\n"); break;
  case TK_STRING: printf("TK_STRING(\"%s\")\n", token->string); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
  return 0;
}