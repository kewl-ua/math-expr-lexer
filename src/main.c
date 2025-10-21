#include <stdio.h>

#include "lex.h"

static void printTokens(const TokenList *tokens) {
    for (size_t i = 0; i < tokens->count; ++i) {
        const Token *token = &tokens->data[i];

        switch (token->type) {
            case NUMBER:
                printf("%-8s -> %-10s (value = %g)\n", tokenTypeToString(token->type), token->lexeme, token->number);
                break;
            case SPACE:
                printf("%-8s -> [space]\n", tokenTypeToString(token->type));
                break;
            default:
                printf("%-8s -> %s\n", tokenTypeToString(token->type), token->lexeme);
                break;
        }
    }
}

int main(void) {
    const char *expression = "3.1415 * radius^2 + sin(theta / 2)";

    TokenList tokens = lexExpression(expression);

    printf("Expression: %s\n", expression);
    printf("Tokens:%c\n", '\n');
    printTokens(&tokens);

    freeTokenList(&tokens);

    return 0;
}
