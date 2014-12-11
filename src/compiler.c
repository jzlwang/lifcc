/* Author:  YOUR NAME HERE */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "code_gen.h"
#include "util/util.h"

#define MAX(X,Y) (X > Y ? X : Y)

AST **top_level_asts = NULL;
size_t num_top_level_asts = 0;
size_t top_level_asts_cap = 0;


/** Prints the appropriate usage of this executable. */
void usage() {
    fatal_error("Usage: lifcc DIR/BASE.lc\n");
}

void append_ast_to_top_level(AST *ast) {
    if (num_top_level_asts >= top_level_asts_cap) {
	top_level_asts_cap = MAX(1, 2 * top_level_asts_cap);
	top_level_asts = safe_realloc(top_level_asts, 
				      top_level_asts_cap * sizeof(AST*));
    }
    top_level_asts[num_top_level_asts++] = ast;
}

/** Compiles the LIFC program residing in FILENAME and prints
 *  the results to stdout. */
void compile(char *filename) {
    lexer luthor;
    /* Initialize lexer. */
    init_lex(&luthor);
    open_file(&luthor, filename);
    
    /* Run parser. */
    parse_init();
    while (peek_type(&luthor) != token_END) {
	AST *ast = build_ast(&luthor);
	append_ast_to_top_level(ast);
	check_tree_shape(ast);
	gather_decls(ast, "", 1);
    }

    /* Run code generation. */
    for (size_t i = 0; i < num_top_level_asts; i += 1) {
    	emit_strings(top_level_asts[i]);
    }
    emit_static_memory();
    for (size_t i = 0; i < num_top_level_asts; i += 1) {
    	emit_main(top_level_asts[i]);
    }
    emit_exit();
    for (size_t i = 0; i < num_top_level_asts; i += 1) {
    	emit_functions(top_level_asts[i]);
    }	

    /* Cleanup. */
    close_file(&luthor);
    parse_close();
    for (size_t i = 0; i < num_top_level_asts; i += 1) {
    	free_ast(top_level_asts[i]);
    }
    if (top_level_asts) {
    	free(top_level_asts);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
	usage();
    }
    compile(argv[1]);
    return 0;
}
