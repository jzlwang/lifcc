#ifndef CODE_GEN_H
#define CODE_GEN_H

#include "parser.h"

/** Prints AST's string literals in the .data segment. */
void emit_strings(AST *ast);
/** Prints the appropriate code to allocate space in the .data segment
 *  for whatever static memory the program uses. */
void emit_static_memory();
/** Prints AST's code for the .text segment to stdout if the AST
 *  is NOT a function definition. */
void emit_main(AST *ast);
/** Saves args onto the stack. */
void save_to_stack(AST* arg1, AST* arg2);
/** Prints MIPS code which will exit the program. */
void emit_exit();
/** Prints AST's code for the .text segment to stdout if the AST
 *  IS a function definition. */
void emit_functions(AST *ast);

#endif
