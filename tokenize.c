#include "9cc.h"

char *user_input;
Token *token;

// エラーを報告するための関数
// printfと同じ引数
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// エラー箇所を報告する
// loc: トークンの途中を指す文字列
// fmt: エラー出力文字列
void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  //fprintf(stderr, "%s\n%d %d %d\n", loc, pos, loc, user_input);
  exit(1);
}

// 次のトークンが期待している記号で同じ長さのときには，トークンを1つ読み進めて
// 真を返す．それ以外の場合には偽を返す．
bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      strncmp(token->str, op, token->len))
      return false;
  token = token->next;
  return true;
}

// 次のトークンが期待している記号で同じ長さのときには，トークンを1つ読み進める．
// それ以外の場合にはエラーを報告する．
void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      strncmp(token->str, op, token->len))
      error_at(token->str, "\"%s\" が期待された", op);
  token = token->next;
}

// 次のトークンが期待している数値の場合，トークンを1つ読み進めてその数値を返す．
// それ以外の場合にはエラーを報告する．
int expect_number(void) {
    if (token->kind != TK_NUM)
        error_at(token->str, "数ではありません");
    long val = token->val;
    token = token->next;
    return val;
}

bool at_eof(void) {
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
    return strncmp(p, q, strlen(q)) == 0;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize() {
    char *p = user_input;
    Token head = {};
    Token *cur = &head;

    while (*p) {
        // 空白をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (startswith(p, "==") || startswith(p, "!=") ||
            startswith(p, "<=") || startswith(p, ">=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        // 単項演算子
        if (strchr("+-*/()<>", *p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        // 整数リテラル
        if (isdigit(*p)) {
            // 数字トークンを作るときは長さは0にしておいて，
            // strtolで値を取得する前の文字列ポインタと
            // した後の文字列ポインタのアドレス値の差分を数字トークンの長さとする
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - p;
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}
