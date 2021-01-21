#include "succ.h"

Node* new_node(NodeKind kind, Node* lhs, Node* rhs) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node* new_node_num(int val) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node* new_node_if(Node* cond, Node* true_stmt, Node* false_stmt) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_IF;
    node->cond = cond;
    node->true_stmt = true_stmt;
    node->false_stmt = false_stmt;
    return node;
}

Node* new_node_for(Node* for_init, Node* for_cond, Node* for_tick, Node* for_body) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;
    node->for_init = for_init;
    node->for_cond = for_cond;
    node->for_tick = for_tick;
    node->for_body = for_body;
    return node;
};

LVar* locals;

LVar* find_lvar(Token* tok) {
    for (LVar* var = locals; var; var = var->next) {
        if (var->len == tok->len && strncmp(var->name, tok->str, var->len) == 0) {
            return var;
        }
    }
    return NULL;
}

Node* code[100];

void program() {
    int i = 0;
    while (token->kind != TK_EOF) {;
        code[i++] = stmt();
    }
    code[i] = NULL;
}

Node* stmt() {
    Node* node;
    if (consume_token(TK_RETURN)) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
        expect(";");
        return node;
    }
    if (consume_token(TK_IF)) {
        expect("(");
        Node* cond = expr();
        expect(")");
        Node* true_stmt = stmt();
        if (consume_token(TK_ELSE)) {
            Node* false_stmt = stmt();
            return new_node_if(cond, true_stmt, false_stmt);
        }
        return new_node_if(cond, true_stmt, NULL);
    }
    if (consume_token(TK_FOR)) {
        Node* init;
        Node* cond;
        Node* tick;
        Node* body;
        expect("(");
        init = expr();
        expect(";");
        cond = expr();
        expect(";");
        tick = expr();
        expect(")");
        body = stmt();
        return new_node_for(init, cond, tick, body);
    }
    if (consume_token(TK_WHILE)) {
        Node* cond;
        Node* body;
        expect("(");
        cond = expr();
        expect(")");
        body = stmt();
        return new_node_for(new_node_num(0), cond, new_node_num(0), body);
    }
    node = expr();
    expect(";");
    return node;
}

Node* expr() {
    Node* node = assign();
    return node;
}

Node* assign() {
    Node* node = equality();
    if (consume("=")) {
        node = new_node(ND_ASSIGN, node, assign());
    }
    return node;
}

Node* equality() {
    Node* node = relational();
    for (;;) {
        if (consume("==")) {
            node = new_node(ND_EQ, node, relational());
        } else if (consume("!=")) {
            node = new_node(ND_NE, node, relational());
        } else {
            return node;
        }
    }
}

Node* relational() {
    Node* node = add();
    for (;;) {
        if (consume("<")) {
            node = new_node(ND_LT, node, add());
        } else if (consume("<=")) {
            node = new_node(ND_LE, node, add());
        } else if (consume(">")) {
            node = new_node(ND_LT, add(), node);
        } else if (consume(">=")) {
            node = new_node(ND_LE, add(), node);
        } else {
            return node;
        }
    }
}

Node* add() {
    Node* node = mul();
    for (;;) {
        if (consume("+")) {
            node = new_node(ND_ADD, node, mul());
        } else if (consume("-")) {
            node = new_node(ND_SUB, node, mul());
        } else {
            return node;
        }
    }
}

Node* mul() {
    Node* node = unary();
    for (;;) {
        if (consume("*")) {
            node = new_node(ND_MUL, node, unary());
        } else if (consume("/")) {
            node = new_node(ND_DIV, node, unary());
        } else {
            return node;
        }
    }
}

Node* unary() {
    if (consume("+")) {
        return primary();
    }
    if (consume("-")) {
        return new_node(ND_SUB, new_node_num(0), primary());
    }
    return primary();
}

int offset = 0;

Node* primary() {
    if (consume("(")) {
        Node* node = expr();
        expect(")");
        return node;
    }
    Token* tok = consume_token(TK_IDENT);
    if (tok) {
        Node* node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;
        LVar* lvar = find_lvar(tok);
        if (lvar) {
            node->offset = lvar->offset;
        } else {
            lvar = calloc(1, sizeof(LVar));
            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset = offset;
            node->offset = lvar->offset;
            locals = lvar;
            offset += 8;
        }
        return node;
    }
    return new_node_num(expect_number());
}

Token* consume_token(TokenKind kind) {
    Token* t = token;
    if (t->kind == kind) {
        token = token-> next;
        return t;
    }
    return NULL;
}