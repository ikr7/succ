#include "succ.h"

Node* new_node_binop(NodeKind kind, Node* lhs, Node* rhs) {
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
    node->if_cond = cond;
    node->if_then = true_stmt;
    node->if_else = false_stmt;
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

LVar* find_lvar(LVar* locals, char* name, int len) {
    for (LVar* var = locals; var; var = var->next) {
        if (var->len == len && strncmp(var->name, name, len) == 0) {
            return var;
        }
    }
    return NULL;
}

void program() {
    int i = 0;
    while (!at_eof()) {;
        code[i++] = funcdef();
    }
    code[i] = NULL;
}

Func* funcdef() {
    Func* func = calloc(1, sizeof(Func));
    cur_func = func;
    Token* ident = expect_token(TK_IDENT);
    func->name = ident->str;
    func->len = ident->len;

    expect_punct("(");

    if (!consume_punct(")")) {
        
        Token* tok = expect_token(TK_IDENT);
        LVar* arg = calloc(1, sizeof(LVar));
        
        arg->name = tok->str;
        arg->len = tok->len;
        arg->offset = func->offset + 8;
        
        func->offset += 8;
        func->args = arg;

        LVar* cur = arg;

        while (!consume_punct(")")) {
            expect_punct(",");
            tok = expect_token(TK_IDENT);
            arg = calloc(1, sizeof(LVar));
            arg->name = tok->str;
            arg->len = tok->len;
            cur->next = arg;
            arg->offset = func->offset + 8;
            func->offset += 8;
            cur = arg;
        }

        func->locals = func->args;

    }

    expect_punct("{");
    Node* body = calloc(1, sizeof(Node));
    Node* stmts = calloc(1, sizeof(Node));
    body->kind = ND_BLOCK;
    body->block_stmts = stmts;
    while (!consume_punct("}")) {
        Node* s = stmt();
        stmts->block_next = s;
        stmts = s;
    };
    body->block_stmts = body->block_stmts->block_next;
    func->body = body;
    return func;
}

Node* stmt() {
    Node* node;
    if (consume_token(TK_RETURN)) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->return_body = expr();
        expect_punct(";");
        return node;
    }
    if (consume_token(TK_IF)) {
        expect_punct("(");
        Node* cond = expr();
        expect_punct(")");
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
        expect_punct("(");
        init = expr();
        expect_punct(";");
        cond = expr();
        expect_punct(";");
        tick = expr();
        expect_punct(")");
        body = stmt();
        return new_node_for(init, cond, tick, body);
    }
    if (consume_token(TK_WHILE)) {
        Node* cond;
        Node* body;
        expect_punct("(");
        cond = expr();
        expect_punct(")");
        body = stmt();
        return new_node_for(new_node_num(0), cond, new_node_num(0), body);
    }
    if (consume_punct("{")) {
        Node* node = calloc(1, sizeof(Node));
        Node* stmts = calloc(1, sizeof(Node));
        node->kind = ND_BLOCK;
        node->block_stmts = stmts;
        while (!consume_punct("}")) {
            Node* s = stmt();
            stmts->block_next = s;
            stmts = s;
        };
        node->block_stmts = node->block_stmts->block_next;
        return node;
    }
    node = expr();
    expect_punct(";");
    return node;
}

Node* expr() {
    Node* node = assign();
    return node;
}

Node* assign() {
    Node* node = equality();
    if (consume_punct("=")) {
        node = new_node_binop(ND_ASSIGN, node, assign());
    }
    return node;
}

Node* equality() {
    Node* node = relational();
    for (;;) {
        if (consume_punct("==")) {
            node = new_node_binop(ND_EQ, node, relational());
        } else if (consume_punct("!=")) {
            node = new_node_binop(ND_NE, node, relational());
        } else {
            return node;
        }
    }
}

Node* relational() {
    Node* node = add();
    for (;;) {
        if (consume_punct("<")) {
            node = new_node_binop(ND_LT, node, add());
        } else if (consume_punct("<=")) {
            node = new_node_binop(ND_LE, node, add());
        } else if (consume_punct(">")) {
            node = new_node_binop(ND_LT, add(), node);
        } else if (consume_punct(">=")) {
            node = new_node_binop(ND_LE, add(), node);
        } else {
            return node;
        }
    }
}

Node* add() {
    Node* node = mul();
    for (;;) {
        if (consume_punct("+")) {
            node = new_node_binop(ND_ADD, node, mul());
        } else if (consume_punct("-")) {
            node = new_node_binop(ND_SUB, node, mul());
        } else {
            return node;
        }
    }
}

Node* mul() {
    Node* node = unary();
    for (;;) {
        if (consume_punct("*")) {
            node = new_node_binop(ND_MUL, node, unary());
        } else if (consume_punct("/")) {
            node = new_node_binop(ND_DIV, node, unary());
        } else {
            return node;
        }
    }
}

Node* unary() {
    if (consume_punct("+")) {
        return primary();
    }
    if (consume_punct("-")) {
        return new_node_binop(ND_SUB, new_node_num(0), primary());
    }
    return primary();
}

Node* primary() {
    if (consume_punct("(")) {
        Node* node = expr();
        expect_punct(")");
        return node;
    }
    Token* tok = consume_token(TK_IDENT);
    if (tok) {
        if (consume_punct("(")) {
            Node* node = calloc(1, sizeof(Node));
            node->kind = ND_CALL;
            node->function_name = tok->str;
            node->function_len = tok->len;
            if (consume_punct(")")) {
                return node;
            }
            Node* e = expr();
            Node* args = e;
            node->function_arg = args;
            while (!consume_punct(")")) {
                expect_punct(",");
                e = expr();
                args->function_arg_next = e;
                args = e;
            }
            return node;
        }
        Node* node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;
        LVar* lvar = find_lvar(cur_func->locals, tok->str, tok->len);
        if (lvar) {
            node->offset = lvar->offset;
        } else {
            lvar = calloc(1, sizeof(LVar));
            lvar->next = cur_func->locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset = cur_func->offset + 8;
            node->offset = lvar->offset;
            cur_func->locals = lvar;
            cur_func->offset += 8;
        }
        return node;
    }
    return new_node_num(expect_number());
}
