#include "succ.h"

Func* code[100];
Func* cur_func;
Token *token;
char* user_input;

int dump_tokens(void) {
    while (token->kind != TK_EOF) {
        printf("%.*s\t%d\n", (int)(token->len), token->str, token->kind);
        token = token->next;
    }
    return 0;
}

int main(int argc, char** argv) {

    struct option long_options[] = {
        {"dump-tokens", no_argument, NULL, 't'},
        {0, 0, 0, 0}
    };
    int opt, option_index;
    bool opt_dump_tokens = false;
    while ((opt = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
        switch (opt) {
            case 't':
                opt_dump_tokens = true;
                break;
        }
    }

    if (argc - optind != 1) {
        fprintf(stderr, "error: no source given\n");
        return 1;
    }

    user_input = argv[optind];
	token = tokenize();

    if (opt_dump_tokens) {
        dump_tokens();
        return 0;
    }

    program();
    analyze();
    gen();

    return 0;

}