#include "succ.h"

void error(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at(char* loc, char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

Token* consume_token(TokenKind kind) {
    Token* t = token;
    if (t->kind == kind) {
        token = token-> next;
        return t;
    }
    return NULL;
}

Token* expect_token(TokenKind kind) {
    Token* t = token;
    if (t->kind != kind) {
        error_at(t->str, "unexpected token");
    }
    token = token-> next;
    return t;
}

bool consume_punct(char* op) {
    if (
        token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        strncmp(token->str, op, token->len) != 0
    ) {
        return false;
    }
    token = token-> next;
    return true;
}

void expect_punct(char* op) {
    if (
        token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        strncmp(token->str, op, token->len) != 0
    ) {
        error_at(token->str, "not '%s'", op);
    }
    token = token-> next;
}

int expect_number(void) {
    if (token->kind != TK_NUM) {
        error_at(token->str, "Not a number");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(void) {
    return token->kind == TK_EOF;
}

bool is_alnum_underscore(char c) {
    return (
        'a' <= c && c <= 'z' ||
        'A' <= c && c <= 'Z' ||
        '0' <= c && c <= '9' ||
        c == '_'
    );
}

Token* new_token(TokenKind kind, Token* cur, char* str, int len) {
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

Token* tokenize(void) {
    char* p = user_input;
    Token head;
    head.next = NULL;
    Token* cur = &head;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }
        if (strncmp(p, "int", 3) == 0) {
            cur = new_token(TK_RESERVED, cur, p, 3);
            p += 3;
            continue;
        }
        if (
            strncmp(p, "==", 2) == 0 ||
            strncmp(p, "!=", 2) == 0 ||
            strncmp(p, "<=", 2) == 0 ||
            strncmp(p, ">=", 2) == 0
        ) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }
        if (strchr("+-*/()<>=;{},&", *p) != NULL) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }
        if (strncmp(p, "sizeof", 6) == 0 && !is_alnum_underscore(p[6])) {
            cur = new_token(TK_RESERVED, cur, p, 6);
            p += 6;
            continue;
        }
        if (strncmp(p, "return", 6) == 0 && !is_alnum_underscore(p[6])) {
            cur = new_token(TK_RETURN, cur, p, 6);
            p += 6;
            continue;
        }
        if (strncmp(p, "if", 2) == 0 && !is_alnum_underscore(p[2])) {
            cur = new_token(TK_IF, cur, p, 2);
            p += 2;
            continue;
        }
        if (strncmp(p, "else", 4) == 0 && !is_alnum_underscore(p[4])) {
            cur = new_token(TK_ELSE, cur, p, 4);
            p += 4;
            continue;
        }
        if (strncmp(p, "for", 3) == 0 && !is_alnum_underscore(p[3])) {
            cur = new_token(TK_FOR, cur, p, 3);
            p += 3;
            continue;
        }
        if (strncmp(p, "while", 5) == 0 && !is_alnum_underscore(p[5])) {
            cur = new_token(TK_WHILE, cur, p, 5);
            p += 5;
            continue;
        }
        if ('a' <= *p && *p <= 'z' || 'A' <= *p && *p <= 'Z' || *p == '_') {
            cur = new_token(TK_IDENT, cur, p, 0);
            char* q = p;
            while (
                'a' <= *p && *p <= 'z' ||
                'A' <= *p && *p <= 'Z' ||
                '0' <= *p && *p <= '9' ||
                *p == '_'
            ) {
                p++;
            }
            cur->len = p - q;
            continue;
        }
        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }
        error_at(p, "Cannot be tokenized");
    }
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}