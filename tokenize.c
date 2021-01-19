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

bool consume(char* op) {
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

void expect(char* op) {
    if (
        token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        strncmp(token->str, op, token->len) != 0
    ) {
        error_at(token->str, "not '%s'", op);
    }
    token = token-> next;
}

int expect_number() {
    if (token->kind != TK_NUM) {
        error_at(token->str, "Not a number");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool is_alnum(char c) {
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

Token* tokenize() {
    char* p = user_input;
    Token head;
    head.next = NULL;
    Token* cur = &head;
    while (*p) {
        if (isspace(*p)) {
            p++;
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
        if (
            *p == '+' ||
            *p == '-' ||
            *p == '*' ||
            *p == '/' ||
            *p == '(' ||
            *p == ')' ||
            *p == '<' ||
            *p == '>' ||
            *p == '=' ||
            *p == ';'
        ) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }
        if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
            cur = new_token(TK_RETURN, cur, p, 6);
            p += 6;
            continue;
        }
        if ('A' <= *p && *p <= 'z' || *p == '_') {
            cur = new_token(TK_IDENT, cur, p, 0);
            char* q = p;
            while (
                'A' <= *p && *p <= 'z' ||
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