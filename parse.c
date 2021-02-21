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

void program(void) {
    int i = 0;
    while (!at_eof()) {;
        code[i++] = funcdef();
    }
    code[i] = NULL;
}

Type* typename(void) {
    if (consume_punct("int")) {
        Type* type = calloc(1, sizeof(Type));
        type->type = TP_INT;
        while (consume_punct("*")) {
            Type* t = calloc(1, sizeof(Type));
            t->type = TP_PTR;
            t->ptr_to = type;
            type = t;
        }
        return type;
    }
    return NULL;
}

Func* funcdef(void) {
    Type* return_type = typename();
    if (!return_type) {
        error("syntax error");
    }
    Func* func = calloc(1, sizeof(Func));
    cur_func = func;
    Token* ident = expect_token(TK_IDENT);
    func->name = ident->str;
    func->len = ident->len;
    func->return_type = return_type;

    expect_punct("(");

    if (!consume_punct(")")) {
        
        Type* argtype = typename();
        Token* tok = expect_token(TK_IDENT);
        LVar* arg = calloc(1, sizeof(LVar));
        
        arg->name = tok->str;
        arg->len = tok->len;
        arg->type = argtype;
        arg->offset = func->offset + 8;
        
        func->offset += 8;
        func->args = arg;

        LVar* cur = arg;

        while (!consume_punct(")")) {
            expect_punct(",");
            Type* type = typename();
            tok = expect_token(TK_IDENT);
            arg = calloc(1, sizeof(LVar));
            arg->name = tok->str;
            arg->len = tok->len;
            arg->type = type;
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

Node* stmt(void) {
    Node* node;
    Type* type;
    if (type = typename()) {
        Token* ident = expect_token(TK_IDENT);
        expect_punct(";");
        LVar* lvar = find_lvar(cur_func->locals, ident->str, ident->len);
        if (lvar) {
            error_at(ident->str, "%.*s is already declared", ident->len, ident->str);
        } else {
            lvar = calloc(1, sizeof(LVar));
            lvar->next = cur_func->locals;
            lvar->name = ident->str;
            lvar->len = ident->len;
            lvar->type = type;
            lvar->offset = cur_func->offset + 8;
            cur_func->locals = lvar;
            cur_func->offset += 8;
        }
        node = calloc(1, sizeof(Node));
        node->kind = ND_NOP;
        return node;
    }
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
        if (!consume_punct(";")) {
            init = expr();
            expect_punct(";");
        } else {
            init = new_node_num(1);
        }
        if (!consume_punct(";")) {
            cond = expr();
            expect_punct(";");
        } else {
            cond = new_node_num(1);
        }
        if (!consume_punct(")")) {
            tick = expr();
            expect_punct(")");
        } else {
            tick = new_node_num(1);
        }
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

Node* expr(void) {
    Node* node = assign();
    return node;
}

Node* assign(void) {
    Node* node = equality();
    if (consume_punct("=")) {
        node = new_node_binop(ND_ASSIGN, node, assign());
    }
    return node;
}

Node* equality(void) {
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

Node* relational(void) {
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

Node* add(void) {
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

Node* mul(void) {
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

Node* unary(void) {
    if (consume_punct("+")) {
        return primary();
    }
    if (consume_punct("-")) {
        return new_node_binop(ND_SUB, new_node_num(0), primary());
    }
    if (consume_punct("*")) {
        Node* node = calloc(1, sizeof(Node));
        node->kind = ND_DEREF;
        node->lhs = unary();
        return node;
    }
    if (consume_punct("&")) {
        Node* node = calloc(1, sizeof(Node));
        node->kind = ND_ADDR;
        node->lhs = unary();
        return node;
    }
    if (consume_punct("sizeof")) {
        Node* lhs = unary();
        Type* type = get_type(lhs);
        if (type->type == TP_INT) {
            return new_node_num(4);
        }
        if (type->type == TP_PTR) {
            return new_node_num(8);
        }
    }
    return primary();
}

Node* primary(void) {
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
            node->lvar = lvar;
        } else {
            error_at(tok->str, "%.*s is not declared yet", tok->len, tok->str);
        }
        return node;
    }
    return new_node_num(expect_number());
}

Type* get_type(Node* node) {
    Type* t;
    switch (node->kind) {
        case ND_ADD: {
            Type* lhs_type = get_type(node->lhs);
            Type* rhs_type = get_type(node->rhs);
            t = calloc(1, sizeof(Type));
            if (lhs_type->type == TP_INT && rhs_type->type == TP_INT) {
                t->type = TP_INT;
            }
            if (lhs_type->type == TP_PTR && rhs_type->type == TP_INT) {
                t->type = TP_PTR;
            }
            if (lhs_type->type == TP_INT && rhs_type->type == TP_PTR) {
                t->type = TP_PTR;
            }
            return t;
        }
        case ND_SUB: {
            Type* lhs_type = get_type(node->lhs);
            Type* rhs_type = get_type(node->rhs);
            t = calloc(1, sizeof(Type));
            if (lhs_type->type == TP_INT && rhs_type->type == TP_INT) {
                t->type = TP_INT;
            }
            if (lhs_type->type == TP_PTR && rhs_type->type == TP_INT) {
                t->type = TP_PTR;
            }
            if (lhs_type->type == TP_PTR && rhs_type->type == TP_PTR) {
                t->type = TP_INT;
            }
            return t;
        }
        case ND_MUL:
        case ND_DIV:
        case ND_EQ:
        case ND_NE:
        case ND_LT:
        case ND_LE:
        case ND_NUM:
        case ND_CALL:
            t = calloc(1, sizeof(Type));
            t->type = TP_INT;
            return t;
        case ND_ASSIGN:
            return get_type(node->rhs);
        case ND_LVAR:
            return node->lvar->type;
        case ND_DEREF:
            return node->lhs->lvar->type->ptr_to;
        case ND_ADDR:
            t = calloc(1, sizeof(Type));
            t->type = TP_PTR;
            t->ptr_to = get_type(node->lhs);
            return t;
        case ND_RETURN:
        case ND_IF:
        case ND_FOR:
        case ND_BLOCK:
        case ND_NOP:
            return NULL;
    }
}
