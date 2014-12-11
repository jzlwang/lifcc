#include "parser.h"
#include "lexer.h"
#include <string.h>
#include <stdlib.h>

/** An array of the different string values of keywords. */
char *keywords[] = {"and", "or", "+", "-", "*", "/", "lt", "eq", 
            "function", "struct", "arrow", "assign", "if", 
            "while", "for", "sequence", "intprint", "stringprint",
            "readint"};
/** Sister array of keywords. Keeps track of the corresponding enum. */
int enums[] = {node_AND, node_OR, node_PLUS, node_MINUS, node_MUL, node_DIV,
           node_LT, node_EQ, node_FUNCTION, node_STRUCT, 
           node_ARROW, node_ASSIGN, node_IF, node_WHILE, node_FOR, 
           node_SEQ, node_I_PRINT, node_S_PRINT, node_READ_INT};

/** Boolean that checks if any ASTs have been built. */
int has_built_AST = 0;

/** A hashmap used for more efficient lookups of (keyword, enum) pairs. */
smap *keyword_str_to_enum;


/** Initializes keyword_str_to_enum so that it contains a map
 *  from the string value of a keyword to its corresponding enum. */
void initialize_keyword_to_enum_mapping();


void parse_init() {
    func_decls = smap_new();
    staticvar_decls = smap_new();
    localvar_decls = amap_new();
    stack_sizes = smap_new();
    num_args = smap_new();
    strings = smap_new();
    keyword_str_to_enum = smap_new();
}

void parse_close() {
    smap_del_contents(func_decls);
    smap_del_contents(staticvar_decls);
    amap_del(localvar_decls);
    smap_del(stack_sizes);
    smap_del(num_args);
    smap_del(strings);
    smap_del(keyword_str_to_enum);
}

AST* build_ast (lexer* lex) {
    /* TODO: Implement me. */
    /* Hint: switch statements are pretty cool, and they work 
     *       brilliantly with enums. */
    if (has_built_AST) {
        read_token(lex); // store the first token
    }
    has_built_AST = 1;
    AST* tree = malloc(sizeof(AST)); // allocate space for the AST
    switch (lex->type) { // set tree type when appropriate
        case token_INT:
            tree->type = node_INT;
            break;
        case token_STRING:
            tree->type = node_STRING;
            break;
        case token_NAME:
            tree->type = node_VAR;
            break;
        case token_CLOSE_PAREN:
            tree->type = node_CLOSE_PAREN;
            break;
        case token_END:
            tree->type = node_END;
            break;
        default:
            break;
    }
    if (lex->type == token_KEYWORD) { // error if '(' does not precede keyword
        fprintf(stderr, "err: expected ')' before call to function");
        exit(1);
    }
    else if (lex->type == token_OPEN_PAREN) {
        read_token(lex); // read the next token
        if (lex->type != token_NAME && lex->type != token_KEYWORD) { // only function calls should come after open parens
            printf("this is the token: %d\n", lex->type);
            fprintf(stderr, "err: bad function call");
            exit(1);
        }
        tree->val = malloc(lex->buff_len); // allocate space for val
        strcpy(tree->val, lex->buffer); // copy contents of buffer into val
        if (lex->type == token_NAME) { // set tree type
            tree->type = node_CALL;
        }
        else if (lex->type == token_KEYWORD) {
            tree->type = lookup_keyword_enum(tree->val);
        }
        tree->children = build_ast_lst(lex); // recursive call to build_ast_lst to find children
    }
    else { // for AST nodes with no children
        tree->val = malloc(lex->buff_len);
        strcpy(tree->val, lex->buffer);
        tree->children = NULL;
    }
    return tree;
}

AST_lst* build_ast_lst(lexer* lex) {
    AST_lst* tree_l = malloc(sizeof(AST_lst)); // allocate space for list of children
    tree_l->val = build_ast(lex); // recursive call to build_ast
    if ((tree_l->val)->type == node_END) { // if the file ends with mismatched parens
        fprintf(stderr, "err: unbalanced parens");
        exit(1);
    }
    else if ((tree_l->val)->type == node_CLOSE_PAREN) { // if there are no operands to function
        return NULL;
    }
    tree_l->next = build_ast_lst(lex); // call itself to find rest of children
    return tree_l;
}

void print_AST(AST *ptr) {
    char* node_val = ptr->val;
    printf(" %s ", node_val);
    printf("(");
    AST_lst* child = ptr->children;
    while (child != NULL) {
        print_AST(child->val);
        child = child->next;
    }
    printf(")");
}

void free_ast (AST *ptr) {
    /* TODO: Implement me. */
    if (ptr) {
        free(ptr->val);

        AST_lst* temp1 = ptr->children, *temp2;
        while (temp1) {
            temp2 = temp1->next;
            free(temp1->val);
            free(temp1);
            temp1 = temp2;
        }
        free(ptr);
    }
}

void check_tree_shape(AST *ptr) {
    /* TODO: Implement me. */
    /* Hint: This function is just asking to be table-driven */
    size_t counter = AST_lst_len(ptr->children);
    if (ptr->type == node_INT || ptr->type == node_VAR || ptr->type == node_STRING) {
        if (counter > 0) {
            fprintf(stderr, "err2: non function called with args");
            exit(1);
        }
    }
    else if (ptr->type == node_CLOSE_PAREN) {
        fprintf(stderr, "err: unbalanced parens");
        exit(1);
    }
    else if (ptr->type == node_I_PRINT || ptr->type == node_S_PRINT) {
        if (counter != 1) {
            fprintf(stderr, "err3: wrong number of args");
            exit(1);
        }
    }
    else if (ptr->type == node_AND || ptr->type == node_OR || ptr->type == node_PLUS || 
             ptr->type == node_MINUS || ptr->type == node_MUL || ptr->type == node_DIV || 
             ptr->type == node_LT || ptr->type == node_EQ || ptr->type == node_FUNCTION ||
             ptr->type == node_ARROW || ptr->type == node_ASSIGN || ptr->type == node_WHILE) {
        if (counter != 2) {
            fprintf(stderr, "err4: wrong number of args");
            exit(1);
        }       
    }
    else if (ptr->type == node_IF) {
        if (counter != 3) {
            fprintf(stderr, "err5: wrong number of args");
            exit(1);
        }
    }
    else if (ptr->type == node_FOR) {
        if (counter != 4) {
            fprintf(stderr, "err6: wrong number of args");
            exit(1);
        }
    }
    else if (ptr->type == node_STRUCT || ptr->type == node_SEQ) {
        if (counter < 1) {
            fprintf(stderr, "err7: wrong number of args");
            exit(1);
        }
    }
    AST_lst* temp = ptr->children;
    while (temp != NULL) {
        check_tree_shape(temp->val);
        temp = temp ->next;
    }
}

void gather_decls(AST *ast, char *env, int is_top_level) {
    /* TODO: Implement me. */
    /* Hint: switch statements are pretty cool, and they work 
     *       brilliantly with enums. */
    if (ast->type == node_FUNCTION) { // if node_FUNCTION:
        if (!is_top_level) { // if its at the top level:
            fprintf(stderr, "err: function declaration inside of function"); // return error because functions cant be declared inside a function
            exit(1);            
        }
        else { // else:
            char* func = ast->children->val->val; // get the name of the function
            AST_lst* funcargs = ast->children->val->children; // get the args
            AST* body = ast->children->next->val; // get the body of the function
            smap_put(func_decls, func, 1); // put into decls with function name as key, and value whatever
            smap_put(num_args, func, (int) AST_lst_len(funcargs));
            smap_put(stack_sizes, func, 4*AST_lst_len(funcargs)); // how do i find the stack size???
            for (AST_lst* temp = funcargs; temp != NULL; temp = temp->next) { // for all of the args of the func
                char** varfunc = malloc(2);
                varfunc[0] = temp->val->val;
                varfunc[1] = func;
                amap_put(localvar_decls, varfunc, 0); //put into amap
            }
            gather_decls(body, func, 0); // recursively call gather_decls on body of procedure
        }
    }
    else if (ast->type == node_ASSIGN) { // else if node_ASSIGN: (we assign a val to a var):
        char* varname = ast->children->val->val;
        AST* value = ast->children->next->val;
        gather_decls(value, env, is_top_level); // recursively call gather_decls on the value we are storing
        if (!is_top_level) { //if its at the top level:
            char** varfunc = malloc(2);
            varfunc[0] = varname;
            varfunc[1] = env;
            if (smap_get(staticvar_decls, varname) != -1 && amap_get(localvar_decls, varfunc) == -1) {
                smap_put(staticvar_decls, varname, 1);
            }
            else {
                amap_put(localvar_decls, varfunc, 0);
                smap_increment(stack_sizes, env, 4);
            }
        }
        else {
            smap_put(staticvar_decls, varname, 1); // put into vardecls with "" and var name ("" because its at top level) as key
        }
    }
    else if (ast->type == node_STRING) {
        smap_put(strings, ast->val, 0); //easy enough, store strings into the right smap
    }
    else if (ast->type == node_VAR) {
        char** varfunc = malloc(2);
        varfunc[0] = ast->val;
        varfunc[1] = env;
        int check_local = amap_get(localvar_decls, varfunc);
        int check_global = smap_get(staticvar_decls, ast->val);
        if (check_local == -1 && check_global == -1) {
            fprintf(stderr, "err: undeclared variable");
            exit(1);
        }
    }
    else if (ast->type == node_CALL) {
        if (smap_get(func_decls, ast->val) != -1) { // if smap_get returns a value:
            AST_lst* temp = ast->children; // return error because the function has not been declared yet
            while (temp != NULL) {
                gather_decls(temp->val, env, is_top_level);
                temp = temp->next;
            }
            if (smap_get(num_args, ast->val) != AST_lst_len(ast->children)) {
                fprintf(stderr, "err: wrong number of args");
                exit(1);
            }
        }
        else { // else:
            fprintf(stderr, "err: undeclared function");
            exit(1);
        }
    }
    else if (is_keyword(ast->val)) {
        if (ast->type == node_STRUCT) {
            if (!is_top_level) {
                smap_increment(stack_sizes, env, 4*AST_lst_len(ast->children));
            }
        }
        for (AST_lst* temp = ast->children; temp != NULL; temp = temp->next) { // recursively call gather_decls on all the children
            gather_decls(temp->val, env, is_top_level);
        }
    }
}

node_type lookup_keyword_enum(char *str) {
    if (smap_get(keyword_str_to_enum, keywords[0]) == -1) {
    initialize_keyword_to_enum_mapping();
    }
    return smap_get(keyword_str_to_enum, str);
}

void initialize_keyword_to_enum_mapping() {
    /* Note that enums is an *array*, not a pointer, so this
     * sizeof business is reasonable. */
    size_t num_keywords = sizeof(enums) / sizeof(int);
    for (size_t i = 0; i < num_keywords; i += 1) {
    smap_put(keyword_str_to_enum, keywords[i], enums[i]);
    }
}

size_t AST_lst_len(AST_lst *lst) {
    int num_fields = 0;
    while (lst) {
    num_fields += 1;
    lst = lst->next;
    }
    return num_fields;
}


smap *func_decls;
smap *staticvar_decls;
amap *localvar_decls;
smap *stack_sizes;
smap *num_args;
smap *strings;

/*int main(int argc, char* argv[]){
    parse_init();
    lexer *lex1 = malloc(sizeof(lexer));
    init_lex(lex1);
    open_file(lex1,argv[1]);
    AST* tree1 = build_ast(lex1);
    AST* tree2 = build_ast(lex1);
    AST* tree3 = build_ast(lex1);
    AST* tree4 = build_ast(lex1);
    AST* tree5 = build_ast(lex1);
    AST* tree6 = build_ast(lex1);
    AST* tree7 = build_ast(lex1);
    AST* tree8 = build_ast(lex1);
    print_AST(tree1);
    printf("\n");
    print_AST(tree2);
    printf("\n");
    print_AST(tree3);
    printf("\n");
    print_AST(tree4);
    printf("\n");
    print_AST(tree5);
    printf("\n");
    print_AST(tree6);
    printf("\n");
    print_AST(tree7);
    printf("\n");
    print_AST(tree8);
    printf("\n");
    gather_decls(tree1, "", 1);
    gather_decls(tree2, "", 1);
    gather_decls(tree3, "", 1);
    gather_decls(tree4, "", 1);
    gather_decls(tree5, "", 1);
    gather_decls(tree6, "", 1);
    gather_decls(tree7, "", 1);
    gather_decls(tree8, "", 1);
    check_tree_shape(tree1);
    check_tree_shape(tree2);
    check_tree_shape(tree3);
    check_tree_shape(tree4);
    check_tree_shape(tree5);
    check_tree_shape(tree6);
    check_tree_shape(tree7);
    check_tree_shape(tree8);
    free_ast(tree1);
    free_ast(tree2);
    free_ast(tree3);
    free_ast(tree4);
    free_ast(tree5);
    free_ast(tree6);
    free_ast(tree7);
    free_ast(tree8);
    free(lex1);
    return 0;
}*/