#include <stdio.h>

#include "math_expr/lexer.h"

static void print_tokens(const math_expr_token_array *tokens)
{
    for (size_t i = 0; tokens && i < tokens->size; ++i) {
        const math_expr_token *token = &tokens->data[i];

        switch (token->type) {
        case MATH_EXPR_TOKEN_NUMBER:
            printf("%-12s -> %-10s (value = %g)\n",
                   math_expr_token_type_to_string(token->type),
                   token->lexeme,
                   token->number);
            break;
        case MATH_EXPR_TOKEN_SPACE:
            printf("%-12s -> [space]\n", math_expr_token_type_to_string(token->type));
            break;
        default:
            printf("%-12s -> %s\n", math_expr_token_type_to_string(token->type), token->lexeme);
            break;
        }
    }
}

int main(void)
{
    const char *expression = "3.1415 * radius^2 + sin(theta / 2)";

    math_expr_token_array tokens;
    math_expr_token_array_init(&tokens);

    printf("Expression: %s\n\n", expression);
    printf("Tokens:\n");

    if (math_expr_lex_expression(expression, &tokens) != 0) {
        fputs("Failed to lex expression.\n", stderr);
        math_expr_token_array_deinit(&tokens);
        return 1;
    }

    print_tokens(&tokens);

    math_expr_token_array_deinit(&tokens);

    return 0;
}
