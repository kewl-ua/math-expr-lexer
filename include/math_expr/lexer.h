#ifndef MATH_EXPR_LEXER_H
#define MATH_EXPR_LEXER_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file lexer.h
 * Public interface for the math expression lexer.
 */

typedef enum math_expr_token_type {
    MATH_EXPR_TOKEN_IDENTIFIER,
    MATH_EXPR_TOKEN_OPERATOR,
    MATH_EXPR_TOKEN_NUMBER,
    MATH_EXPR_TOKEN_SPACE
} math_expr_token_type;

typedef struct math_expr_token {
    math_expr_token_type type;
    char *lexeme;
    double number;
} math_expr_token;

typedef struct math_expr_token_array {
    math_expr_token *data;
    size_t size;
    size_t capacity;
} math_expr_token_array;

void math_expr_token_array_init(math_expr_token_array *array);
void math_expr_token_array_clear(math_expr_token_array *array);
void math_expr_token_array_deinit(math_expr_token_array *array);

int math_expr_lex_expression(const char *expression, math_expr_token_array *out_tokens);
const char *math_expr_token_type_to_string(math_expr_token_type type);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MATH_EXPR_LEXER_H
