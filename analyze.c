#include "succ.h"

void analyze(void) {
    for (int i = 0; code[i]; i++) {
        for (int j = 0; j < i; j++) {
            if (code[i]->len == code[j]->len && strncmp(code[i]->name, code[j]->name, code[i]->len) == 0) {
                error("function `%.*s` is already declared", code[i]->len, code[i]->name);
            };
        }
    }
    for (int i = 0; code[i]; i++) {
        analyze_func(code[i]);
    }
}

void analyze_func(Func* func) {
    func->locals = func->args;
    for (Node* stmt = func->body->block_stmts; stmt; stmt = stmt->block_next) {
        analyze_node(stmt, func);
    }
}

void analyze_node(Node* node, Func* func) {
    switch (node->kind) {
        case ND_ADD:
        case ND_SUB:
        case ND_MUL:
        case ND_DIV:
        case ND_EQ:
        case ND_NE:
        case ND_LT:
        case ND_LE:
        case ND_ASSIGN:
            analyze_node(node->lhs, func);
            analyze_node(node->rhs, func);
            break;
        case ND_LVAR: {
            LVar* lvar = find_lvar(func->locals, node->var_name, node->var_name_len);
            if (lvar) {
                node->lvar = lvar;
            } else {
                error("%.*s is not declared yet", node->var_name, node->var_name_len);
            }
            break;
        }
        case ND_NUM:
            break;
        case ND_RETURN:
            analyze_node(node->return_body, func);
            break;
        case ND_IF:
            analyze_node(node->if_cond, func);
            analyze_node(node->if_then, func);
            if (node->if_else) {
                analyze_node(node->if_else, func);
            }
            break;
        case ND_FOR:
            analyze_node(node->for_init, func);
            analyze_node(node->for_cond, func);
            analyze_node(node->for_tick, func);
            analyze_node(node->for_body, func);
            break;
        case ND_BLOCK:
            for (Node* stmt = node->block_stmts; stmt; stmt = stmt->block_next) {
                analyze_node(stmt, func);
            }
            break;
        case ND_CALL:
            for (Node* arg = node->function_arg; arg; arg = arg->function_arg_next) {
                analyze_node(arg, func);
            }
            break;
        case ND_DEREF:
        case ND_ADDR:
            analyze_node(node->lhs, func);
            break;
        case ND_NOP:
            break;
        case ND_VARDEC: {
            LVar* lvar = find_lvar(func->locals, node->var_name, node->var_name_len);
            if (lvar) {
                error("%.*s is already declared", lvar->len, lvar->name);
            } else {
                lvar = calloc(1, sizeof(LVar));
                lvar->next = func->locals;
                lvar->name = node->var_name;
                lvar->len = node->var_name_len;
                lvar->type = node->var_type;
                func->locals = lvar;
            }
            break;
        }
        case ND_SIZEOF: {
            analyze_node(node->lhs, func);
            Type* type = get_type(node->lhs);
            int size;
            if (type->type == TP_INT) {
                size = 4;
            }
            if (type->type == TP_PTR) {
                size = 8;
            }
            node->kind = ND_NUM;
            node->val = size;
            break;
        }
    }
}

LVar* find_lvar(LVar* locals, char* name, int len) {
    for (LVar* var = locals; var; var = var->next) {
        if (var->len == len && strncmp(var->name, name, len) == 0) {
            return var;
        }
    }
    return NULL;
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
        case ND_SIZEOF:
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
        case ND_VARDEC:
            return NULL;
    }
    return NULL;
}