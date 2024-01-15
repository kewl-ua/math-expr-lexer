#ifndef LEX_H
#define LEX_H

#include <stdbool.h>

#define ZERO_CODE 48
#define NINE_CODE 57
#define LOWER_A_CODE 97
#define LOWER_Z_CODE 122
#define UPPER_A_CODE 65
#define UPPER_Z_CODE 90

typedef enum { ID, OPERATOR, NUMBER, SPACE } TokenType;

// Predicates
bool isNumber(char);
bool isAlpha(char);
bool isDot(char);
bool isSpace(char);

// Transformers
int chToInt(char);

// Parsers
double parseNumber(char*);

#endif
