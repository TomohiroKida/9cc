#include "9cc.h"

Token *token;

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs  = lhs;
  node->rhs  = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val  = val;
  return node;
}

// 次のトークンが期待している記号で同じ長さのときには，トークンを1つ読み進めて
// 真を返す．それ以外の場合には偽を返す．
bool consume(char *op) {
  if (token->kind != TK_RESERVED || token->len != strlen(op) ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

// 次のトークンが期待している記号で同じ長さのときには，トークンを1つ読み進める．
// それ以外の場合にはエラーを報告する．
void expect(char *op) {
  if (token->kind != TK_RESERVED || token->len != strlen(op) ||
      memcmp(token->str, op, token->len))
    error_at(token->str, "'%c'ではありません", op);
  token = token->next;
}

// 次のトークンが期待している数値の場合，トークンを1つ読み進めてその数値を返す．
// それ以外の場合にはエラーを報告する．
int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q))==0;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize() {
  Token head;
  head.next = NULL;
  Token *cur = &head;
  char *p = user_input;

  while (*p) {
    // 空白をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    if ('a' <= *p &&  *p <= 'z') {
        cur = new_token(TK_IDENT, cur, p, 1);
        p++;
        continue;
    }

    if (startswith(p, "==") || startswith(p, "!=") ||
        startswith(p, "<=") || startswith(p, ">=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    //printf("%d\n", strchr("+-*/()<>", *p));
    if (strchr("+-*/()<>", *p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (isdigit(*p)) {
      // 数字トークンを作るときは長さは0にしておいて，
      // strtolで値を取得する前の文字列ポインタと
      // した後の文字列ポインタのアドレス値の差分を数字トークンの長さとする
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p-p;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}

Node *expr() {
  return equality();
}

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NE, node, relational());
    else 
      return node;
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<"))
      node = new_node(ND_LT, node, add());
    else if (consume("<="))
      node = new_node(ND_LE, node, add());
    else if (consume(">"))
      node = new_node(ND_LT, add(), node);
    else if (consume(">="))
      node = new_node(ND_LE, add(), node);
    else 
      return node;
  }
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary() {
  if (consume("+")) 
    return primary();
  if (consume("-")) 
    //return new_node_num(-expect_number());
    return new_node(ND_SUB, new_node_num(0), unary());
  return primary();
}

Node *primary() {
  // 次のトークンが'('なら，'(' expr ')' なはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }
  
  // そうでなければ数値のはず
  return new_node_num(expect_number());
}
