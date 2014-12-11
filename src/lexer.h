#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

/** An enumeration of the four different types of tokens. */
typedef enum token_type {
             token_INT /* integer literal */,
			 token_STRING /* string literal*/,
			 token_NAME /* variable or function name */,
			 token_KEYWORD /* keyword */,
			 token_OPEN_PAREN /* '(' */,
			 token_CLOSE_PAREN /* ')' */,
			 token_END /* end of file */,
			 token_SENTINEL /* 'special' value */} token_type;

typedef struct lexer {
    /** Pointer to current input source. */
    FILE *file;
    /** Buffer in which to store the raw text of each token as it is
     *  encountered. */
    char *buffer;
    /** Location in which to store the type of the most recent token. */
    token_type type;
    /** Tracks the current size of the buffer. */
    size_t buff_len;
} lexer;

/** Replaces any garbage stored in LUTHOR's fields with sane initializers. */
void init_lex(lexer *luthor);

/** Opens FILENAME for reading by LEX. */
void open_file(lexer *lex, char *filename);

/** Closes LEX's the currently open file and reclaims the reading buffer.
 *  Behaviour is undefined if LEX does not currently have a file open for
 *  reading. */
void close_file(lexer *lex);

/** Moves LEX to the next token of input. */
void read_token(lexer *lex);

/** checks if token is a keyword. */
int is_keyword(char *buffer);

/** checks if token is completely numeric. */
int is_number(char *buffer);

/** checks if token is a valid name. */
int is_valid_name(char *buffer);

/** Returns the type of the current token of input in LEX. */
token_type peek_type(lexer *lex);

/** Returns the raw text associated with the current token of
 *  input in LEX. This text is overwritten every time read_token()
 *  is called, so be careful. */
char *peek_value(lexer *lex);

#endif
