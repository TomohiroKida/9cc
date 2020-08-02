#include "9cc.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    error_at(token->str, "引数の個数が正しくありません");
    return 1;
  }

  // 入力の先頭を指すポインタ
  // トークナイズしてパースする
  // 結果はcodeに保存される
  user_input = argv[1];
  tokenize();
  program();

  // アセンブリの前半部分の出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // プロローグ
  // 変数26個分の領域を確保する
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  // 先頭の式から順にコード生成
  for (int i = 0; code[i]; i++) {
      gen(code[i]);

      // 式の評価結果としてスタックに１つの値が残っているはずなので，
      // スタックが溢れないようにポップしておく
      printf("  pop rax\n");
  }

  // エピローグ
  // 最後の式の結果がRAXに残っているのでそれが返り値になる
  printf("  mov rsp, rbp\n");
  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
