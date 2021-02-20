#include "succ.h"

int label_count = 0;

void gen(void) {

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");

    for (int i = 0; code[i]; i++) {
        gen_func(code[i]);
    }

}

void gen_func(Func* func) {

    printf("%.*s:\n", func->len, func->name);

    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", func->offset);

    LVar* arg = func->args;
    int arg_count = 0;
    while (arg && arg_count < 6) {

        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", arg->offset);

        switch (arg_count) {
            case 0:
                printf("  mov [rax], rdi\n");
                break;
            case 1:
                printf("  mov [rax], rsi\n");
                break;
            case 2:
                printf("  mov [rax], rdx\n");
                break;
            case 3:
                printf("  mov [rax], rcx\n");
                break;
            case 4:
                printf("  mov [rax], r8\n");
                break;
            case 5:
                printf("  mov [rax], r9\n");
                break;
        }

        arg = arg->next;
        arg_count++;
        
    }

    gen_node(func->body);

    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");

}

void gen_node(Node* node) {

    switch (node->kind) {
        case ND_NOP:
            return;
        case ND_ADD:
        case ND_SUB:
        case ND_MUL:
        case ND_DIV:
        case ND_EQ:
        case ND_NE:
        case ND_LT:
        case ND_LE:
            gen_binop(node);
            return;
        case ND_ASSIGN:
            gen_lval(node->lhs);
            gen_node(node->rhs);
            printf("  pop rdi\n");
            printf("  pop rax\n");
            printf("  mov [rax], rdi\n");
            printf("  push rdi\n");
            return;
        case ND_LVAR:
            gen_lval(node);
            printf("  pop rax\n");
            printf("  mov rax, [rax]\n");
            printf("  push rax\n");
            return;
        case ND_NUM:
            printf("  push %d\n", node->val);
            return;
        case ND_RETURN:
            gen_node(node->return_body);
            printf("  pop rax\n");
            printf("  mov rsp, rbp\n");
            printf("  pop rbp\n");
            printf("  ret\n");
            return;
        case ND_IF:
            gen_node(node->if_cond);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lelse%d\n", label_count);
            gen_node(node->if_then);
            printf("  jmp .Lend%d\n", label_count);
            printf(".Lelse%d:\n", label_count);
            if (node->if_else) {
                gen_node(node->if_else);
            }
            printf(".Lend%d:\n", label_count);
            label_count++;
            return;
        case ND_FOR:
            gen_node(node->for_init);
            printf(".Lbegin%d:\n", label_count);
            gen_node(node->for_cond);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lend%d\n", label_count);
            gen_node(node->for_body);
            gen_node(node->for_tick);
            printf("  jmp .Lbegin%d\n", label_count);
            printf(".Lend%d:\n", label_count);
            label_count++;
            return;
        case ND_BLOCK: {
            Node *cur = node->block_stmts;
            while (cur) {
                gen_node(cur);
                cur = cur->block_next;
            }
            return;
        }
        case ND_CALL:
            if (node->function_arg != NULL) {
                Node* arg = node->function_arg;
                int arg_count = 0;
                while (arg) {
                    if (arg_count >= 6) {
                        break;
                    }
                    gen_node(arg);
                    printf("  pop rax\n");
                    switch (arg_count) {
                        case 0:
                            printf("  mov rdi, rax\n");
                            break;
                        case 1:
                            printf("  mov rsi, rax\n");
                            break;
                        case 2:
                            printf("  mov rdx, rax\n");
                            break;
                        case 3:
                            printf("  mov rcx, rax\n");
                            break;
                        case 4:
                            printf("  mov r8, rax\n");
                            break;
                        case 5:
                            printf("  mov r9, rax\n");
                            break;
                    }
                    arg = arg->function_arg_next;
                    arg_count++;
                }
            }
            printf("  call %.*s\n", node->function_len, node->function_name);
            printf("  push rax\n");
            return;
        case ND_DEREF:
            gen_node(node->lhs);
            printf("  pop rax\n");
            printf("  mov rax, [rax]\n");
            printf("  push rax\n");
            return;
        case ND_ADDR:
            gen_lval(node->lhs);
            return;
    }

}

void gen_binop(Node* node) {

    gen_node(node->lhs);
    gen_node(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
        case ND_ADD: {
            Type* lhs_type = get_type(node->lhs);
            Type* rhs_type = get_type(node->rhs);
            // int + int
            if (lhs_type->type == TP_INT && rhs_type->type == TP_INT) {
                printf("  add rax, rdi\n");
                break;
            }
            // int + ptr
            if (lhs_type->type == TP_INT && rhs_type->type == TP_PTR) {
                if (rhs_type->ptr_to->type == TP_INT) {
                    printf("  imul rax, 4\n");
                }
                if (rhs_type->ptr_to->type == TP_PTR) {
                    printf("  imul rax, 8\n");
                }
                printf("  add rax, rdi\n");
                break;
            }
            // ptr + int
            if (lhs_type->type == TP_PTR && rhs_type->type == TP_INT) {
                if (lhs_type->ptr_to->type == TP_INT) {
                    printf("  imul rdi, 4\n");
                }
                if (lhs_type->ptr_to->type == TP_PTR) {
                    printf("  imul rdi, 8\n");
                }
                printf("  add rax, rdi\n");
                break;
            }
        }
        case ND_SUB:
            printf("  sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("  imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("  cqo\n");
            printf("  idiv rdi\n");
            break;
        case ND_EQ:
            printf("  cmp rax, rdi\n");
            printf("  sete al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_NE:
            printf("  cmp rax, rdi\n");
            printf("  setne al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LT:
            printf("  cmp rax, rdi\n");
            printf("  setl al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LE:
            printf("  cmp rax, rdi\n");
            printf("  setle al\n");
            printf("  movzb rax, al\n");
            break;
    }

    printf("  push rax\n");

}

void gen_lval(Node* node) {
    if (node->kind == ND_LVAR) {
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", node->lvar->offset);
        printf("  push rax\n");
        return;
    }
    if (node->kind == ND_DEREF) {
        gen_node(node->lhs);
        return;
    }
    error("assignment to non-variable");
}