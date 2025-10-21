#ifndef MATH_EXPR_LEXER_H
#define MATH_EXPR_LEXER_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file lexer.h
 * Public interface for the math expression lexer.
 */

typedef enum TokenType {
    TOKEN_IDENTIFIER,
    TOKEN_OPERATOR,
    TOKEN_NUMBER,
    TOKEN_SPACE
} TokenType;

typedef struct Token {
    TokenType type;
    char *lexeme;
    double number;
} Token;

typedef struct TokenList {
    Token *data;
    size_t size;
    size_t capacity;
} TokenList;

void token_list_init(TokenList *list);
void token_list_free(TokenList *list);

TokenList lex_expression(const char *expression);
const char *token_type_to_string(TokenType type);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MATH_EXPR_LEXER_H
