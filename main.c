#include "9cc.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "num of arguments is not corrent\n");
    return 1;
  }

  if (strcmp(argv[1], "-test")==0) {
    runtest();
    return 0;
  }

  // tokenize and parse
  tokenize(argv[1]);
  Node *node = add();

  // pre part of assembly
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n\n");

  // shake AST and create code
  gen(node);

  // value of fomula remain stack top,
  // load it to rax and return from function
  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
