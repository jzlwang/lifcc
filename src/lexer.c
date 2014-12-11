#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"
#include "util/util.h"
#include "parser.h"

#define INIT_BUFFER_SIZE 256
#define KEYWORDS_LENGTH 19

/** An array of the different string values of keywords. */
char *key_words[] = {"and", "or", "+", "-", "*", "/", "lt", "eq", 
            "function", "struct", "arrow", "assign", "if", 
            "while", "for", "sequence", "intprint", "stringprint",
            "readint"};

void init_lex(lexer *luthor) {
    luthor->file = NULL;
    luthor->buffer = NULL;
    luthor->type = token_SENTINEL;
    luthor->buff_len = 0;
}

void open_file(lexer *lex, char *filename) {
    if (lex) {
    lex->file = fopen(filename, "r");
    if (!lex->file) {
        fatal_error("Could not read input file.\n");
    }
    lex->buff_len = INIT_BUFFER_SIZE;
    lex->buffer = safe_calloc(INIT_BUFFER_SIZE * sizeof(char));
    }
}

void close_file(lexer *lex) {
    if (lex) {
        fclose(lex->file);
        free(lex->buffer);
        lex->buff_len = 0;
        lex->buffer = NULL;
    }
}

void read_token(lexer *lex) {
    /* TODO: Implement me. */
    /* HINT: fgetc() and ungetc() could be pretty useful here. */
    char c = fgetc(lex->file); //get next char
    if (c == EOF) { //if we reach the end of the file
        char* temp = malloc(sizeof(c) + 1);
        temp[0] = 32;
        temp[1] = '\0';
        lex->buffer = temp;
        lex->type = token_END;
        lex->buff_len = sizeof(c);
    }
    else if (c == ' ' || c == '\n') { //ignore spaces
        read_token(lex);
    }
    else if (c == '(') { //if token is open paren
        lex->buffer = (char *) malloc(2);
        char token[] = "(\0";
        lex->buffer = token;
        lex->type = token_OPEN_PAREN;
        lex->buff_len = 1;
    }
    else if (c == ')') { //if token is close paren
        lex->buffer = (char *) malloc(2);
        char token[] = ")";
        lex->buffer = token;
        lex->type = token_CLOSE_PAREN;
        lex->buff_len = 1;
    }
    else if (c == 34) { //if token is a string literal
            int counter = 1;
            do {
                ++counter;
                c = fgetc(lex->file);
                if (c == EOF) {
                    fprintf(stderr, "err: unexpected end of file reached");
                    exit(1);
                }
            } while (c != 34);
            fseek(lex->file, -(counter), SEEK_CUR);
            lex->buffer = (char *) malloc(counter+1);
            fgets(lex->buffer, (counter+1), lex->file);
            lex->type = token_STRING;
            lex->buff_len = counter;
    }
    else { //everything else
        int counter = 0;
        while (c != '(' && c != ')' && c != ' ' && c != '\n' && c != EOF) {
            ++counter;
            c = fgetc(lex->file);
        }
        if (c == -1) {
            fseek(lex->file, -(counter), SEEK_CUR);
        }
        else {
            fseek(lex->file, -(counter+1), SEEK_CUR);
        }
        lex->buffer = (char *) malloc(counter+1);
        fgets(lex->buffer, (counter+1), lex->file);
        if (is_keyword(lex->buffer))
            lex->type = token_KEYWORD; //set type
        else if (is_number(lex->buffer))
            lex->type = token_INT;
        else if (is_valid_name(lex->buffer)) 
            lex->type = token_NAME;
        else {
            fprintf(stderr,"err: not a valid character or name");
            exit(1);
        }
        lex->buff_len = counter;
    }
}

int is_keyword(char *buffer) {
    for (int i = 0; i < KEYWORDS_LENGTH; ++i) {
        if (!strcmp(buffer, key_words[i]))
            return 1;
    }
    return 0;
}

int is_number(char *buffer) {
    char* nums = "0123456789";
    for (int i = 0; buffer[i] != '\0'; ++i) {
        if (!strchr(nums, buffer[i])) {
            return 0;
        }
    }
    return 1;
}

int is_valid_name(char *buffer) {
    char* chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    for (int i = 0; buffer[i] != '\0'; ++i) {
        if (!strchr(chars, buffer[i])) {
            return 0;
        }
    }
    return 1;
}

token_type peek_type(lexer *lex) {
    if (!lex) {
    return token_SENTINEL;
    }
    if (lex->type == token_SENTINEL) {
    read_token(lex);
    }
    return lex->type;
}

char *peek_value(lexer *lex) {
    if (!lex) {
    return NULL;
    }
    if (lex->type == token_SENTINEL) {
    read_token(lex);
    }
    return lex->buffer;
}

/*int main(int argc, char* argv[]){
    lexer *lex1 = malloc(sizeof(lexer));
    init_lex(lex1);
    open_file(lex1,argv[1]);
    read_token(lex1);
    read_token(lex1);
    read_token(lex1);
    read_token(lex1);
    read_token(lex1);
}*/