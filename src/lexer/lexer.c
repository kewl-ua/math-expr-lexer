#include "math_expr/lexer.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const size_t kInitialTokenCapacity = 16U;

static int is_identifier_start(int c)
{
    return isalpha(c) || c == '_';
}

static int is_identifier_part(int c)
{
    return is_identifier_start(c) || isdigit(c);
}

static int is_operator_char(int c)
{
    switch (c) {
    case '+':
    case '-':
    case '*':
    case '/':
    case '^':
    case '%':
    case '=':
    case '(':
    case ')':
    case ',':
        return 1;
    default:
        return 0;
    }
}

static int is_space_char(int c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

static void token_list_reserve(TokenList *list)
{
    if (!list) {
        return;
    }

    if (list->size < list->capacity) {
        return;
    }

    size_t new_capacity = list->capacity == 0 ? kInitialTokenCapacity : list->capacity * 2U;
    Token *new_data = (Token *)realloc(list->data, new_capacity * sizeof(*list->data));
    if (!new_data) {
        perror("math_expr_lexer: realloc");
        return;
    }

    list->data = new_data;
    list->capacity = new_capacity;
}

static char *copy_range(const char *start, size_t length)
{
    char *buffer = (char *)malloc(length + 1U);
    if (!buffer) {
        perror("math_expr_lexer: malloc");
        return NULL;
    }

    if (length > 0U) {
        memcpy(buffer, start, length);
    }
    buffer[length] = '\0';
    return buffer;
}

static void token_list_append(TokenList *list, TokenType type, const char *start, size_t length, double value)
{
    if (!list) {
        return;
    }

    token_list_reserve(list);
    if (list->capacity == 0U || list->size >= list->capacity) {
        return;
    }

    char *lexeme = copy_range(start, length);
    if (!lexeme) {
        return;
    }

    Token *token = &list->data[list->size++];
    token->type = type;
    token->lexeme = lexeme;
    token->number = value;
}

static void append_space(TokenList *tokens, const char *start, const char *cursor)
{
    token_list_append(tokens, TOKEN_SPACE, start, (size_t)(cursor - start), 0.0);
}

static void append_number(TokenList *tokens, const char *start, char **endptr)
{
    errno = 0;
    double value = strtod(start, endptr);
    if (start == *endptr) {
        return;
    }

    if (errno != 0) {
        perror("math_expr_lexer: strtod");
    }

    token_list_append(tokens, TOKEN_NUMBER, start, (size_t)(*endptr - start), value);
}

static void append_identifier(TokenList *tokens, const char *start, const char *cursor)
{
    token_list_append(tokens, TOKEN_IDENTIFIER, start, (size_t)(cursor - start), 0.0);
}

static void append_operator(TokenList *tokens, const char *start)
{
    token_list_append(tokens, TOKEN_OPERATOR, start, 1U, 0.0);
}

void token_list_init(TokenList *list)
{
    if (!list) {
        return;
    }

    list->data = NULL;
    list->size = 0U;
    list->capacity = 0U;
}

void token_list_free(TokenList *list)
{
    if (!list) {
        return;
    }

    for (size_t i = 0; i < list->size; ++i) {
        free(list->data[i].lexeme);
        list->data[i].lexeme = NULL;
    }

    free(list->data);
    list->data = NULL;
    list->size = 0U;
    list->capacity = 0U;
}

TokenList lex_expression(const char *expression)
{
    TokenList tokens;
    token_list_init(&tokens);

    if (!expression) {
        return tokens;
    }

    const char *cursor = expression;

    while (*cursor != '\0') {
        const unsigned char current = (unsigned char)*cursor;

        if (is_space_char(current)) {
            const char *start = cursor;
            while (*cursor != '\0' && is_space_char((unsigned char)*cursor)) {
                ++cursor;
            }
            append_space(&tokens, start, cursor);
            continue;
        }

        if (isdigit(current) || (*cursor == '.' && isdigit((unsigned char)*(cursor + 1)))) {
            const char *start = cursor;
            char *endptr = NULL;
            append_number(&tokens, start, &endptr);
            if (!endptr || endptr == start) {
                ++cursor;
                continue;
            }
            cursor = endptr;
            continue;
        }

        if (is_identifier_start(current)) {
            const char *start = cursor;
            while (*cursor != '\0' && is_identifier_part((unsigned char)*cursor)) {
                ++cursor;
            }
            append_identifier(&tokens, start, cursor);
            continue;
        }

        if (is_operator_char(current)) {
            append_operator(&tokens, cursor);
            ++cursor;
            continue;
        }

        fprintf(stderr, "math_expr_lexer: unrecognized character '%c'\n", *cursor);
        ++cursor;
    }

    return tokens;
}

const char *token_type_to_string(TokenType type)
{
    switch (type) {
    case TOKEN_IDENTIFIER:
        return "IDENTIFIER";
    case TOKEN_OPERATOR:
        return "OPERATOR";
    case TOKEN_NUMBER:
        return "NUMBER";
    case TOKEN_SPACE:
        return "SPACE";
    default:
        return "UNKNOWN";
    }
}
