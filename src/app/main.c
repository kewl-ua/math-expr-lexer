#include <stdio.h>

#include "math_expr/lexer.h"

static void print_tokens(const TokenList *tokens)
{
    for (size_t i = 0; tokens && i < tokens->size; ++i) {
        const Token *token = &tokens->data[i];

        switch (token->type) {
        case TOKEN_NUMBER:
            printf("%-12s -> %-10s (value = %g)\n", token_type_to_string(token->type), token->lexeme, token->number);
            break;
        case TOKEN_SPACE:
            printf("%-12s -> [space]\n", token_type_to_string(token->type));
            break;
        default:
            printf("%-12s -> %s\n", token_type_to_string(token->type), token->lexeme);
            break;
        }
    }
}

int main(void)
{
    const char *expression = "3.1415 * radius^2 + sin(theta / 2)";

    TokenList tokens = lex_expression(expression);

    printf("Expression: %s\n\n", expression);
    printf("Tokens:\n");
    print_tokens(&tokens);

    token_list_free(&tokens);

    return 0;
}
