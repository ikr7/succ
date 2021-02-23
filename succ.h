#include <ctype.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tokenize.c

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

Token* consume_token(TokenKind kind);
Token* expect_token(TokenKind kind);
bool consume_punct(char* op);
void expect_punct(char* op);
int expect_number(void);
bool at_eof(void);
bool is_alnum_underscore(char c);

Token* new_token(TokenKind kind, Token* cur, char* str, int len);
Token* tokenize(void);

// parse.c

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
    ND_DEREF,
    ND_ADDR,
    ND_NOP,
} NodeKind;

typedef enum {
    TP_INT,
    TP_PTR,
} TypeKind;

typedef struct Type Type;

struct Type {
    TypeKind type;
    Type* ptr_to;
};

typedef struct LVar LVar;

struct LVar {
    LVar* next;
    char* name;
    int len;
    int offset;
    Type* type;
};

LVar* find_lvar(LVar* locals, char* name, int len);

typedef struct Node Node;

struct Node {
    NodeKind kind;

    // binary operators
    Node* lhs;
    Node* rhs;

    // variable
    LVar* lvar;
    int val;

    // return
    Node* return_body;

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
    Node* function_arg;
    Node* function_arg_next;
};

Node* new_node_binop(NodeKind kind, Node* lhs, Node* rhs);
Node* new_node_num(int val);
Node* new_node_if(Node* cond, Node* true_stmt, Node* false_stmt);
Node* new_node_for(Node* for_init, Node* for_cond, Node* for_tick, Node* for_body);

typedef struct Func Func;

struct Func {
    char* name;
    int len;
    int offset;
    LVar* args;
    LVar* locals;
    Node* body;
    Type* return_type;
};

void program(void);
Func* funcdef(void);
Node* stmt(void);
Node* expr(void);
Node* assign(void);
Node* equality(void);
Node* relational(void);
Node* add(void);
Node* mul(void);
Node* unary(void);
Node* primary(void);

Type* get_type(Node* node);

// codegen.c

void gen(void);
void gen_func(Func* func);
void gen_node(Node* node);
void gen_binop(Node* node);
void gen_lval(Node* node);

// main.c

int dump_tokens(void);

extern Func* code[];
extern Func* cur_func;
extern Token *token;
extern char* user_input;