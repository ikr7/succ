#include "succ.h"

int label_count = 0;

void gen(Node* node) {

    switch (node->kind) {
        case ND_NUM:
            printf("  push %d\n", node->val);
            return;
        case ND_LVAR:
            gen_lval(node);
            printf("  pop rax\n");
            printf("  mov rax, [rax]\n");
            printf("  push rax\n");
            return;
        case ND_ASSIGN:
            gen_lval(node->lhs);
            gen(node->rhs);
            printf("  pop rdi\n");
            printf("  pop rax\n");
            printf("  mov [rax], rdi\n");
            printf("  push rdi\n");
            return;
        case ND_RETURN:
            gen(node->lhs);
            printf("  pop rax\n");
            printf("  mov rsp, rbp\n");
            printf("  pop rbp\n");
            printf("  ret\n");
            return;
        case ND_IF:
            gen(node->if_cond);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lelse%d\n", label_count);
            gen(node->if_then);
            printf("  jmp .Lend%d\n", label_count);
            printf(".Lelse%d:\n", label_count);
            if (node->if_else) {
                gen(node->if_else);
            }
            printf(".Lend%d:\n", label_count);
            label_count++;
            return;
        case ND_FOR:
            gen(node->for_init);
            printf(".Lbegin%d:\n", label_count);
            gen(node->for_cond);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lend%d\n", label_count);
            gen(node->for_body);
            gen(node->for_tick);
            printf("  jmp .Lbegin%d\n", label_count);
            printf(".Lend%d:\n", label_count);
            label_count++;
            return;
        case ND_BLOCK:
            while (node->block_stmts) {
                gen(node->block_stmts);
                printf("  pop rax\n");
                node->block_stmts = node->block_stmts->block_next;
            }
            return;
        case ND_CALL:
            if (node->function_arg != NULL) {
                Node* arg = node->function_arg;
                int arg_count = 0;
                while (arg) {
                    if (arg_count >= 6) {
                        break;
                    }
                    gen(arg);
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
                            printf("  mov r8d, rax\n");
                            break;
                        case 5:
                            printf("  mov r9d, rax\n");
                            break;
                    }
                    arg = arg->function_arg_next;
                    arg_count++;
                }
            }
            printf("  call %.*s\n", node->function_len, node->function_name);
            printf("  push rax\n");
            return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
    case ND_ADD:
        printf("  add rax, rdi\n");
        break;
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
    if (node->kind != ND_LVAR) {
        error("assignment to non-variable");
    }
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}