#include "9cc.h"

Token *token;
char* user_input;

int dump_tokens() {
    while (token->kind != TK_EOF) {
        printf("%.*s\t%d\n", token->len, token->str, token->kind);
        token = token->next;
    }
    return 0;
}

int main(int argc, char** argv) {

	if (argc != 2) {
		fprintf(stderr, "invalid number of arguments\n");
		return 1;
	}

    user_input = argv[1];
	token = tokenize();
    program();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", 8 * 26);

    for (int i = 0; code[i]; i++) {
        gen(code[i]);
        printf("  pop rax\n");
    }

    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");

    return 0;

}