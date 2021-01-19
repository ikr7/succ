#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tokenize

typedef enum {
    TK_RESERVED,
    TK_IDENT,
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *str;
    int len;
};

void error(char* fmt, ...);
void error_at(char* loc, char* fmt, ...);
bool consume(char* op);
void expect(char* op);
int expect_number();
bool at_eof();

Token* new_token(TokenKind kind, Token* cur, char* str, int len);
Token* tokenize();

// parse

typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_ASSIGN,
    ND_LVAR,
    ND_NUM,
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node* lhs;
    Node* rhs;
    int val;
    int offset;
};

Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
Node* new_node_num(int val);

typedef struct LVar LVar;

struct LVar {
    LVar* next;
    char* name;
    int len;
    int offset;
};

LVar* find_lvar(Token* tok);

extern Node* code[];
extern LVar* locals;

extern int offset;

void program();
Node* stmt();
Node* expr();
Node* assign();
Node* equality();
Node* relational();
Node* add();
Node* mul();
Node* unary();
Node* primary();

Token* consume_ident();

// codegen

void gen(Node* node);
void gen_lval(Node* node);

// main

int dump_tokens();

extern Token *token;
extern char* user_input;