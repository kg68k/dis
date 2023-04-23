// ソースコードジェネレータ
// テーブル行構文解析・評価
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

// This file is part of dis (source code generator).
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program. If not, see <https://www.gnu.org/licenses/>.

#include "eval.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "disasm.h"
#include "estruct.h"
#include "etc.h"
#include "fpconv.h"
#include "generate.h"
#include "hex.h"
#include "label.h"
#include "labelfile.h"
#include "offset.h"
#include "table.h"

typedef enum {
  TOKEN_ERROR = -1,
  TOKEN_NUL = 0,

  TOKEN_NUMBER,
  TOKEN_STRING,

  TOKEN_SIZEID,
  TOKEN_FSIZEID,
  TOKEN_BREAK,
  TOKEN_PEEK,
  TOKEN_EXT,

  TOKEN_EQUAL,
  TOKEN_NOT_EQUAL,
  TOKEN_LESS_OR_EQUAL,
  TOKEN_GREATER_OR_EQUAL,
  TOKEN_LOGICAL_OR,
  TOKEN_LOGICAL_AND,

  // 1文字の演算子や括弧などは文字コードをそのまま使う
  TOKEN_LESS = '<',
  TOKEN_GREATER = '>',
} TokenType;

typedef struct {
  TokenType type;
  ULONG value;
} Token;

typedef struct {
  char* s;
  size_t length;
} StrSpan;

typedef struct {
  char* _p;
  const char* _error;
  const char* _idstr;
  boolean _hasToken;
  StrSpan _strSpan;
  Token _current;
  ULONG* _constValues;  // {pc, tabletop}
} Tokenizer;

typedef enum {
  EXPR_ULONG,
  EXPR_LABEL,
  EXPR_STRING,
} ExprType;

typedef struct {
  ExprType type;
  union {
    ULONG ul;
    StrSpan strSpan;
  } value;
} Expr;

static Expr createExprUlong(ULONG ul) { return (Expr){EXPR_ULONG, {.ul = ul}}; }
static Expr createExprLabel(ULONG ul) { return (Expr){EXPR_LABEL, {.ul = ul}}; }
static Expr createExprString(StrSpan strSpan) {
  return (Expr){EXPR_STRING, {.strSpan = strSpan}};
}

typedef struct {
  Tokenizer tokenizer;
  ParseTblResult* result;
  const char* error;
  char* writePtr;
  char* bufferEnd;
  address pc;
} Parser;

static ParseResult parseExprParenExpr(Parser* parser, Expr* result);
static ParseResult parseExpr(Parser* parser, Expr* result);
static ParseResult parseCloseBracket(Parser* parser, TokenType close);
static ParseResult parseSyntaxError(Parser* parser, const char* message);
static ParseResult parseTokenError(Parser* parser);
static ParseResult parseRuntimeError(Parser* parser, const char* message);

static ParseResult parseSuccess(Parser* parser, opesize id, int count,
                                int bytes, ULONG value) {
  ParseTblResult* result = parser->result;

  result->pr = PARSE_SUCCESS;
  result->id = id;
  result->count = count;
  result->bytes = bytes;
  result->value = value;
  return PARSE_SUCCESS;
}

static Tokenizer createTokenizer(char* p, ULONG* constValues);
static Token tokenError(Tokenizer* tokenizer, const char* message);
static const char* getTokenizerError(Tokenizer* tokenizer);
static Token getToken(Tokenizer* tokenizer);
static void consumeToken(Tokenizer* tokenizer);

static StrSpan getStrSpan(Tokenizer* tokenizer) { return tokenizer->_strSpan; }
static const char* getIdStr(Tokenizer* tokenizer) { return tokenizer->_idstr; }

// 指定したアドレスに対応する読み込んだファイル上の位置を返す
//   アドレスがBBSを指していれば0埋めされたバッファを返す
static codeptr secured(address ptr) {
  static const union {
    SingleReal sr;
    DoubleReal dr;
    ExtendedReal xr;
    PackedDecimal pd;
  } zero;

  return (Dis.beginBSS <= ptr) ? (codeptr)&zero : (codeptr)(ptr + Dis.Ofst);
}

static void stringifyLong(char* p, address pc) {
  ULONG ul = PEEK_LONG(pc);
  if (depend_address(pc))
    make_proper_symbol(p, (address)ul);
  else
    itox8d(p, ul);
}

static void stringifyDataAtPc(Parser* parser, opesize size) {
  address pc = parser->pc;
  char* p = parser->writePtr;

  switch (size) {
    default:
      *p = '\0';
      break;

    case BYTESIZE:
      itox2d(p, PEEK_BYTE(pc));
      break;
    case WORDSIZE:
      itox4d(p, PEEK_WORD(pc));
      break;
    case LONGSIZE:
      stringifyLong(p, pc);
      break;

    case SINGLESIZE:
      fpconv_s(p, secured(pc));
      break;
    case DOUBLESIZE:
      fpconv_d(p, secured(pc));
      break;
    case EXTENDSIZE:
      fpconv_x(p, secured(pc));
      break;
    case PACKEDSIZE:
      fpconv_p(p, secured(pc));
      break;
  }
}

static ParseResult stringTooLong(Parser* parser) {
  const char* message = "オペランド表現式で生成した文字列が長すぎます。";
  return parseRuntimeError(parser, message);
}

static ParseResult stringifyExprString(Parser* parser, Expr* expr) {
  char* p = parser->writePtr;
  size_t rest = parser->bufferEnd - p - 1;

  switch (expr->type) {
    default:
      *p = '\0';
      break;

    case EXPR_ULONG:
      if (rest < 10) return stringTooLong(parser);  // strlen("$xxxxxxxx")

      // 技巧的なオペランド表現式の場合、式の値が識別子のサイズより大きくなる
      // こともありえるので、dc.b、dc.wでもitox2d()、itox4d()にはしない
      p = itox8d(p, expr->value.ul);
      break;

    case EXPR_LABEL:
      if (rest < 32) return stringTooLong(parser);  // 厳密な安全確認ではない
      p = make_proper_symbol(p, expr->value.ul);
      break;

    case EXPR_STRING: {
      size_t len = expr->value.strSpan.length;
      if (rest < len) return stringTooLong(parser);

      memcpy(p, expr->value.strSpan.s, len);
      p += len;
      *p = '\0';
      break;
    }
  }
  parser->writePtr = p;
  return PARSE_SUCCESS;
}

static ParseResult parseStrOrExp(Parser* parser, Expr* result) {
  ParseResult pr;
  Token token = getToken(&parser->tokenizer);
  if (token.type == TOKEN_ERROR) return parseTokenError(parser);

  if (token.type == TOKEN_STRING) {
    *result = createExprString(getStrSpan(&parser->tokenizer));
    consumeToken(&parser->tokenizer);
    return PARSE_SUCCESS;
  }

  if (token.type == '{') {
    Expr expr;
    consumeToken(&parser->tokenizer);
    pr = parseExpr(parser, &expr);
    if (pr == PARSE_SUCCESS) pr = parseCloseBracket(parser, '}');

    if (pr == PARSE_SUCCESS) {
      if (expr.type != EXPR_ULONG)
        internalError(__FILE__, __LINE__, "type != EXPR_ULONG.");

      if (parser->writePtr == NULL) {
        // 解析中ならラベル登録
        // テーブルでないと判定された場合に取り消す処理を追加したほうがよいかも
        regist_label((address)expr.value.ul, DATLABEL | UNKNOWN);
      }
      *result = createExprLabel(expr.value.ul);
    }
    return pr;
  }

  pr = parseExpr(parser, result);
  if (pr == PARSE_SUCCESS) {
    if (result->type != EXPR_ULONG)
      internalError(__FILE__, __LINE__, "type != EXPR_ULONG.");
  }
  return pr;
}

static ParseResult parseExprString(Parser* parser) {
  while (1) {
    TokenType type;
    Expr expr;
    ParseResult pr = parseStrOrExp(parser, &expr);
    if (pr != PARSE_SUCCESS) return pr;

    if (parser->writePtr != NULL) {
      pr = stringifyExprString(parser, &expr);
      if (pr != PARSE_SUCCESS) return pr;
    }

    type = getToken(&parser->tokenizer).type;
    if (type == TOKEN_ERROR) return parseTokenError(parser);
    if (type == TOKEN_NUL) return PARSE_SUCCESS;
    if (type == ',') {
      consumeToken(&parser->tokenizer);
      continue;
    }
    return parseSyntaxError(parser,
                            "オペランド表現式が\",\"で区切られていません。");
  }
}

static void peek_warning(address adrs, int size) {
  if (isOdd(adrs)) {
    eprintf("警告: peek.%cの引数が奇数アドレス(" PRI_ADRS ")です。\n", size,
            adrs);
  }
}

static ULONG peek(address adrs, opesize size) {
  switch (size) {
    default:
      break;

    case BYTESIZE:
      return PEEK_BYTE(adrs);
    case WORDSIZE:
      peek_warning(adrs, 'w');
      return PEEK_WORD(adrs);
    case LONGSIZE:
      peek_warning(adrs, 'l');
      return PEEK_LONG(adrs);
  }

  return 0;
}

static ULONG evalFunc(Token token, ULONG ul) {
  opesize size = token.value;

  if (token.type == TOKEN_PEEK) {
    return peek((address)ul, size);
  }

  // TOKEN_EXT
  return (size == WORDSIZE) ? extw(ul) : extl(ul);
}

static ParseResult parseExprElement(Parser* parser, Expr* result) {
  Token token = getToken(&parser->tokenizer);
  TokenType type = token.type;
  if (type == TOKEN_ERROR) return parseTokenError(parser);
  if (type == TOKEN_NUL) return parseSyntaxError(parser, "式がありません。");
  if (type == TOKEN_STRING)
    return parseSyntaxError(parser, "式に文字列は使えません。");

  if (type == TOKEN_NUMBER) {
    consumeToken(&parser->tokenizer);
    *result = createExprUlong(token.value);
    return PARSE_SUCCESS;
  }

  if (type == '(') {
    // ここではconsumeToken()は呼ばない
    return parseExprParenExpr(parser, result);
  }

  if (type == TOKEN_PEEK || type == TOKEN_EXT) {
    ParseResult pr;

    consumeToken(&parser->tokenizer);
    pr = parseExprParenExpr(parser, result);
    if (pr == PARSE_SUCCESS) {
      result->value.ul = evalFunc(token, result->value.ul);
    }
    return pr;
  }

  return parseSyntaxError(parser, "式として解釈できません。");
}

static ParseResult parseExprParenExpr(Parser* parser, Expr* result) {
  ParseResult pr;
  TokenType type = getToken(&parser->tokenizer).type;
  if (type == TOKEN_ERROR) return parseTokenError(parser);

  // 関数呼び出しではない"(式)"の場合は呼び出し元で"("を確認しているので
  // ここでエラーになることはない
  if (type != '(')
    return parseSyntaxError(parser, "関数名の直後に\"(\"がありません。");
  consumeToken(&parser->tokenizer);

  pr = parseExpr(parser, result);
  if (pr == PARSE_SUCCESS) pr = parseCloseBracket(parser, ')');
  if (pr != PARSE_SUCCESS) return pr;

  return PARSE_SUCCESS;
}

static ULONG neg(ULONG ul) {
  LONG l = (LONG)ul;
  return (ULONG)-l;
}

static ParseResult parseExprFactor(Parser* parser, Expr* result) {
  ParseResult pr;
  TokenType type = getToken(&parser->tokenizer).type;

  if (type == '-' || type == '!') {
    consumeToken(&parser->tokenizer);

    pr = parseExprFactor(parser, result);
    if (pr == PARSE_SUCCESS)
      result->value.ul =
          (type == '-') ? neg(result->value.ul) : !result->value.ul;
    return pr;
  }

  return parseExprElement(parser, result);
}

static ParseResult parseExprMul(Parser* parser, Expr* result) {
  ParseResult pr = parseExprFactor(parser, result);
  if (pr != PARSE_SUCCESS) return pr;

  while (1) {
    Expr expr2;
    TokenType type = getToken(&parser->tokenizer).type;

    if (type != '*' && type != '/' && type != '%') return PARSE_SUCCESS;
    consumeToken(&parser->tokenizer);

    pr = parseExprFactor(parser, &expr2);
    if (pr != PARSE_SUCCESS) return pr;

    if (type == '*') {
      result->value.ul = (result->value.ul * expr2.value.ul);
    } else {
      ULONG divisor = expr2.value.ul;
      if (divisor == 0) return parseRuntimeError(parser, "0で除算しました。");

      result->value.ul = (type == '/') ? (result->value.ul / divisor)
                                       : (result->value.ul % divisor);
    }
  }
}

static ParseResult parseExprAdditive(Parser* parser, Expr* result) {
  ParseResult pr = parseExprMul(parser, result);
  if (pr != PARSE_SUCCESS) return pr;

  while (1) {
    Expr expr2;
    TokenType type = getToken(&parser->tokenizer).type;

    if (type != '+' && type != '-') return PARSE_SUCCESS;
    consumeToken(&parser->tokenizer);

    pr = parseExprMul(parser, &expr2);
    if (pr != PARSE_SUCCESS) return pr;

    result->value.ul = (type == '+') ? (result->value.ul + expr2.value.ul)
                                     : (result->value.ul - expr2.value.ul);
  }
}

static ParseResult parseExprRelational(Parser* parser, Expr* result) {
  ParseResult pr = parseExprAdditive(parser, result);
  if (pr != PARSE_SUCCESS) return pr;

  while (1) {
    Expr expr2;
    TokenType type = getToken(&parser->tokenizer).type;

    if (type != TOKEN_LESS_OR_EQUAL && type != '<' &&
        type != TOKEN_GREATER_OR_EQUAL && type != '>')
      return PARSE_SUCCESS;
    consumeToken(&parser->tokenizer);

    pr = parseExprAdditive(parser, &expr2);
    if (pr != PARSE_SUCCESS) return pr;

    switch (type) {
      default:
        break;

      case TOKEN_LESS_OR_EQUAL:
        result->value.ul = (result->value.ul <= expr2.value.ul);
        break;
      case '<':
        result->value.ul = (result->value.ul < expr2.value.ul);
        break;
      case TOKEN_GREATER_OR_EQUAL:
        result->value.ul = (result->value.ul >= expr2.value.ul);
        break;
      case '>':
        result->value.ul = (result->value.ul > expr2.value.ul);
        break;
    }
  }
}

static ParseResult parseExprEquality(Parser* parser, Expr* result) {
  ParseResult pr = parseExprRelational(parser, result);
  if (pr != PARSE_SUCCESS) return pr;

  while (1) {
    Expr expr2;
    TokenType type = getToken(&parser->tokenizer).type;

    if (type != TOKEN_EQUAL && type != TOKEN_NOT_EQUAL) return PARSE_SUCCESS;
    consumeToken(&parser->tokenizer);

    pr = parseExprRelational(parser, &expr2);
    if (pr != PARSE_SUCCESS) return pr;

    result->value.ul = (type == TOKEN_EQUAL)
                           ? (result->value.ul == expr2.value.ul)
                           : (result->value.ul != expr2.value.ul);
  }
}

static ParseResult parseExpr(Parser* parser, Expr* result) {
  ParseResult pr = parseExprEquality(parser, result);
  if (pr != PARSE_SUCCESS) return pr;

  while (1) {
    Expr expr2;
    TokenType type = getToken(&parser->tokenizer).type;

    if (type != TOKEN_LOGICAL_OR && type != TOKEN_LOGICAL_AND)
      return PARSE_SUCCESS;
    consumeToken(&parser->tokenizer);

    pr = parseExprEquality(parser, &expr2);
    if (pr != PARSE_SUCCESS) return pr;

    result->value.ul = (type == TOKEN_LOGICAL_OR)
                           ? (result->value.ul || expr2.value.ul)
                           : (result->value.ul && expr2.value.ul);
  }
}

// ASCIIZ 形式データのサイズ(文字列+"\0"のバイト数)を調べる
static int getAsciizLength(Parser* parser) {
  address pc = parser->pc;  // BSSの可能性あり(途中からの場合も)
  UBYTE c;

  do {
    c = PEEK_BYTE(pc);
    pc += 1;
  } while (c);

  return (int)(pc - parser->pc);
}

static int opesizeToBytes(opesize size) {
  static const uint8_t bytes[] = {
      1,   // BYTESIZE
      2,   // WORDSIZE
      4,   // LONGSIZE
      8,   // QUADSIZE,
      0,   // SHORTSIZE
      4,   // SINGLESIZE
      8,   // DOUBLESIZE
      12,  // EXTENDSIZE
      12,  // PACKEDSIZE
  };
  return (size < _countof(bytes)) ? bytes[size] : 0;
}

static ParseResult evaluateSizeid(Parser* parser, opesize size, int count) {
  int bytes = 0;
  ULONG value = 0;

  switch (size) {
    default:
      break;

    case BYTESIZE:
    case WORDSIZE:
    case LONGSIZE:
    case SINGLESIZE:
    case DOUBLESIZE:
    case EXTENDSIZE:
    case PACKEDSIZE:
      bytes = opesizeToBytes(size);
      break;

    case EVENID:
      bytes = parser->pc & 1;
      if (bytes) value = PEEK_BYTE(parser->pc);
      break;

    case CRID:
      break;

    case BYTEID:
    case ASCIIID:
      bytes = count;  // [n]がバイト数の指定になる
      count = 1;      // 繰り返し回数は1回に固定
      break;
    case ASCIIZID:
      bytes = getAsciizLength(parser);
      break;
    case LASCIIID:
      bytes = (int)PEEK_BYTE(parser->pc) + 1;
      break;
  }

  return parseSuccess(parser, size, count, bytes, value);
}

static ParseResult parseSizeid(Parser* parser, Token idToken) {
  ParseResult pr;
  opesize id = idToken.value;
  int count = 1;  //[]省略時は繰り返し回数=1

  TokenType type = getToken(&parser->tokenizer).type;

  if (type == '[') {
    Expr expr;
    consumeToken(&parser->tokenizer);

    pr = parseExpr(parser, &expr);
    if (pr != PARSE_SUCCESS) return pr;

    if (expr.type != EXPR_ULONG)
      return parseSyntaxError(parser,
                              "識別子の\"[]\"内には値以外は指定できません。");

    pr = parseCloseBracket(parser, ']');
    if (pr != PARSE_SUCCESS) return pr;

    count = expr.value.ul;
    type = getToken(&parser->tokenizer).type;
  }

  if (type == TOKEN_NUL) {
    // オペランド表現式がなければPCの値を出力する
    if (parser->writePtr != NULL) {
      stringifyDataAtPc(parser, id);
    }
  } else {
    // オペランド表現式があれば評価して文字列化
    if (idToken.type == TOKEN_FSIZEID) {
      return parseSyntaxError(
          parser, "識別子dc.[sdxp]にはオペランド表現式を指定できません。");
    }
    pr = parseExprString(parser);
    if (pr != PARSE_SUCCESS) return pr;
  }

  return evaluateSizeid(parser, id, count);
}

static const char* getNoCloseBracketMessage(TokenType type) {
  if (type == ']') return "\"[\"に対応する\"]\"がありません。";
  if (type == ')') return "\"(\"に対応する\")\"がありません。";
  return "\"{\"に対応する\"}\"がありません。";
}

static ParseResult parseCloseBracket(Parser* parser, TokenType close) {
  TokenType type = getToken(&parser->tokenizer).type;
  if (type == TOKEN_ERROR) return parseTokenError(parser);

  if (type != close)
    return parseSyntaxError(parser, getNoCloseBracketMessage(close));

  consumeToken(&parser->tokenizer);
  return PARSE_SUCCESS;
}

static ParseResult parseBreak(Parser* parser) {
  Expr expr;
  ParseResult pr = parseExpr(parser, &expr);
  if (pr != PARSE_SUCCESS) return pr;

  return parseSuccess(parser, BREAKID, 1, 0, expr.value.ul);
}

static Parser createParser(ParseTblParam* param, ParseTblResult* result) {
  Tokenizer tokenizer = createTokenizer(param->text, param->constValues);
  return (Parser){
      tokenizer,
      result,
      NULL,
      param->buffer,
      param->buffer + param->bufLen,
      param->constValues[PARSE_CONST_PC]  //
  };
}

// テーブル内の1行を解析、実行する
ParseResult parseTableLine(ParseTblParam* param, ParseTblResult* result) {
  Parser parser = createParser(param, result);
  Token token;

  *result = (ParseTblResult){PARSE_SUCCESS, 0, NULL, NULL, 0, 0, 0};

  token = getToken(&parser.tokenizer);
  if (token.type == TOKEN_ERROR) return parseTokenError(&parser);

  switch (token.type) {
    default:
      return parseSyntaxError(&parser, "行の先頭には識別子が必要です。");

    case TOKEN_NUL:
      break;

    case TOKEN_SIZEID:
    case TOKEN_FSIZEID:
      result->idstr = getIdStr(&parser.tokenizer);
      consumeToken(&parser.tokenizer);
      return parseSizeid(&parser, token);

    case TOKEN_BREAK:
      consumeToken(&parser.tokenizer);
      return parseBreak(&parser);
  }

  // 空行の場合
  return parseSyntaxError(&parser, "識別子がありません。");
}

static ParseResult parseError(Parser* parser, ParseResult pr,
                              const char* message) {
  ParseTblResult* result = parser->result;

  result->pr = pr;
  result->error = message;
  return pr;
}

static ParseResult parseSyntaxError(Parser* parser, const char* message) {
  return parseError(parser, PARSE_SYNTAX_ERROR, message);
}

static ParseResult parseTokenError(Parser* parser) {
  const char* message = getTokenizerError(&parser->tokenizer);
  return parseError(parser, PARSE_SYNTAX_ERROR, message);
}

static ParseResult parseRuntimeError(Parser* parser, const char* message) {
  return parseError(parser, PARSE_RUNTIME_ERROR, message);
}

// トークナイザを初期化する
static Tokenizer createTokenizer(char* p, ULONG* constValues) {
  Token token = {TOKEN_ERROR, 0};
  Tokenizer tokenizer = {p, NULL, NULL, FALSE, {NULL, 0}, token, constValues};

  return tokenizer;
}

typedef struct {
  uint8_t length;
  char str[13];
  uint8_t type;  // TokenType
  uint8_t size;  // opesize
} Keyword;

static const Keyword keywords[] = {
    {2, "pc", TOKEN_NUMBER, PARSE_CONST_PC},              //
    {8, "tabletop", TOKEN_NUMBER, PARSE_CONST_TABLETOP},  //

    {5, ".dc.b", TOKEN_SIZEID, BYTESIZE},  //
    {5, ".dc.w", TOKEN_SIZEID, WORDSIZE},  //
    {5, ".dc.l", TOKEN_SIZEID, LONGSIZE},  //
    {3, ".dc", TOKEN_SIZEID, WORDSIZE},    //
    {5, ".byte", TOKEN_SIZEID, BYTEID},    //
    {5, ".even", TOKEN_SIZEID, EVENID},    //

    {6, ".break", TOKEN_BREAK, 0},  //

    {6, "peek.b", TOKEN_PEEK, BYTESIZE},  //
    {6, "peek.w", TOKEN_PEEK, WORDSIZE},  //
    {6, "peek.l", TOKEN_PEEK, LONGSIZE},  //
    {5, "ext.w", TOKEN_EXT, WORDSIZE},    //
    {5, "ext.l", TOKEN_EXT, LONGSIZE},    //

    {3, ".cr", TOKEN_SIZEID, CRID},          //
    {6, ".ascii", TOKEN_SIZEID, ASCIIID},    //
    {6, ".asciz", TOKEN_SIZEID, ASCIIZID},   //
    {7, ".asciiz", TOKEN_SIZEID, ASCIIZID},  //
    {7, ".lascii", TOKEN_SIZEID, LASCIIID},  //

    {5, ".dc.s", TOKEN_FSIZEID, SINGLESIZE},  //
    {5, ".dc.d", TOKEN_FSIZEID, DOUBLESIZE},  //
    {5, ".dc.x", TOKEN_FSIZEID, EXTENDSIZE},  //
    {5, ".dc.p", TOKEN_FSIZEID, PACKEDSIZE},  //

};

static Token getTokenId(Tokenizer* tokenizer, StrSpan span) {
  const size_t kwLength = _countof(keywords);
  const char dot = (span.s[0] != '.') ? '.' : '\0';
  size_t i;

  for (i = 0; i < kwLength; ++i) {
    const Keyword* kw = &keywords[i];
    size_t len = kw->length;
    const char* t = kw->str;

    if (t[0] == dot) {
      // 入力がドットなし、比較キーワードがドット付きなら、ドットは除外する
      t += 1;
      len -= 1;
    }
    if (span.length == len && strncasecmp(span.s, t, len) == 0) {
      ULONG value = kw->size;

      // pc、tabletopを現時点の値にする
      if (kw->type == TOKEN_NUMBER) value = tokenizer->_constValues[value];

      tokenizer->_idstr = t;
      return (Token){kw->type, value};
    }
  }

  return (Token){TOKEN_ERROR, 0};
}

static StrSpan getKeywordSpan(char first, char* s) {
  const char* p = s;
  char c;

  if (isalpha(first) || first == '.') {
    while (isalnum(c = *p++) || c == '.')
      ;
  }
  return (StrSpan){s - 1, p - s};
}

// キーワード形式のトークン(識別子、定数、関数)
static Token fetchTokenKeyword(Tokenizer* tokenizer, char first) {
  StrSpan span = getKeywordSpan(first, tokenizer->_p);

  Token token = getTokenId(tokenizer, span);
  if (token.type != TOKEN_ERROR) tokenizer->_p += span.length - 1;
  return token;
}

static Token fetchTokenHex(Tokenizer* tokenizer, int zero_x) {
  char* p = tokenizer->_p;
  ULONG n = 0;

  p += zero_x;
  if (!isxdigit(*p)) {
    const char* e = zero_x ? "\"0x\"の後に16進数がありません。"
                           : "\"$\"の後に16進数がありません。";
    return tokenError(tokenizer, e);
  }

  do {
    char c = *p++;
    c = toupper(c);
    n = (n << 4) + (c >= 'A' ? c - 'A' + 10 : c - '0');
  } while (isxdigit(*p));
  tokenizer->_p = p;

  return (Token){TOKEN_NUMBER, n};
}

static Token fetchTokenDecimal(Tokenizer* tokenizer, char first) {
  char* p = tokenizer->_p;
  ULONG n = first - '0';

  while (isdigit(*p)) {
    n = n * 10 + (*p++ - '0');
  }
  tokenizer->_p = p;

  return (Token){TOKEN_NUMBER, n};
}

static Token fetchTokenString(Tokenizer* tokenizer) {
  char* term = strchr(tokenizer->_p, '"');

  if (term == NULL)
    return tokenError(tokenizer, "文字列が'\"'で閉じていません。");

  // 構文仕様上、複数の文字列を同時に保持する必要はないので
  // 情報は Tokenizer 内に一つだけ置く
  tokenizer->_strSpan = (StrSpan){tokenizer->_p, term - tokenizer->_p};

  tokenizer->_p = term + 1;
  return (Token){TOKEN_STRING, 0};
}

static Token fetchToken(Tokenizer* tokenizer) {
  char c, c2;
  tokenizer->_error = NULL;

  while ((c = *tokenizer->_p++) == ' ' || c == '\t' || c == '\n')
    ;
  if (c == '\0') {
    tokenizer->_p -= 1;
    return (Token){TOKEN_NUL, 0};
  }
  c2 = *tokenizer->_p;

  // c  -> 文字列の先頭の文字
  // c2 -> 2文字目。tokenizer->_p はこの文字のアドレスを指している

  // 16進数リテラル
  if (c == '$') return fetchTokenHex(tokenizer, 0);
  if (c == '0' && tolower(c2) == 'x') return fetchTokenHex(tokenizer, 1);

  // 10進数リテラル
  if (isdigit(c)) return fetchTokenDecimal(tokenizer, c);

  // 文字列リテラル
  if (c == '"') return fetchTokenString(tokenizer);

  // 記号2文字の演算子
  if (c == '!' && c2 == '=') {
    tokenizer->_p += 1;
    return (Token){TOKEN_NOT_EQUAL, 0};
  }
  if (c == '<' && c2 == '=') {
    tokenizer->_p += 1;
    return (Token){TOKEN_LESS_OR_EQUAL, 0};
  }
  if (c == '>' && c2 == '=') {
    tokenizer->_p += 1;
    return (Token){TOKEN_GREATER_OR_EQUAL, 0};
  }

  if (c == '=') {
    if (c2 != c) return tokenError(tokenizer, "\"=\"演算子は使えません。");
    tokenizer->_p += 1;
    return (Token){TOKEN_EQUAL, 0};
  }
  if (c == '&') {
    if (c2 != c) return tokenError(tokenizer, "\"&\"演算子は使えません。");
    tokenizer->_p += 1;
    return (Token){TOKEN_LOGICAL_AND, 0};
  }
  if (c == '|') {
    if (c2 != c) return tokenError(tokenizer, "\"|\"演算子は使えません。");
    tokenizer->_p += 1;
    return (Token){TOKEN_LOGICAL_OR, 0};
  }

  // 記号1文字の演算子
  if (strchr("!%()*+,-/<>[]{}", c) != NULL) return (Token){c, 0};

  // 識別子、定数、関数
  {
    Token token = fetchTokenKeyword(tokenizer, c);
    if (token.type != TOKEN_ERROR) return token;
  }

  return tokenError(tokenizer, "トークンとして解釈できません。");
}

static Token tokenError(Tokenizer* tokenizer, const char* message) {
  tokenizer->_error = message;
  return (Token){TOKEN_ERROR, 0};
}

static const char* getTokenizerError(Tokenizer* tokenizer) {
  return tokenizer->_error;
}

// 字句解析してトークンを返す
//   トークンは内部に保持して次のgetToken()でも同じものを返す。
//   取得したトークンが不要になったらconsumeToken()を呼び出すこと。
static Token getToken(Tokenizer* tokenizer) {
  if (tokenizer->_hasToken == FALSE) {
    tokenizer->_hasToken = TRUE;
    tokenizer->_current = fetchToken(tokenizer);
  }
  return tokenizer->_current;
}

// 内部に保持しているトークンを消費済みとする(消去する)。
static void consumeToken(Tokenizer* tokenizer) { tokenizer->_hasToken = FALSE; }

// EOF
