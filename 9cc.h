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

typedef struct {
  void **data;
  int capacity;
  int len;
} Vector;

extern Token tokens[100];
extern int pos;

extern Vector *new_vector();                           
extern void vec_push(Vector *vec, void *elem);         
extern int expect(int line, int expected, int actual); 
extern void runtest();                                 
extern void error(char *fmt, ...);                     
extern void tokenize(char *p);                         
extern Node *new_node(int ty, Node *lhs, Node *rhs);   
extern Node *new_node_num(int val);                    
extern int consume(int ty);                            
extern Node *add();                                    
extern Node *mul();                                    
extern Node *unary();                                  
extern Node *term();                                   
extern void gen(Node *node);                           
