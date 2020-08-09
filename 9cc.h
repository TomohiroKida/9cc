#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// tokenize.c
//

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

// トークン型
typedef struct Token Token;
struct Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合，その数値
  char *str;      // トークン文字列
  int len;        // トークンの長さ
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
Token *consume_ident(void);
void expect(char *op);
int expect_number(void);
bool at_eof(void);
Token *tokenize(void);

// 入力プログラム
extern char *user_input;
// 現在着目しているトークン
extern Token *token;

//
// parse.c
//

typedef enum {
  ND_ADD,       // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LT,        // <
  ND_LE,        // <=
  ND_ASSIGN,    // =
  ND_RETURN,    // "return"
  ND_EXPR_STMT, // 式文
  ND_VAR,       // 変数
  ND_NUM,       // 整数
} NodeKind;

// 抽象構文木のノード型
typedef struct Node Node;
struct Node {
  TokenKind kind; // ノードの型
  Node *next;     // 次のノード
  Node *lhs;      // 左辺
  Node *rhs;      // 右辺
  char name;      // kindがND_VARの場合のみ使う
  int val;        // kindがND_NUMの場合のみ使う
};

Node *program(void);

//
// codegen.c
//

void codegen(Node *node);
