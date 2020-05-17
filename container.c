#include "9cc.h"

char *user_input;

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
