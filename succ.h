#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tokenize

typedef enum {
    TK_RESERVED,
    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_FOR,
    TK_WHILE,
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
bool is_alnum(char c);

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
    ND_RETURN,
    ND_IF,
    ND_FOR,
    ND_BLOCK,
    ND_CALL,
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;

    // operators
    Node* lhs;
    Node* rhs;

    // variable
    int val;
    int offset;

    // if
    Node* if_cond;
    Node* if_then;
    Node* if_else;

    // for
    Node* for_init;
    Node* for_cond;
    Node* for_tick;
    Node* for_body;

    // block
    Node* block_stmts;
    Node* block_next;

    // function call
    char* function_name;
    int function_len;
};

Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
Node* new_node_num(int val);
Node* new_node_if(Node* cond, Node* true_stmt, Node* false_stmt);
Node* new_node_for(Node* for_init, Node* for_cond, Node* for_tick, Node* for_body);

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

Token* consume_token();

// codegen

void gen(Node* node);
void gen_lval(Node* node);

// main

int dump_tokens();

extern Token *token;
extern char* user_input;