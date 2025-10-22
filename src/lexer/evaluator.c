#include "math_expr/evaluator.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

typedef struct parser {
    const math_expr_token_array *tokens;
    size_t index;
} parser;

static void parser_skip_spaces(parser *p)
{
    if (!p || !p->tokens) {
        return;
    }

    while (p->index < p->tokens->size &&
           p->tokens->data[p->index].type == MATH_EXPR_TOKEN_SPACE) {
        ++p->index;
    }
}

static const math_expr_token *parser_peek(parser *p)
{
    if (!p || !p->tokens) {
        return NULL;
    }

    parser_skip_spaces(p);

    if (p->index >= p->tokens->size) {
        return NULL;
    }

    return &p->tokens->data[p->index];
}

static const math_expr_token *parser_consume(parser *p)
{
    if (!p || !p->tokens) {
        return NULL;
    }

    parser_skip_spaces(p);

    if (p->index >= p->tokens->size) {
        return NULL;
    }

    return &p->tokens->data[p->index++];
}

static int parser_match_operator(parser *p, const char *lexeme)
{
    const math_expr_token *token = parser_peek(p);
    if (!token || token->type != MATH_EXPR_TOKEN_OPERATOR) {
        return 0;
    }

    if (strcmp(token->lexeme, lexeme) == 0) {
        ++p->index;
        return 1;
    }

    return 0;
}

static int parser_expect_operator(parser *p, const char *lexeme)
{
    if (parser_match_operator(p, lexeme)) {
        return 0;
    }

    fprintf(stderr, "math_expr_evaluator: expected '%s'\n", lexeme);
    return -1;
}

static int str_iequal(const char *lhs, const char *rhs)
{
    if (!lhs || !rhs) {
        return 0;
    }

    while (*lhs && *rhs) {
        if (tolower((unsigned char)*lhs) != tolower((unsigned char)*rhs)) {
            return 0;
        }
        ++lhs;
        ++rhs;
    }

    return *lhs == '\0' && *rhs == '\0';
}

static double to_radians(double degrees)
{
    return degrees * (M_PI / 180.0);
}

static double func_sin(const double *args)
{
    return sin(to_radians(args[0]));
}

static double func_cos(const double *args)
{
    return cos(to_radians(args[0]));
}

static double func_tan(const double *args)
{
    return tan(to_radians(args[0]));
}

static double func_sqrt(const double *args)
{
    return sqrt(args[0]);
}

static double func_abs(const double *args)
{
    return fabs(args[0]);
}

static double func_ln(const double *args)
{
    return log(args[0]);
}

static double func_log(const double *args)
{
    return log10(args[0]);
}

static double func_exp(const double *args)
{
    return exp(args[0]);
}

static double func_pow(const double *args)
{
    return pow(args[0], args[1]);
}

static double func_max(const double *args)
{
    return args[0] > args[1] ? args[0] : args[1];
}

static double func_min(const double *args)
{
    return args[0] < args[1] ? args[0] : args[1];
}

static int evaluate_function(const char *name, const double *args, size_t arg_count, double *out)
{
    if (!name || !out) {
        return -1;
    }

    struct function_entry {
        const char *name;
        size_t arity;
        double (*func)(const double *args);
    };

    static const struct function_entry functions[] = {
        {"sin", 1, func_sin},
        {"cos", 1, func_cos},
        {"tan", 1, func_tan},
        {"sqrt", 1, func_sqrt},
        {"abs", 1, func_abs},
        {"ln", 1, func_ln},
        {"log", 1, func_log},
        {"exp", 1, func_exp},
        {"pow", 2, func_pow},
        {"max", 2, func_max},
        {"min", 2, func_min}
    };

    for (size_t i = 0; i < sizeof(functions) / sizeof(functions[0]); ++i) {
        if (str_iequal(name, functions[i].name)) {
            if (functions[i].arity != arg_count) {
                fprintf(stderr,
                        "math_expr_evaluator: function '%s' expects %zu argument(s)\n",
                        name,
                        functions[i].arity);
                return -1;
            }

            *out = functions[i].func(args);
            return 0;
        }
    }

    fprintf(stderr, "math_expr_evaluator: unknown function '%s'\n", name);
    return -1;
}

static int lookup_constant(const char *name, double *out)
{
    if (!name || !out) {
        return -1;
    }

    struct constant_entry {
        const char *name;
        double value;
    };

    static const struct constant_entry constants[] = {
        {"pi", M_PI},
        {"e", M_E}
    };

    for (size_t i = 0; i < sizeof(constants) / sizeof(constants[0]); ++i) {
        if (str_iequal(name, constants[i].name)) {
            *out = constants[i].value;
            return 0;
        }
    }

    return -1;
}

static int parse_expression(parser *p, double *out);

static int parse_primary(parser *p, double *out)
{
    const math_expr_token *token = parser_peek(p);
    if (!token) {
        fprintf(stderr, "math_expr_evaluator: unexpected end of input\n");
        return -1;
    }

    if (token->type == MATH_EXPR_TOKEN_NUMBER) {
        parser_consume(p);
        *out = token->number;
        return 0;
    }

    if (token->type == MATH_EXPR_TOKEN_OPERATOR && strcmp(token->lexeme, "(") == 0) {
        parser_consume(p);

        if (parse_expression(p, out) != 0) {
            return -1;
        }

        if (parser_expect_operator(p, ")") != 0) {
            return -1;
        }

        return 0;
    }

    if (token->type == MATH_EXPR_TOKEN_IDENTIFIER) {
        const char *identifier = token->lexeme;
        parser_consume(p);

        if (parser_match_operator(p, "(")) {
            double *args = NULL;
            size_t arg_count = 0U;
            size_t capacity = 0U;
            int status = 0;

            const math_expr_token *next = parser_peek(p);
            if (next && !(next->type == MATH_EXPR_TOKEN_OPERATOR && strcmp(next->lexeme, ")") == 0)) {
                for (;;) {
                    double value = 0.0;
                    if (parse_expression(p, &value) != 0) {
                        status = -1;
                        break;
                    }

                    if (arg_count == capacity) {
                        size_t new_capacity = capacity == 0 ? 4U : capacity * 2U;
                        double *new_args = (double *)realloc(args, new_capacity * sizeof(*new_args));
                        if (!new_args) {
                            perror("math_expr_evaluator: realloc");
                            status = -1;
                            break;
                        }
                        args = new_args;
                        capacity = new_capacity;
                    }

                    args[arg_count++] = value;

                    if (!parser_match_operator(p, ",")) {
                        break;
                    }
                }
            }

            if (status == 0 && parser_expect_operator(p, ")") != 0) {
                status = -1;
            }

            if (status == 0) {
                status = evaluate_function(identifier, args, arg_count, out);
            }

            free(args);
            return status;
        }

        if (lookup_constant(identifier, out) == 0) {
            return 0;
        }

        fprintf(stderr, "math_expr_evaluator: unknown identifier '%s'\n", identifier);
        return -1;
    }

    fprintf(stderr, "math_expr_evaluator: unexpected token '%s'\n", token->lexeme);
    return -1;
}

static int parse_power(parser *p, double *out)
{
    if (parse_primary(p, out) != 0) {
        return -1;
    }

    const math_expr_token *token = parser_peek(p);
    if (token && token->type == MATH_EXPR_TOKEN_OPERATOR && strcmp(token->lexeme, "^") == 0) {
        parser_consume(p);
        double rhs = 0.0;
        if (parse_power(p, &rhs) != 0) {
            return -1;
        }
        *out = pow(*out, rhs);
    }

    return 0;
}

static int parse_unary(parser *p, double *out)
{
    const math_expr_token *token = parser_peek(p);
    if (token && token->type == MATH_EXPR_TOKEN_OPERATOR) {
        if (strcmp(token->lexeme, "+") == 0) {
            parser_consume(p);
            return parse_unary(p, out);
        }

        if (strcmp(token->lexeme, "-") == 0) {
            parser_consume(p);
            if (parse_unary(p, out) != 0) {
                return -1;
            }
            *out = -*out;
            return 0;
        }
    }

    return parse_power(p, out);
}

static int parse_term(parser *p, double *out)
{
    if (parse_unary(p, out) != 0) {
        return -1;
    }

    while (1) {
        const math_expr_token *token = parser_peek(p);
        if (!token || token->type != MATH_EXPR_TOKEN_OPERATOR) {
            break;
        }

        if (strcmp(token->lexeme, "*") == 0) {
            parser_consume(p);
            double rhs = 0.0;
            if (parse_unary(p, &rhs) != 0) {
                return -1;
            }
            *out *= rhs;
            continue;
        }

        if (strcmp(token->lexeme, "/") == 0) {
            parser_consume(p);
            double rhs = 0.0;
            if (parse_unary(p, &rhs) != 0) {
                return -1;
            }
            if (rhs == 0.0) {
                fprintf(stderr, "math_expr_evaluator: division by zero\n");
                return -1;
            }
            *out /= rhs;
            continue;
        }

        if (strcmp(token->lexeme, "%") == 0) {
            parser_consume(p);
            double rhs = 0.0;
            if (parse_unary(p, &rhs) != 0) {
                return -1;
            }
            if (rhs == 0.0) {
                fprintf(stderr, "math_expr_evaluator: modulo by zero\n");
                return -1;
            }
            *out = fmod(*out, rhs);
            continue;
        }

        break;
    }

    return 0;
}

static int parse_expression(parser *p, double *out)
{
    if (parse_term(p, out) != 0) {
        return -1;
    }

    while (1) {
        const math_expr_token *token = parser_peek(p);
        if (!token || token->type != MATH_EXPR_TOKEN_OPERATOR) {
            break;
        }

        if (strcmp(token->lexeme, "+") == 0) {
            parser_consume(p);
            double rhs = 0.0;
            if (parse_term(p, &rhs) != 0) {
                return -1;
            }
            *out += rhs;
            continue;
        }

        if (strcmp(token->lexeme, "-") == 0) {
            parser_consume(p);
            double rhs = 0.0;
            if (parse_term(p, &rhs) != 0) {
                return -1;
            }
            *out -= rhs;
            continue;
        }

        break;
    }

    return 0;
}

int math_expr_evaluate_tokens(const math_expr_token_array *tokens, double *out_result)
{
    if (!tokens || !out_result) {
        return -1;
    }

    parser p = {tokens, 0U};
    double value = 0.0;

    if (parse_expression(&p, &value) != 0) {
        return -1;
    }

    parser_skip_spaces(&p);
    if (p.index < tokens->size) {
        fprintf(stderr, "math_expr_evaluator: unexpected trailing tokens\n");
        return -1;
    }

    *out_result = value;
    return 0;
}

int math_expr_evaluate(const char *expression, double *out_result)
{
    if (!expression || !out_result) {
        return -1;
    }

    math_expr_token_array tokens;
    math_expr_token_array_init(&tokens);

    if (math_expr_lex_expression(expression, &tokens) != 0) {
        math_expr_token_array_deinit(&tokens);
        return -1;
    }

    int status = math_expr_evaluate_tokens(&tokens, out_result);
    math_expr_token_array_deinit(&tokens);
    return status;
}
