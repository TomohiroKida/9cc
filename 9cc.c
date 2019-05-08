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
  ND_NUM = 256, // type of integer node
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
 
void error(char *fmt, ...);
void tokenize(char *p);
Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
int consume(int ty);
Node *add();
Node *mul();
Node *term();
void gen(Node *node);

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

    if (*p == '+' || 
        *p == '-' ||
        *p == '*' || 
        *p == '/' || 
        *p == '(' || 
        *p == ')') {
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

Node *new_node_num(int val) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_NUM;
  node->val = val;
  return node;
}

int consume(int ty) {
  if (tokens[pos].ty != ty)
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

  if (tokens[pos].ty == TK_NUM) 
    return new_node_num(tokens[pos++].val);

  error("token is num or open backets: %s", tokens[pos].input);
}

void gen(Node *node) {
  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->ty) {
    case '+':
      printf("  add rax, rdi\n");
      break;
    case '-':
      printf("  sub rax, rdi\n");
      break;
    case '*':
      printf("  mul rdi\n");
      break;
    case '/':
      printf("  mov rdx, 0\n");
      printf("  div rdi\n");
      break;
  }

  printf("  push rax\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "num of arguments is not corrent\n");
    return 1;
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
