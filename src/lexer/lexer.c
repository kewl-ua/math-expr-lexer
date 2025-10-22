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

static void token_array_reserve(math_expr_token_array *array)
{
    if (!array) {
        return;
    }

    if (array->size < array->capacity) {
        return;
    }

    size_t new_capacity = array->capacity == 0 ? kInitialTokenCapacity : array->capacity * 2U;
    math_expr_token *new_data = (math_expr_token *)realloc(array->data, new_capacity * sizeof(*array->data));
    if (!new_data) {
        perror("math_expr_lexer: realloc");
        return;
    }

    array->data = new_data;
    array->capacity = new_capacity;
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

static int token_array_append(math_expr_token_array *array,
                             math_expr_token_type type,
                             const char *start,
                             size_t length,
                             double value)
{
    if (!array) {
        return -1;
    }

    token_array_reserve(array);
    if (array->capacity == 0U || array->size >= array->capacity) {
        return -1;
    }

    char *lexeme = copy_range(start, length);
    if (!lexeme) {
        return -1;
    }

    math_expr_token *token = &array->data[array->size++];
    token->type = type;
    token->lexeme = lexeme;
    token->number = value;
    return 0;
}

static int append_space(math_expr_token_array *tokens, const char *start, const char *cursor)
{
    return token_array_append(tokens,
                              MATH_EXPR_TOKEN_SPACE,
                              start,
                              (size_t)(cursor - start),
                              0.0);
}

static int append_number(math_expr_token_array *tokens, const char *start, char **endptr)
{
    errno = 0;
    char *local_endptr = NULL;
    double value = strtod(start, &local_endptr);

    if (endptr) {
        *endptr = local_endptr;
    }

    if (start == local_endptr) {
        return 1;
    }

    if (errno != 0) {
        perror("math_expr_lexer: strtod");
    }

    if (token_array_append(tokens,
                           MATH_EXPR_TOKEN_NUMBER,
                           start,
                           (size_t)(local_endptr - start),
                           value) != 0) {
        return -1;
    }

    return 0;
}

static int append_identifier(math_expr_token_array *tokens, const char *start, const char *cursor)
{
    return token_array_append(tokens,
                              MATH_EXPR_TOKEN_IDENTIFIER,
                              start,
                              (size_t)(cursor - start),
                              0.0);
}

static int append_operator(math_expr_token_array *tokens, const char *start)
{
    return token_array_append(tokens,
                              MATH_EXPR_TOKEN_OPERATOR,
                              start,
                              1U,
                              0.0);
}

void math_expr_token_array_init(math_expr_token_array *array)
{
    if (!array) {
        return;
    }

    array->data = NULL;
    array->size = 0U;
    array->capacity = 0U;
}

void math_expr_token_array_clear(math_expr_token_array *array)
{
    if (!array) {
        return;
    }

    for (size_t i = 0; i < array->size; ++i) {
        free(array->data[i].lexeme);
        array->data[i].lexeme = NULL;
    }

    array->size = 0U;
}

void math_expr_token_array_deinit(math_expr_token_array *array)
{
    if (!array) {
        return;
    }

    math_expr_token_array_clear(array);
    free(array->data);
    array->data = NULL;
    array->capacity = 0U;
}

int math_expr_lex_expression(const char *expression, math_expr_token_array *out_tokens)
{
    if (!out_tokens) {
        return -1;
    }

    if (out_tokens->data != NULL || out_tokens->size != 0U || out_tokens->capacity != 0U) {
        math_expr_token_array_clear(out_tokens);
    }

    if (!expression) {
        return 0;
    }

    const char *cursor = expression;

    while (*cursor != '\0') {
        const unsigned char current = (unsigned char)*cursor;

        if (is_space_char(current)) {
            const char *start = cursor;
            while (*cursor != '\0' && is_space_char((unsigned char)*cursor)) {
                ++cursor;
            }
            if (append_space(out_tokens, start, cursor) != 0) {
                goto error;
            }
            continue;
        }

        if (isdigit(current) || (*cursor == '.' && isdigit((unsigned char)*(cursor + 1)))) {
            const char *start = cursor;
            char *endptr = NULL;
            int result = append_number(out_tokens, start, &endptr);
            if (result == 1 || !endptr || endptr == start) {
                ++cursor;
                continue;
            }
            if (result != 0) {
                goto error;
            }
            cursor = endptr;
            continue;
        }

        if (is_identifier_start(current)) {
            const char *start = cursor;
            while (*cursor != '\0' && is_identifier_part((unsigned char)*cursor)) {
                ++cursor;
            }
            if (append_identifier(out_tokens, start, cursor) != 0) {
                goto error;
            }
            continue;
        }

        if (is_operator_char(current)) {
            if (append_operator(out_tokens, cursor) != 0) {
                goto error;
            }
            ++cursor;
            continue;
        }

        fprintf(stderr, "math_expr_lexer: unrecognized character '%c'\n", *cursor);
        ++cursor;
    }

    return 0;

error:
    math_expr_token_array_deinit(out_tokens);
    return -1;
}

const char *math_expr_token_type_to_string(math_expr_token_type type)
{
    switch (type) {
    case MATH_EXPR_TOKEN_IDENTIFIER:
        return "IDENTIFIER";
    case MATH_EXPR_TOKEN_OPERATOR:
        return "OPERATOR";
    case MATH_EXPR_TOKEN_NUMBER:
        return "NUMBER";
    case MATH_EXPR_TOKEN_SPACE:
        return "SPACE";
    default:
        return "UNKNOWN";
    }
}
