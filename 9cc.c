#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// value representing type of token
enum {
  TK_NUM = 256, // integer token
  TK_EOF,       // token representing end of input
};

typedef struct {
  int ty;      // type of token
  int val;     // if ty is 'TK_NUM', it is the value
  char *input; // token string (for error message)
} Token;

// token array tokenized save this array.
// define that more 100 tokens don't come
Token tokens[100];
 
// it report error
// it have same arguments to 'printf'
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// string that p arrowing are splited by token and saved to 'tokens'
void tokenize(char *p) {
  int i = 0;
  while (*p) {
    // skip space word
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-') {
      tokens[i].ty    = *p;
      tokens[i].input =  p;
      i++;
      p++;
      continue;
    }

    if (isdigit(*p)) {
      tokens[i].ty    = TK_NUM;
      tokens[i].input = p;
      tokens[i].val   = strtol(p, &p, 10);
      i++;
      continue;
    }

    error("don't tokenize: %s", p);
    exit(1);
  }

  tokens[i].ty    = TK_EOF;
  tokens[i].input = p;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "num of arguments is not corrent\n");
    return 1;
  }

  tokenize(argv[1]);

  // pre part of assembly
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n\n");

  if (tokens[0].ty != TK_NUM) 
    error("first term is not number");
  else 
    printf("  mov rax, %d\n", tokens[0].val);

  int i = 1;
  while (tokens[i].ty != TK_EOF) {
    if (tokens[i].ty == '+') {
      i++;
      if (tokens[i].ty != TK_NUM)
        error("no expected word: %s\n", tokens[i].input);
      printf(" add rax, %d\n", tokens[i].val);
      i++;
      continue;
    }
    if (tokens[i].ty == '-') {
      i++;
      if (tokens[i].ty != TK_NUM)
        error("no expected word: %s\n", tokens[i].input);
      printf(" sub rax, %d\n", tokens[i].val);
      i++;
      continue;
    }

    error("no expected word: %s\n", tokens[i].input);
  }
  printf("  ret\n");
  return 0;
}
