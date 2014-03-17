/* Dot output */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 
#include "dotoutput.h"

#define NCHAR 64

int rec_print(node *n, FILE *f, int i, char *mylabel, int nchildren) {
	char child_label[NCHAR];
	int c;
	i++;
	if (n == NULL)
		return i;
	for (c=0; c<nchildren; c++) {
		if (n->children[c]->nodeType == T_LIST)
			i = list_print(n->children[c], f, i, mylabel);
		else {
			i = print(n->children[c], f, i, child_label);
			fprintf(f, "\t%s -- %s;\n", mylabel, child_label);
		}
	}
	return i;
}

int list_print(node *n, FILE *f, int i, char *mylabel) {
	char child_label[NCHAR];
	if (n->info->val->intT == 1) {
		i = print(n->children[0], f, i, child_label);
		fprintf(f, "\t%s -- %s;\n", mylabel, child_label);
		return i;
	}
	i = list_print(n->children[0], f, i, mylabel);
	i = print(n->children[1], f, i, child_label);
	fprintf(f, "\t%s -- %s;\n", mylabel, child_label);
	return i;
}

int print(node *n, FILE *f, int i, char *res) {
	char mylabel[NCHAR];
	switch(n->nodeType) {
		case(T_VALUE_TRUE):
			snprintf(res, NCHAR, "true_%d", i);
			return i + 1;
		case(T_VALUE_FALSE):
			snprintf(res, NCHAR, "false_%d", i);
			return i + 1;
		case(T_VALUE_INT):
			snprintf(res, NCHAR,"cstint_%d_%d", n->info->val->intT, i);
			return i + 1;
		case(T_VALUE_DECIMAL):
			snprintf(res, NCHAR,"cstfloat_%d_%d", (int) n->info->val->floatT, i); //print as an integer, dot doesn't like dots
			return i + 1;
		case(T_IDENTIFIER):
			snprintf(res, NCHAR,"id_%s_%d", n->info->label, i);
			return i + 1;
		case(T_ARRAY_INDEX) :
			snprintf(res, NCHAR, "array_index_%d", i);
			strcpy(mylabel, res);
			return rec_print(n, f, i, mylabel, 2);
		case(T_UNARY_ARITHMETIC) :
			snprintf(res, NCHAR, "minus_%d", i);
			strcpy(mylabel, res);
			return rec_print(n, f, i, mylabel, 1);
		case(T_BINARY_ARITHMETIC) :
			switch(n->info->op) {
				case(OP_ADD):
					snprintf(res, NCHAR, "add_%d", i);
					strcpy(mylabel, res);
					return rec_print(n, f, i, mylabel, 2);
				case(OP_SUB):
					snprintf(res, NCHAR, "sub_%d", i);
					strcpy(mylabel, res);
					return rec_print(n, f, i, mylabel, 2);
				case(OP_MULT):
					snprintf(res, NCHAR, "mult_%d", i);
					strcpy(mylabel, res);
					return rec_print(n, f, i, mylabel, 2);
				case(OP_DIV):
					snprintf(res, NCHAR, "div_%d", i);
					strcpy(mylabel, res);
					return rec_print(n, f, i, mylabel, 2);
			}
			break;
		case(T_UNARY_BOOLEAN) :
			snprintf(res, NCHAR, "not_%d", i);
			strcpy(mylabel, res);
			return rec_print(n, f, i, mylabel, 1);
		case(T_BINARY_BOOLEAN) :
			switch(n->info->op) {
				case(OP_LOWER_THAN):
					snprintf(res, NCHAR, "lt_%d", i);
					strcpy(mylabel, res);
					return rec_print(n, f, i, mylabel, 2);
				case(OP_GREATER_THAN):
					snprintf(res, NCHAR, "gt_%d", i);
					strcpy(mylabel, res);
					return rec_print(n, f, i, mylabel, 2);
				case(OP_LOWER_EQUALS):
					snprintf(res, NCHAR, "le_%d", i);
					strcpy(mylabel, res);
					return rec_print(n, f, i, mylabel, 2);
				case(OP_GREATER_EQUALS):
					snprintf(res, NCHAR, "ge_%d", i);
					strcpy(mylabel, res);
					return rec_print(n, f, i, mylabel, 2);
				case(OP_EQUALS):
					snprintf(res, NCHAR, "eq_%d", i);
					strcpy(mylabel, res);
					return rec_print(n, f, i, mylabel, 2);
				case(OP_NOT_EQUALS):
					snprintf(res, NCHAR, "neq=_%d", i);
					strcpy(mylabel, res);
					return rec_print(n, f, i, mylabel, 2);
				case(OP_AND):
					snprintf(res, NCHAR, "and_%d", i);
					strcpy(mylabel, res);
					return rec_print(n, f, i, mylabel, 2);
				case(OP_OR):
					snprintf(res, NCHAR, "or_%d", i);
					strcpy(mylabel, res);
					return rec_print(n, f, i, mylabel, 2);
			}
		case(T_SKIP):
			snprintf(res, NCHAR, "skip_%d", i);
			return i + 1;
		case(T_TYPE_INT):
			snprintf(res, NCHAR, "int_%d", i);
			return i + 1;
		case(T_TYPE_DECIMAL):
			snprintf(res, NCHAR, "dec_%d", i);
			return i + 1;
		case(T_TYPE_ARRAY):
			snprintf(res, NCHAR, "array_%d", i);
			strcpy(mylabel, res);
			return rec_print(n, f, i, mylabel, 1);
		case(T_RETURN):
			snprintf(res, NCHAR, "return_%d", i);
			strcpy(mylabel, res);
			if(n->children[0] == NULL) return rec_print(n, f, i, mylabel, 0);
			else return rec_print(n, f, i, mylabel, 1);
		case(T_ASSIGN):
			snprintf(res, NCHAR, "assign_%d", i);
			strcpy(mylabel, res);
			return rec_print(n, f, i, mylabel, 2);
		case(T_FREE):
			snprintf(res, NCHAR, "free_%d", i);
			strcpy(mylabel, res);
			return rec_print(n, f, i, mylabel, 1);
		case(T_NEW):
			snprintf(res, NCHAR, "new_%d", i);
			strcpy(mylabel, res);
			return rec_print(n, f, i, mylabel, 2);
		case(T_FUNCTION_CALL):
			snprintf(res, NCHAR, "fun_call_%d", i);
			strcpy(mylabel, res);
			return rec_print(n, f, i, mylabel, 2);
		case(T_PARAMETER):
			snprintf(res, NCHAR, "param_%d", i);
			strcpy(mylabel, res);
			return rec_print(n, f, i, mylabel, 2);
		case(T_LIST):
			snprintf(res, NCHAR, "list_%d", i);
			strcpy(mylabel, res);
			return list_print(n, f, i, mylabel);
		case(T_IF_STATEMENT):
			snprintf(res, NCHAR, "if_then_else_%d", i);
			strcpy(mylabel, res);
			return rec_print(n, f, i, mylabel, 3);
		case(T_WHILE_STATEMENT):
			snprintf(res, NCHAR, "while_do_%d", i);
			strcpy(mylabel, res);
			return rec_print(n, f, i, mylabel, 2);
		case(T_BLOCK_STATEMENT):
			snprintf(res, NCHAR, "begin_end_%d", i);
			strcpy(mylabel, res);
			if (n->info->val->intT == 0)
				return rec_print(n, f, i, mylabel, 1);
			return rec_print(n, f, i, mylabel, 2);
		case(T_DECLARATION_STATEMENT):
			snprintf(res, NCHAR, "var_decl_%d", i);
			strcpy(mylabel, res);
			return rec_print(n, f, i, mylabel, 2);
		case(T_PROGRAM):
			snprintf(res, NCHAR, "program_%d", i);
			strcpy(mylabel, res);
			return rec_print(n, f, i, mylabel, (n->info->val->intT == 0 ? 1 : 2)
				+ (n->children[0] == NULL ? 0 : 1));
    case(T_FUNCTION_DECLARATION):
			snprintf(res, NCHAR, "fun_decl_%d", i);
			strcpy(mylabel, res);
			if (n->info->val->intT == 0)
				return rec_print(n, f, i, mylabel, 3);
			return rec_print(n, f, i, mylabel, 4);
    case(T_PROCEDURE_DECLARATION):
			snprintf(res, NCHAR, "proc_decl_%d", i);
			strcpy(mylabel, res);
			if (n->info->val->intT == 0)
				return rec_print(n, f, i, mylabel, 2);
			return rec_print(n, f, i, mylabel, 3);
	}
	return i;
}

void to_dot(char *filename, node* n) {
  char buf[NCHAR];
  FILE *f = fopen(filename,"w");
  if (f != NULL) {
		fprintf(f, "graph AST {\n");
		print(n, f, 0, buf);
		fprintf(f, "}\n");
		fclose(f);
		printf("Dot file generated\n");
	} else {
		printf("WARNING: unable to write dot file %s\n", filename);
	}
}
