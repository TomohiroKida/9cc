#include "9cc.h"

// token array tokenized save this array.
// define that more 100 tokens don't come
Token tokens[100];
// position of tokens now
int pos = 0;
 
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
  // add: mul
  Node *node = mul();

  for (;;) {
    // add: add "+" mul
    if (consume('+'))
      node = new_node('+', node, mul());
    // add: add "-" mul
    else if (consume('-'))
      node = new_node('-', node, mul());
    // add: mul
    else
      return node;
  }
}

Node *mul() {
  // mul: unary
  Node *node = unary();

  for (;;) {
    // mul: mul "*" unary
    if (consume('*'))
      node = new_node('*', node, unary());
    // mul: mul "/" unary 
    else if (consume('/'))
      node = new_node('/', node, unary());
    // mul: unary
    else
      return node;
  }
}

Node *unary() {
  // unary: "+" term
  if (consume('+')) 
    return term();
  // unary: "-" term
  if (consume('-'))
    return new_node('-', new_node_num(0), term());
  // unary: term
  return term();
}

Node *term() {
  // term: "(" add ")"
  if (consume('(')) {
    Node *node = add();
    if (!consume(')'))
      error("no close blacket appending open blacket: %s",
          tokens[pos].input);
    return node;
  }

  // term: num
  if (tokens[pos].ty == TK_NUM) 
    return new_node_num(tokens[pos++].val);
  else
    error("token is num or open backets: %s", tokens[pos].input);
}
