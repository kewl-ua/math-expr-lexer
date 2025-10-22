#ifndef MATH_EXPR_EVALUATOR_H
#define MATH_EXPR_EVALUATOR_H

#include "math_expr/lexer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file evaluator.h
 * Expression evaluation helpers built on top of the lexer tokens.
 */

/**
 * Evaluate the given expression string.
 *
 * @param expression Null-terminated UTF-8 expression string.
 * @param out_result Output pointer that receives the computed value on success.
 * @return 0 on success, non-zero on failure.
 */
int math_expr_evaluate(const char *expression, double *out_result);

/**
 * Evaluate a pre-tokenised expression.
 *
 * @param tokens Token array produced by the lexer.
 * @param out_result Output pointer that receives the computed value on success.
 * @return 0 on success, non-zero on failure.
 */
int math_expr_evaluate_tokens(const math_expr_token_array *tokens, double *out_result);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MATH_EXPR_EVALUATOR_H
