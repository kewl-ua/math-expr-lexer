#include <math.h>
#include <stdio.h>
#include "lex.h"

#define DEBUG 1

// Predicates
bool isNumber(char c) {
    return c >= ZERO_CODE && c <= NINE_CODE;
}

bool isAlpha(char c) {
    return c >= UPPER_A_CODE && c <= UPPER_Z_CODE || c >= LOWER_A_CODE && c <= LOWER_Z_CODE;
}

bool isDot(char c) {
    return c == '.';
}

bool isSpace(char c) {
    return c == ' ';
}

// Transformers
int chToInt(char c) {
    return c - ZERO_CODE;
}

// Parsers
double parseNumber(char *nStr) {
    typedef enum { PARSING_INT, DOT, PARSING_DOUBLE } State;

    int iPart = 0;
    double dPart = 0;
    unsigned short int iCounter, dCounter = 0;
    char buff;

    State state = PARSING_INT;

    while ((buff = *(nStr++)) != '\0') {
        if (isDot(buff)) {
            state = DOT;
        }

        switch (state) {
            case PARSING_INT:
                if (isNumber(buff)) {
                    iCounter++;
                    iPart += chToInt(buff);
                } 
                break;
            case DOT:
                state = PARSING_DOUBLE;
                break;
            case PARSING_DOUBLE:
                if (isNumber(buff)) {
                    dPart += chToInt(buff) * pow(10, -1 * dCounter++); 
                }
                break;
        }
    }

    return iPart + dPart;
}
