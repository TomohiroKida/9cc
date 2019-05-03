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

enum {
  ND_NUM = 256; // type of integer node
};

typedef struct Node {
  int ty;           // operation or 'ND_NUM'
  struct Node *lhs; // left-hand side
  struct Node *rhs; // right-hand side
  int val;          // use if ty is 'ND_NUM'
} Node;

// token array tokenized save this array.
// define that more 100 tokens don't come
Token tokens[100];
// position of tokens now
int pos = 0;
 
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

Node *new_node(int ty, Node *lhs, Node *rhs) {
  Node *node = malloc(sizeof(Node));
  node->ty = ty;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node(int val) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_NUM;
  node->val = val;
  return node;
}

int consume(int ty) {
  if (toknes[pos].ty != ty)
    return 0;
  pos++;
  return 1;
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume('+'))
      node = new_node('+', node, mul());
    else if (consume('-'))
      node = new_node('-', node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = term();

  for (;;) {
    if (consume('*'))
      node = new_node('*', node, term());
    else if (consume('/'))
      node = new_node('/', node, term());
    else
      return node;
  }
}

Node *term() {
  if (consume('(')) {
    Node *node = add();
    if (!consume(')'))
      error("no close blacket appending open blacket: %s",
          tokens[pos].input);
    return node;
  }

  if (toknes[pos].ty == TK_NUM) 
    return new_node_num(tokens[pos++].val);

  error("token is num or open backets: %s", tokens[pos].input);
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
