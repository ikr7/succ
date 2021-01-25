#include "succ.h"

Node* code[100];
LVar* locals;
int offset = 0;
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

    gen();

    return 0;

}