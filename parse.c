#include "9cc.h"

static Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs  = lhs;
  node->rhs  = rhs;
  return node;
}

static Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val  = val;
  return node;
}

static Node *stmt();
static Node *expr();
static Node *assign();
static Node *equality();
static Node *relational();
static Node *add();
static Node *mul();
static Node *unary();
static Node *primary();

// 次のトークンが期待している記号で同じ長さのときには，トークンを1つ読み進めて
// 真を返す．それ以外の場合には偽を返す．
static bool consume(char *op) {
  if (token->kind != TK_RESERVED || token->len != strlen(op) ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

// 現在のトークンが識別子かどうか比較する
static Token *consume_ident() {
    if (token->kind != TK_IDENT)
        return NULL;
    Token *t = token;
    token = token->next;
    return t;
}

// 次のトークンが期待している記号で同じ長さのときには，トークンを1つ読み進める．
// それ以外の場合にはエラーを報告する．
static void expect(char *op) {
  if (token->kind != TK_RESERVED || token->len != strlen(op) ||
      memcmp(token->str, op, token->len))
    error_at(token->str, "'%c'ではありません", op);
  token = token->next;
}

// 次のトークンが期待している数値の場合，トークンを1つ読み進めてその数値を返す．
// それ以外の場合にはエラーを報告する．
static int expect_number() {
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
static Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

static bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q))==0;
}

Token *token;
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

    // 識別子
    if ('a' <= *p && *p <= 'z') {
        cur = new_token(TK_IDENT, cur, p, 1);
        p++;
        continue;
    }

    // ２文字の句読点
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

// program = stmt*
void *program() {
    int i = 0;
    while (!at_eof()) 
        code[i++] = stmt();
    code[i] = NULL;
}

// stmt = expr ";"
static Node *stmt() {
    Node *node = expr();
    expect(";");
    return node;
}

// expr = assign
static Node *expr() {
  return assign();
}

// assign = equality ("=" assign)?
static Node *assign() {
    Node *node = equality();
    if (consume("=")) 
        node = new_node(ND_ASSIGN, node, assign());
    return node;
}

// equality = relational ("==" relational | "!=" relational)*
static Node *equality() {
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

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
static Node *relational() {
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

// add = mul ("+" mul | "-" mul)*
static Node *add() {
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

// mul = unary ("*" unary | "/" unary)*
static Node *mul() {
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

// unary = ("+" | "-")? primary
static Node *unary() {
  if (consume("+")) 
    return unary();
  if (consume("-")) 
    //return new_node_num(-expect_number());
    return new_node(ND_SUB, new_node_num(0), unary());
  return primary();
}

// primary = num | indent | "(" expr ")"
static Node *primary() {
  // 次のトークンが'('なら，'(' expr ')' なはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }
  
  // 識別子なら，ND_LVARなはず
  // tok = 識別子以降のトークン
  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->offset = (tok->str[0] - 'a' + 1) * 8;
    return node; 
  }

  // そうでなければ数値のはず
  return new_node_num(expect_number());
}
