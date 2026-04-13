#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libcycles/lexer.h"

// ANSI Color Codes
#define COLOR_LIGHT_RED   "\033[91m"
#define COLOR_YELLOW      "\033[33m"
#define COLOR_DARK_GREEN  "\033[32m"
#define COLOR_DARK_GREY   "\033[90m"
#define COLOR_RESET       "\033[0m"

typedef struct {
    char name[256];
    char value[256];
} Variable;

Variable memory[100];
int var_count = 0;
int operatingio_enabled = 0;

const char *global_source_start = NULL;

void error_handler(const char* type, const char* hint, const char* current_ptr) {
    // error: (Light Red) Type (Yellow)
    printf("\n%serror:%s %s%s%s\n", COLOR_LIGHT_RED, COLOR_RESET, COLOR_YELLOW, type, COLOR_RESET);

    const char *line_start = current_ptr;
    while (line_start > global_source_start && *(line_start - 1) != '\n') {
        line_start--;
    }

    printf("     ");
    const char *temp = line_start;
    while (*temp != '\n' && *temp != '\0') {
        putchar(*temp);
        temp++;
    }
    putchar('\n');

    printf("     ");
    for (const char *p = line_start; p < current_ptr - 1; p++) {
        printf(" ");
    }
    printf("^\n");

    // Hint: (Dark Green) Message (Dark Grey)
    printf("%sHint:%s %s%s%s\n", COLOR_DARK_GREEN, COLOR_RESET, COLOR_DARK_GREY, hint, COLOR_RESET);
    exit(1);
}

void set_variable(const char* name, const char* value) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(memory[i].name, name) == 0) {
            strcpy(memory[i].value, value);
            return;
        }
    }
    strcpy(memory[var_count].name, name);
    strcpy(memory[var_count].value, value);
    var_count++;
}

char* get_variable(const char* name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(memory[i].name, name) == 0) return memory[i].value;
    }
    return NULL;
}

void run_interpreter(const char *source);

void load_and_run_file(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("%serror:%s %sModule Error%s\n", COLOR_LIGHT_RED, COLOR_RESET, COLOR_YELLOW, COLOR_RESET);
        printf("%sHint:%s %sThe module file '%s' was not found.%s\n", COLOR_DARK_GREEN, COLOR_RESET, COLOR_DARK_GREY, filename, COLOR_RESET);
        return;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    char *code = malloc(size + 1);
    if (code) {
        fread(code, 1, size, f);
        code[size] = '\0';
        fclose(f);
        run_interpreter(code);
        free(code);
    }
}

void run_interpreter(const char *source) {
    global_source_start = source;
    const char *ptr = source;
    Token t;

    while ((t = get_next_token(&ptr)).type != TOKEN_EOF) {
        switch (t.type) {
            case TOKEN_IMPORT: {
                Token module = get_next_token(&ptr);
                if (module.type != TOKEN_IDENT) {
                    error_handler("Syntax Error", "Expected a module name after import.", ptr);
                }

                if (strcmp(module.value, "operatingio") == 0) {
                    operatingio_enabled = 1;
                    t = get_next_token(&ptr);
                    if (t.type != TOKEN_SEMICOLON) error_handler("Syntax Error", "check syntax: missing ';'", ptr);
                } else {
                    char fileName[540];
                    snprintf(fileName, sizeof(fileName), "%s.cycles", module.value);
                    load_and_run_file(fileName);
                    t = get_next_token(&ptr);
                }
                break;
            }

            case TOKEN_IDENT: {
                if (strcmp(t.value, "operatingio") == 0) {
                    if (!operatingio_enabled) {
                        error_handler("Module Error", "Module 'operatingio' used but not imported.", ptr);
                    }
                    get_next_token(&ptr);
                    Token func = get_next_token(&ptr);

                    if (strcmp(func.value, "createFile") == 0 || strcmp(func.value, "runProcess") == 0) {
                        if (get_next_token(&ptr).type != TOKEN_LPAREN) error_handler("Syntax Error", "check syntax: missing '('", ptr);
                        Token arg = get_next_token(&ptr);

                        if (strcmp(func.value, "createFile") == 0) {
                            FILE *f = fopen(arg.value, "w");
                            if (f) { fprintf(f, "// Cycles IO\n"); fclose(f); }
                        } else {
                            system(arg.value);
                        }

                        if (get_next_token(&ptr).type != TOKEN_RPAREN) error_handler("Syntax Error", "check syntax: missing ')'", ptr);
                    } else {
                        error_handler("No Such Member", "You typed a valid module but the module had invalid member why not check the function on the module", ptr);
                    }
                }
                break;
            }

            case TOKEN_VAR:
            case TOKEN_STRING_KW: {
                Token name = get_next_token(&ptr);
                Token op = get_next_token(&ptr);
                if (op.type == TOKEN_ASSIGN) {
                    Token val = get_next_token(&ptr);
                    if (val.type != TOKEN_STRING && val.type != TOKEN_IDENT) {
                        error_handler("Syntax Error", "Expected value after '='", ptr);
                    }
                    set_variable(name.value, val.value);
                }
                break;
            }

            case TOKEN_INPUT: {
                get_next_token(&ptr);
                Token varName = get_next_token(&ptr);
                char buffer[256];
                if (fgets(buffer, sizeof(buffer), stdin)) {
                    buffer[strcspn(buffer, "\n")] = 0;
                    set_variable(varName.value, buffer);
                }
                get_next_token(&ptr);
                break;
            }

            case TOKEN_PRINTLN: {
                if (get_next_token(&ptr).type != TOKEN_LPAREN) error_handler("Syntax Error", "check syntax: missing '('", ptr);
                t = get_next_token(&ptr);
                if (t.type == TOKEN_STRING) printf("%s\n", t.value);
                else if (t.type == TOKEN_IDENT) {
                    char* val = get_variable(t.value);
                    if (val) printf("%s\n", val);
                    else error_handler("Syntax Error", "Undefined variable usage.", ptr);
                }
                if (get_next_token(&ptr).type != TOKEN_RPAREN) error_handler("Syntax Error", "check syntax: missing ')'", ptr);
                break;
            }

            case TOKEN_HOLD: getchar(); break;
            case TOKEN_UNKNOWN: error_handler("Syntax Error", "check syntax: unrecognized token", ptr); break;
            default: break;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("%serror:%s %sNo Input%s\n", COLOR_LIGHT_RED, COLOR_RESET, COLOR_YELLOW, COLOR_RESET);
        printf("%sHint:%s %sPlease provide a .cycles file to run.%s\n", COLOR_DARK_GREEN, COLOR_RESET, COLOR_DARK_GREY, COLOR_RESET);
        return 1;
    }
    load_and_run_file(argv[1]);
    return 0;
}
