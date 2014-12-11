#include "code_gen.h"
#include "parser.h"
#include "util/amap.h"
#include "util/smap.h"
#include <stdlib.h>

/** A counter to prevent the issuance of duplicate labels. */
unsigned label_count = 0;
/** True iff the data segment has already been partially printed. */
int data_seg_opened = 0;
/** True iff the text segment has already been partially printed. */
int text_seg_opened = 0;

/** A counter to give all the strings unique names. */
unsigned string_count = 0;
/** A counter to give all bool return labels unique names. */
unsigned bools = 0;
/** A counter to give all end return labels unique names. */
unsigned dones = 0;
/** A counter to give all loop labels unique names. */
unsigned loops = 0;


void emit_strings(AST *ast) {
    /* TODO: Implement me. */
    if (!data_seg_opened) {
    	printf("	.data\n\n");
    	data_seg_opened = 1;
    }
    if (ast->type == node_STRING) {
    	printf("string$%d:    .asciiz %s\n", string_count, ast->val);
    	smap_put(strings, ast->val, string_count);
    	++string_count;
    }
    for (AST_lst* temp = ast->children; temp != NULL; temp = temp->next) {
    	emit_strings(temp->val);
    }
}

void emit_static_memory() {
    /* TODO: Implement me. */
    for (size_t i = 0; i < staticvar_decls->num_buckets; i += 1) {
    	for (size_t j = 0; j < staticvar_decls->buckets[i].num_pairs; j += 1) {
    		printf("svar$%s:    .space 4\n", staticvar_decls->buckets[i].pairs[j].key);
    	}
    }
}

void emit_main(AST *ast) {
	if (!text_seg_opened) {
		printf("\n	.text\n\n");
		text_seg_opened = 1;
	}
    /* TODO: Implement me. */
    if (ast->type != node_FUNCTION) {
   		if (ast->type == node_INT) {
   			int n = strtol(ast->val, NULL, 10);
   			printf("	li $v0 %d\n", n);
   		}
   		else if (ast->type == node_STRING) {
   			printf("	la $v0 string$%d\n", smap_get(strings, ast->val));
   		}
   		else if (ast->type == node_VAR) {

   		}
   		else if (ast->type == node_CALL) {
   			printf("	jal func$%s\n", ast->val);
   		}
   		else if (ast->type == node_CLOSE_PAREN) {
   			fprintf(stderr, "err: this should not have happened...");
   			exit(1);
   		}
   		else if (ast->type == node_END) {
   			return;
   		}
   		else if (ast->type == node_AND) {
   			emit_main(ast->children->val);
   			printf("	move $t0 $v0\n");
   			emit_main(ast->children->next->val);
   			printf("	move $t1 $v0\n");
   			printf("	beq $t0 0 bool$%d\n", bools);
   			printf("	beq $t1 0 bool$%d\n", bools);
   			printf("	li $v0 1\n");
   			printf("	j done$%d\n", dones);
   			printf("bool$%d:\n", bools);
   			printf("	li $v0 0\n");
   			printf("done$%d:\n", dones);
   			++bools;
   			++dones;
   		}
   		else if (ast->type == node_OR) {
   			emit_main(ast->children->val);
   			printf("	move $t0 $v0\n");
   			emit_main(ast->children->next->val);
   			printf("	move $t1 $v0\n");
   			printf("	bne $t0 0 bool$%d\n", bools);
   			printf("	bne $t1 0 bool$%d\n", bools);
   			printf("	li $v0 0\n");
   			printf("	j done$%d\n", dones);
   			printf("bool$%d:\n", bools);
   			printf("	li $v0 1\n");
   			printf("done$%d:\n", dones);
   			++bools;
   			++dones;
   		}
   		else if (ast->type == node_PLUS) {
   			save_to_stack(ast->children->val, ast->children->next->val);
   			printf("	lw $t0 0($sp)\n");
   			printf("	lw $t1 4($sp)\n");
   			printf("	addi $sp $sp 8\n");
   			printf("	add $v0 $t0 $t1\n");
   		}
   		else if (ast->type == node_MINUS) {
   			save_to_stack(ast->children->val, ast->children->next->val);
   			printf("	lw $t0 0($sp)\n");
   			printf("	lw $t1 4($sp)\n");
   			printf("	addi $sp $sp 8\n");
   			printf("	sub $v0 $t0 $t1\n");

   		}
   		else if (ast->type == node_MUL) {
   			save_to_stack(ast->children->val, ast->children->next->val);
   			printf("	lw $t0 0($sp)\n");
   			printf("	lw $t1 4($sp)\n");
   			printf("	addi $sp $sp 8\n");
   			printf("	mul $v0 $t0 $t1\n");
   		}
   		else if (ast->type == node_LT) {
   			save_to_stack(ast->children->val, ast->children->next->val);
   			printf("	lw $t0 0($sp)\n");
   			printf("	lw $t1 4($sp)\n");
   			printf("	addi $sp $sp 8\n");
   			printf("	slt $v0 $t0 $t1\n");
   		}
   		else if (ast->type == node_EQ) {
   			save_to_stack(ast->children->val, ast->children->next->val);
   			printf("	lw $t0 0($sp)\n");
   			printf("	lw $t1 4($sp)\n");
   			printf("	addi $sp $sp 8\n");
   			printf("	beq $t0 $t1 bool$%d\n", bools);
   			printf("	li $v0 0\n");
   			printf("	j done$%d\n", dones);
   			printf("bool$%d:\n", bools);
   			printf("	li $v0 1\n");
   			printf("done$%d:\n", dones);
   			++bools;
   			++dones;
   		}
   		else if (ast->type == node_DIV) {
   			save_to_stack(ast->children->val, ast->children->next->val);
   			printf("	lw $t0 0($sp)\n");
   			printf("	lw $t1 4($sp)\n");
   			printf("	addi $sp $sp 8\n");
   			printf("	div $v0 $t0 $t1\n");
   		}
   		else if (ast->type == node_STRUCT) {
   			printf("	li $v0 9\n");
   			int space = 4*AST_lst_len(ast->children);
   			printf("	li $a0 %d\n", space);
   			printf("	syscall\n");
   			printf("	move $s0 $v0\n");
			int count = 0;
			printf("	addi $sp $sp -4\n");
			printf("	sw $v0 0($sp)\n");
			for (AST_lst* temp = ast->children; temp != NULL; temp = temp->next) {
				emit_main(temp->val);
				printf("	sw $v0 %d($s0)\n", count);
				count+=4;
			}
			printf("	lw $v0 0($sp)\n");
			printf("	addi $sp $sp 4\n");
   		}
   		else if (ast->type == node_ARROW) {
   			emit_main(ast->children->next->val);
   			printf("	move $t0 $v0\n");
   			printf("	sll $t0 $t0 2\n");
   			emit_main(ast->children->val);
   			printf("	move $t1 $v0\n");
   			printf("	add $t0 $t0 $t1\n");
   			printf("	lw $v0 0($t0)\n");
   		}
   		else if (ast->type == node_ASSIGN) {

   		}
   		else if (ast->type == node_IF) {
   			emit_main(ast->children->val);
   			printf("	move $t0 $v0\n");
   			printf("	beq $t0 0 bool$%d\n", bools);
   			emit_main(ast->children->next->val);
   			printf("	j done$%d\n", dones);
   			printf("bool$%d:\n", bools);
   			emit_main(ast->children->next->next->val);
   			printf("done$%d:\n", dones);
   			++bools;
   			++dones;
   		}
   		else if (ast->type == node_WHILE) {
   			printf("loop$%d:\n", loops);
   			emit_main(ast->children->val);
   			printf("	move $t0 $v0\n");
   			printf("	beq $t0 0 done$%d\n", dones);
   			emit_main(ast->children->next->val);
   			printf("	j loop$%d\n", loops);
   			printf("done$%d:\n", dones);
   			printf("	li $v0 0\n");
   			++loops;
   			++dones;
   		}
   		else if (ast->type == node_FOR) {
   			emit_main(ast->children->val);
   			printf("loop$%d:\n", loops);
   			emit_main(ast->children->next->val);
   			printf("	beq $v0 0 done$%d\n", dones);
   			emit_main(ast->children->next->next->next->val);
   			emit_main(ast->children->next->next->val);
   			printf("	j loop$%d\n", loops);
   			printf("done$%d:\n", dones);
   			printf("	li $v0 0");
   			++loops;
   			++dones;
   		}
   		else if (ast->type == node_SEQ) {
   			for (AST_lst* temp = ast->children; temp != NULL; temp = temp->next) {
   				emit_main(temp->val);
   			}
   		}
   		else if (ast->type == node_I_PRINT) {
   			emit_main(ast->children->val);
   			printf("	move $a0 $v0\n");
   			printf("	li $v0 1\n");
   			printf("	syscall\n");
   		}
   		else if (ast->type == node_S_PRINT) {
   			emit_main(ast->children->val);
   			printf("	move $a0 $v0\n");
   			printf("	li $v0 4\n");
   			printf("	syscall\n");
   		}
   		else if (ast->type == node_READ_INT) {
   			printf("	li $v0 5\n");
   			printf("	syscall\n");
   		}
	}
}

void save_to_stack(AST* arg1, AST* arg2) {
	printf("	addi $sp $sp -8\n");
	emit_main(arg1);
	printf("	sw $v0 0($sp)\n");
	emit_main(arg2);
	printf("	sw $v0 4($sp)\n");
}

void emit_exit() {
    printf("    li $v0 10\n");
    printf("    syscall\n");
}

void emit_functions(AST *ast) {
    /* TODO: Implement me. */
    if (ast->type == node_FUNCTION) {
      char* func_name = ast->children->val->val;
      AST* body = ast->children->next->val;
      printf("%s$f:\n", func_name);
      printf("sw $ra 0($sp)\n");
      emit_main(body);
      printf("lw $ra 0($sp)\n");
      printf("jr $ra\n");
    }
}
