#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lex.h"

#define INITIAL_TOKEN_CAPACITY 16

// Predicates
bool isNumber(char c) {
    return c >= ZERO_CODE && c <= NINE_CODE;
}

bool isAlpha(char c) {
    return (c >= UPPER_A_CODE && c <= UPPER_Z_CODE) || (c >= LOWER_A_CODE && c <= LOWER_Z_CODE);
}

bool isDot(char c) {
    return c == '.';
}

bool isSpace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool isOperator(char c) {
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
            return true;
        default:
            return false;
    }
}

static bool isIdentifierStart(char c) {
    return isAlpha(c) || c == '_';
}

static bool isIdentifierPart(char c) {
    return isIdentifierStart(c) || isNumber(c);
}

// Transformers
int chToInt(char c) {
    return c - ZERO_CODE;
}

// Parsers
double parseNumber(const char *nStr) {
    if (!nStr) {
        return 0.0;
    }

    size_t i = 0;
    int sign = 1;

    if (nStr[i] == '+') {
        i++;
    } else if (nStr[i] == '-') {
        sign = -1;
        i++;
    }

    double integerPart = 0.0;
    while (isNumber(nStr[i])) {
        integerPart = integerPart * 10.0 + chToInt(nStr[i]);
        i++;
    }

    double fractionalPart = 0.0;
    double divisor = 1.0;
    if (isDot(nStr[i])) {
        i++;
        while (isNumber(nStr[i])) {
            fractionalPart = fractionalPart * 10.0 + chToInt(nStr[i]);
            divisor *= 10.0;
            i++;
        }
    }

    double value = integerPart + (divisor != 0.0 ? fractionalPart / divisor : 0.0);

    if (nStr[i] == 'e' || nStr[i] == 'E') {
        i++;
        int expSign = 1;
        if (nStr[i] == '+') {
            i++;
        } else if (nStr[i] == '-') {
            expSign = -1;
            i++;
        }

        int exponent = 0;
        while (isNumber(nStr[i])) {
            exponent = exponent * 10 + chToInt(nStr[i]);
            i++;
        }

        value *= pow(10.0, expSign * exponent);
    }

    return sign * value;
}

static void ensureCapacity(TokenList *list) {
    if (list->count < list->capacity) {
        return;
    }

    size_t newCapacity = list->capacity == 0 ? INITIAL_TOKEN_CAPACITY : list->capacity * 2;
    Token *newData = realloc(list->data, newCapacity * sizeof(Token));
    if (!newData) {
        perror("Lexer: Memory allocation failed");
        return;
    }

    list->data = newData;
    list->capacity = newCapacity;
}

static char *copyRange(const char *start, size_t length) {
    char *buffer = malloc(length + 1);
    if (!buffer) {
        perror("Lexer: Memory allocation failed");
        return NULL;
    }

    memcpy(buffer, start, length);
    buffer[length] = '\0';
    return buffer;
}

static void appendToken(TokenList *list, TokenType type, const char *start, size_t length, double number) {
    ensureCapacity(list);
    if (list->capacity == 0 || list->count >= list->capacity) {
        return;
    }

    char *lexeme = copyRange(start, length);
    if (!lexeme) {
        return;
    }

    list->data[list->count].type = type;
    list->data[list->count].lexeme = lexeme;
    list->data[list->count].number = number;
    list->count++;
}

static double parseNumberRange(const char *start, size_t length) {
    char *buffer = copyRange(start, length);
    if (!buffer) {
        return 0.0;
    }

    double value = parseNumber(buffer);
    free(buffer);
    return value;
}

void initTokenList(TokenList *list) {
    if (!list) {
        return;
    }

    list->data = NULL;
    list->count = 0;
    list->capacity = 0;
}

void freeTokenList(TokenList *list) {
    if (!list) {
        return;
    }

    for (size_t i = 0; i < list->count; ++i) {
        free(list->data[i].lexeme);
    }

    free(list->data);
    list->data = NULL;
    list->count = 0;
    list->capacity = 0;
}

TokenList lexExpression(const char *expression) {
    TokenList tokens;
    initTokenList(&tokens);

    if (!expression) {
        return tokens;
    }

    size_t i = 0;
    size_t length = strlen(expression);

    while (i < length) {
        char current = expression[i];

        if (isSpace(current)) {
            size_t start = i;
            while (i < length && isSpace(expression[i])) {
                i++;
            }
            appendToken(&tokens, SPACE, &expression[start], i - start, 0.0);
            continue;
        }

        if (isNumber(current) || (isDot(current) && i + 1 < length && isNumber(expression[i + 1]))) {
            size_t start = i;
            bool seenDot = isDot(current);
            i++;

            while (i < length) {
                if (isNumber(expression[i])) {
                    i++;
                    continue;
                }
                if (isDot(expression[i]) && !seenDot) {
                    seenDot = true;
                    i++;
                    continue;
                }
                break;
            }

            size_t tokenLength = i - start;
            double number = parseNumberRange(&expression[start], tokenLength);
            appendToken(&tokens, NUMBER, &expression[start], tokenLength, number);
            continue;
        }

        if (isIdentifierStart(current)) {
            size_t start = i;
            i++;
            while (i < length && isIdentifierPart(expression[i])) {
                i++;
            }

            appendToken(&tokens, ID, &expression[start], i - start, 0.0);
            continue;
        }

        if (isOperator(current)) {
            appendToken(&tokens, OPERATOR, &expression[i], 1, 0.0);
            i++;
            continue;
        }

        fprintf(stderr, "Lexer: Unrecognized character '%c'\n", current);
        i++;
    }

    return tokens;
}

const char *tokenTypeToString(TokenType type) {
    switch (type) {
        case ID:
            return "ID";
        case OPERATOR:
            return "OPERATOR";
        case NUMBER:
            return "NUMBER";
        case SPACE:
            return "SPACE";
        default:
            return "UNKNOWN";
    }
}
