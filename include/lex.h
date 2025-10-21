#ifndef LEX_H
#define LEX_H

#include <stdbool.h>
#include <stddef.h>

typedef enum { ID, OPERATOR, NUMBER, SPACE } TokenType;

#define ZERO_CODE 48
#define NINE_CODE 57
#define LOWER_A_CODE 97
#define LOWER_Z_CODE 122
#define UPPER_A_CODE 65
#define UPPER_Z_CODE 90

typedef struct {
    TokenType type;
    char *lexeme;
    double number;
} Token;

typedef struct {
    Token *data;
    size_t count;
    size_t capacity;
} TokenList;

// Predicates
bool isNumber(char);
bool isAlpha(char);
bool isDot(char);
bool isSpace(char);
bool isOperator(char);

// Transformers
int chToInt(char);

// Parsers
double parseNumber(const char*);

// Lexer API
void initTokenList(TokenList*);
void freeTokenList(TokenList*);
TokenList lexExpression(const char*);
const char* tokenTypeToString(TokenType);

#endif
