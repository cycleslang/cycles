#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    TOKEN_IMPORT,   // import
    TOKEN_EXTENDS,  // extends
    TOKEN_PRINTLN,  // println
    TOKEN_VOID,     // void
    TOKEN_STRING_KW,// string (keyword)
    TOKEN_VAR,      // var
    TOKEN_HOLD,     // hold
    TOKEN_IF,       // if
    TOKEN_WHILE,    // while
    TOKEN_INPUT,    // input
    TOKEN_IDENT,    // identifiers
    TOKEN_STRING,   // "text" (literal)
    TOKEN_LBRACE,   // {
    TOKEN_RBRACE,   // }
    TOKEN_LPAREN,   // (
    TOKEN_RPAREN,   // )
    TOKEN_DOT,      // .
    TOKEN_SEMICOLON,// ;
    TOKEN_ASSIGN,   // =
    TOKEN_EOF,
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char value[256];
} Token;

Token get_next_token(const char **src) {
    // Skip whitespace and comments (//)
    while (**src != '\0') {
        if (isspace(**src)) {
            (*src)++;
        } else if (**src == '/' && *(*src + 1) == '/') {
            // Found a comment, skip to the end of the line
            while (**src != '\n' && **src != '\0') {
                (*src)++;
            }
        } else {
            // Not whitespace or a comment, start lexing
            break;
        }
    }

    Token token;
    memset(token.value, 0, 256);

    if (**src == '\0') {
        token.type = TOKEN_EOF;
        return token;
    }

    // Handle String Literals
    if (**src == '"') {
        token.type = TOKEN_STRING;
        (*src)++; // Skip opening quote
        int i = 0;
        while (**src != '"' && **src != '\0') {
            if (i < 255) {
                token.value[i++] = *((*src)++);
            } else {
                (*src)++; // Prevent buffer overflow
            }
        }
        if (**src == '"') (*src)++; // Skip closing quote
        return token;
    }

    // Handle Keywords and Identifiers
    if (isalpha(**src) || **src == '_') {
        int i = 0;
        while (isalnum(**src) || **src == '_') {
            if (i < 255) {
                token.value[i++] = *((*src)++);
            } else {
                (*src)++;
            }
        }

        if (strcmp(token.value, "import") == 0) token.type = TOKEN_IMPORT;
        else if (strcmp(token.value, "extends") == 0) token.type = TOKEN_EXTENDS;
        else if (strcmp(token.value, "println") == 0) token.type = TOKEN_PRINTLN;
        else if (strcmp(token.value, "input") == 0) token.type = TOKEN_INPUT;
        else if (strcmp(token.value, "void") == 0) token.type = TOKEN_VOID;
        else if (strcmp(token.value, "string") == 0) token.type = TOKEN_STRING_KW;
        else if (strcmp(token.value, "var") == 0) token.type = TOKEN_VAR;
        else if (strcmp(token.value, "hold") == 0) token.type = TOKEN_HOLD;
        else if (strcmp(token.value, "if") == 0) token.type = TOKEN_IF;
        else if (strcmp(token.value, "while") == 0) token.type = TOKEN_WHILE;
        else token.type = TOKEN_IDENT;
        return token;
    }

    // Handle Individual Characters/Symbols
    char c = *((*src)++);
    token.value[0] = c;
    if (c == '{') token.type = TOKEN_LBRACE;
    else if (c == '}') token.type = TOKEN_RBRACE;
    else if (c == '(') token.type = TOKEN_LPAREN;
    else if (c == ')') token.type = TOKEN_RPAREN;
    else if (c == '.') token.type = TOKEN_DOT;
    else if (c == ';') token.type = TOKEN_SEMICOLON;
    else if (c == '=') token.type = TOKEN_ASSIGN;
    else token.type = TOKEN_UNKNOWN;

    return token;
}

#endif
