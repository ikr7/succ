#include "9cc.h"

Token *token;
char* user_input;

int main(int argc, char** argv) {

	if (argc != 2) {
		fprintf(stderr, "invalid number of arguments\n");
		return 1;
	}

    user_input = argv[1];
	token = tokenize();
    Node* node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    gen(node);

    printf("  pop rax\n");
    printf("  ret\n");

    return 0;

}