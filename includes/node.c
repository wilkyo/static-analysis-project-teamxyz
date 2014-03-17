#define DEBUG 0
#define MAIN 0

#include "node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**************** Leafs creation ********************/

node * mk_leaf_id(int line, int col, char * lexeme, char * label, type nodeType) {
	if(DEBUG) printf("mk_leaf_id(%s, %s)\n", lexeme, label);
	node * n = malloc(sizeof(node));
	n->lexeme = malloc(strlen(lexeme) * sizeof(char));
	strcpy(n->lexeme, lexeme);
	n->nodeType = nodeType;
	attribute * info = malloc(sizeof(attribute));
	info->label = malloc(strlen(label) * sizeof(char));
	strcpy(info->label, label);
	n->info = info;
	n->children = NULL;
	n->line = line;
	n->column = col;
	return n;
}

node * mk_leaf_cst(int line, int col, char * lexeme, value * val, type nodeType) {
	if(DEBUG && nodeType == T_VALUE_INT) printf("mk_leaf_cst(%s, %d)\n", lexeme, val->intT);
	if(DEBUG && nodeType == T_VALUE_DECIMAL) printf("mk_leaf_cst(%s, %f)\n", lexeme, val->floatT);
	node * n = malloc(sizeof(node));
	n->lexeme = malloc(strlen(lexeme) * sizeof(char));
	strcpy(n->lexeme, lexeme);
	n->nodeType = nodeType;
	attribute * info = malloc(sizeof(attribute));
	info->val = val;
	n->info = info;
	n->children = NULL;
	n->line = line;
	n->column = col;
	return n;
}

node * mk_leaf_type(int line, int col, char * lexeme, type nodeType) {
	if(DEBUG) printf("mk_leaf_type(%s)\n", lexeme);
	node * n = malloc(sizeof(node));
	n->lexeme = malloc(strlen(lexeme) * sizeof(char));
	strcpy(n->lexeme, lexeme);
	n->nodeType = nodeType;
	n->info = NULL;
	n->children = NULL;
	n->line = line;
	n->column = col;
	return n;
}

node * mkleaf_skip(int line, int col) {
	return mk_leaf_cst(line, col, "SKIP", NULL, T_SKIP);
}

node * mkleaf_true(int line, int col) {
	value * v = malloc(sizeof(value));
	v->intT = 1;
	return mk_leaf_cst(line, col, "VALUE_TRUE", v, T_VALUE_TRUE);
}

node * mkleaf_false(int line, int col) {
	value * v = malloc(sizeof(value));
	v->intT = 0;
	return mk_leaf_cst(line, col, "VALUE_FALSE", v, T_VALUE_FALSE);
}

node * mkleaf_int(int line, int col, int intT) {
	value * v = malloc(sizeof(value));
	v->intT = intT;
	return mk_leaf_cst(line, col, "VALUE_INT", v, T_VALUE_INT);
}

node * mkleaf_decimal(int line, int col, float floatT) {
	value * v = malloc(sizeof(value));
	v->floatT = floatT;
	return mk_leaf_cst(line, col, "VALUE_DECIMAL", v, T_VALUE_DECIMAL);
}

node * mkleaf_identifier(int line, int col, char * identifier) {
	return mk_leaf_id(line, col, "IDENTIFIER", identifier, T_IDENTIFIER);
}

/**************** Types ********************/

node * mkleaf_type_int(int line, int col) {
	return mk_leaf_type(line, col, "TYPE_INT", T_TYPE_INT);
}

node * mkleaf_type_bool(int line, int col) {
	return mk_leaf_type(line, col, "TYPE_BOOL", T_TYPE_BOOL);
}

node * mkleaf_type_decimal(int line, int col) {
	return mk_leaf_type(line, col, "TYPE_DECIMAL", T_TYPE_DECIMAL);
}

node * mknode_type_array(int line, int col, node * expr) {
	node ** children = malloc(sizeof(node));
	children[0] = expr;
	return mk_node(line, col, "TYPE_ARRAY", NULL, T_TYPE_ARRAY, children);
}

/*************** Functions *****************/

node * mknode_return(int line, int col, node * expr) {
	node ** children = malloc(sizeof(node));
	children[0] = expr;
	return mk_node(line, col, "RETURN", NULL, T_RETURN, children);
}

node * mknode_assign(int line, int col, node * left, node * right) {
	node ** children = malloc(2 * sizeof(node));
	children[0] = left;
	children[1] = right;
	return mk_node(line, col, "ASSIGN", NULL, T_ASSIGN, children);
}

node * mknode_free(int line, int col, node * expr) {
	node ** children = malloc(sizeof(node));
	children[0] = expr;
	return mk_node(line, col, "FREE", NULL, T_FREE, children);
}

node * mknode_new(int line, int col, node * eltType, node * expr) {
	node ** children = malloc(2 * sizeof(node));
	children[0] = eltType;
	children[1] = expr;
	return mk_node(line, col, "NEW", NULL, T_NEW, children);
}

node * mknode_function_call(int line, int col, node * identifier, node * params) {
	node ** children = malloc(2 * sizeof(node));
	children[0] = identifier;
	children[1] = params;
	return mk_node(line, col, "FUNCTION_CALL", NULL, T_FUNCTION_CALL, children);
}

node * mknode_parameter(int line, int col, node * idParam, node * typeParam) {
	node ** children = malloc(2 * sizeof(node));
	children[0] = idParam;
	children[1] = typeParam;
	return mk_node(line, col, "PARAMETER", NULL, T_PARAMETER, children);
}

node * mknode_list(int line, int col, node * list, node * expr) {
	attribute * info = malloc(sizeof(attribute));
	info->val = malloc(sizeof(value));
	node ** children;
	if(list == NULL) {
		info->val->intT = 1; // Length of the list
		children = malloc(sizeof(node));
		children[0] = expr;
	} else {
		info->val->intT = list->info->val->intT + 1;
		children = malloc(2 * sizeof(node));
		children[0] = list;
		children[1] = expr;
	}
	return mk_node(line, col, "LIST", info, T_LIST, children);
}

/**************** Nodes creation ********************/

node * mk_node(int line, int col, char * lexeme, attribute * info, type nodeType, node ** children) {
	if(DEBUG) printf("mk_node(%s)\n", lexeme);
	node * n = malloc(sizeof(node));
	n->lexeme = malloc(strlen(lexeme) * sizeof(char));
	strcpy(n->lexeme, lexeme);
	n->nodeType = nodeType;
	n->info = info;
	n->children = children;
	n->line = line;
	n->column = col;
	return n;
}

node * mknode_array_index(int line, int col, node * arrayId, node * index) {
	if(DEBUG) printf("mknode_array_index(%s, %s)\n", arrayId->info->label, index->lexeme);
	node ** children = malloc(2 * sizeof(node));
	children[0] = arrayId;
	children[1] = index;
	return mk_node(line, col, "ARRAY_INDEX", NULL, T_ARRAY_INDEX, children);
}

node * mknode_unary_arithmetic(int line, int col, operator op, node * expr) {
	attribute * info = malloc(sizeof(attribute));
	node ** children = malloc(sizeof(node));
	info->op = op;
	children[0] = expr;
	return mk_node(line, col, "UNARY_ARITHMETIC", info, T_UNARY_ARITHMETIC, children);
}

node * mknode_binary_arithmetic(int line, int col, node * left, operator op, node * right) {
	attribute * info = malloc(sizeof(attribute));
	node ** children = malloc(2 * sizeof(node));
	info->op = op;
	children[0] = left;
	children[1] = right;
	return mk_node(line, col, "BINARY_ARITHMETIC", info, T_BINARY_ARITHMETIC, children);
}

node * mknode_unary_boolean(int line, int col, operator op, node * expr) {
	attribute * info = malloc(sizeof(attribute));
	node ** children = malloc(sizeof(node));
	info->op = op;
	children[0] = expr;
	return mk_node(line, col, "UNARY_BOOLEAN", info, T_UNARY_BOOLEAN, children);
}

node * mknode_binary_boolean(int line, int col, node * left, operator op, node * right) {
	attribute * info = malloc(sizeof(attribute));
	node ** children = malloc(2 * sizeof(node));
	info->op = op;
	children[0] = left;
	children[1] = right;
	return mk_node(line, col, "BINARY_BOOLEAN", info, T_BINARY_BOOLEAN, children);
}

/**************** Statements ***************/

node * mknode_if(int line, int col, node * boolExpr, node * thenExpr, node * elseExpr) {
	attribute * info = malloc(sizeof(attribute));
	node ** children = malloc(3 * sizeof(node));
	children[0] = boolExpr;
	children[1] = thenExpr;
	children[2] = elseExpr;
	return mk_node(line, col, "IF_STATEMENT", NULL, T_IF_STATEMENT, children);
}

node * mknode_while(int line, int col, node * boolExpr, node * statExpr) {
	attribute * info = malloc(sizeof(attribute));
	node ** children = malloc(2 * sizeof(node));
	children[0] = boolExpr;
	children[1] = statExpr;
	return mk_node(line, col, "WHILE_STATEMENT", NULL, T_WHILE_STATEMENT, children);
}

node * mknode_start(int line, int col, node * declList, node * blockStatement) {
	attribute * info = malloc(sizeof(attribute));
	node ** children;
	info->val = malloc(sizeof(value));
	if(declList == NULL) {
		info->val->intT = 0;
		children = malloc(sizeof(node));
		children[0] = blockStatement;
	} else {
		info->val->intT = declList->info->val->intT; // Number of declarations
		children = malloc(2 * sizeof(node));
		children[0] = declList;
		children[1] = blockStatement;
	}
	return mk_node(line, col, "BLOCK_STATEMENT", info, T_BLOCK_STATEMENT, children);
}

node * mknode_vdecl(int line, int col, node * typeDecl, node * idList) {
	attribute * info = malloc(sizeof(attribute));
	node ** children = malloc(2 * sizeof(node));
	info->val = malloc(sizeof(value));
	info->val->intT = idList->info->val->intT; // Number of identifiers declared
	children[0] = typeDecl;
	children[1] = idList;
	return mk_node(line, col, "DECLARATION_STATEMENT", info, T_DECLARATION_STATEMENT, children);
}

node * mknode_program(int line, int col, node * id, node * declList, node * statExpr) {
	attribute * info = malloc(sizeof(attribute));
	node ** children;
	info->val = malloc(sizeof(value));
	if(declList == NULL) {
		info->val->intT = 0;
		children = malloc(2 * sizeof(node));
		children[0] = id;
		children[1] = statExpr;
	} else {
		info->val->intT = declList->info->val->intT; // Number of declarations
		children = malloc(3 * sizeof(node));
		children[0] = id;
		children[1] = declList;
		children[2] = statExpr;
	}
	root = mk_node(line, col, "PROGRAM", info, T_PROGRAM, children);
	return root;
}

node * mknode_function_declaration(int line, int col, node * id, node * paramsList, node * returnParam, node * statExpr) {
	attribute * info = malloc(sizeof(attribute));
	node ** children;
	info->val = malloc(sizeof(value));
	if(paramsList == NULL) {
		info->val->intT = 0; // Number of parameters
		children = malloc(3 * sizeof(node));
		children[0] = id;
		children[1] = returnParam;
		children[2] = statExpr;
	} else {
		info->val->intT = paramsList->info->val->intT;
		children = malloc(4 * sizeof(node));
		children[0] = id;
		children[1] = paramsList;
		children[2] = returnParam;
		children[3] = statExpr;
	}
	return mk_node(line, col, "FUNCTION_DECLARATION", info, T_FUNCTION_DECLARATION, children);
}

node * mknode_procedure_declaration(int line, int col, node * id, node * paramsList, node * statExpr) {
	attribute * info = malloc(sizeof(attribute));
	node ** children;
	info->val = malloc(sizeof(value));
	if(paramsList == NULL) {
		info->val->intT = 0; // Number of parameters
		children = malloc(2 * sizeof(node));
		children[0] = id;
		children[1] = statExpr;
	} else {
		info->val->intT = paramsList->info->val->intT;
		children = malloc(3 * sizeof(node));
		children[0] = id;
		children[1] = paramsList;
		children[2] = statExpr;
	}
	return mk_node(line, col, "PROCEDURE_DECLARATION", info, T_PROCEDURE_DECLARATION, children);
}

/**************** DEBUG ********************/

void checkNode(node * n) {
	switch(n->nodeType) {
		case(T_VALUE_TRUE) :
			printf("true: %d\n", n->info->val->intT);
			break;
		case(T_VALUE_FALSE) :
			printf("false: %d\n", n->info->val->intT);
			break;
		case(T_VALUE_INT) :
			printf("int: %d\n", n->info->val->intT);
			break;
		case(T_VALUE_DECIMAL) :
			printf("decimal: %f\n", n->info->val->floatT);
			break;
		case(T_IDENTIFIER) :
			printf("id: %s\n", n->info->label);
			break;
		case(T_ARRAY_INDEX) :
			printf("%s[%d]\n", n->children[0]->info->label, n->children[1]->info->val->intT);
			break;
		case(T_UNARY_ARITHMETIC) :
			printf("un_ar: -%d\n", n->children[0]->info->val->intT);
			break;
		case(T_BINARY_ARITHMETIC) :
			switch(n->info->op) {
				case(OP_ADD):
					printf("bin_ar: %d + %d\n", n->children[0]->info->val->intT, n->children[1]->info->val->intT); break;
				case(OP_SUB):
					printf("bin_ar: %d - %d\n", n->children[0]->info->val->intT, n->children[1]->info->val->intT); break;
				case(OP_MULT):
					printf("bin_ar: %d * %d\n", n->children[0]->info->val->intT, n->children[1]->info->val->intT); break;
				case(OP_DIV):
					printf("bin_ar: %d / %d\n", n->children[0]->info->val->intT, n->children[1]->info->val->intT);
			}
			break;
		case(T_UNARY_BOOLEAN) :
			printf("un_bool: !%d\n", n->children[0]->info->val->intT);
			break;
		case(T_BINARY_BOOLEAN) :
			switch(n->info->op) {
				case(OP_LOWER_THAN):
					printf("bin_bool: %d < %d\n", n->children[0]->info->val->intT, n->children[1]->info->val->intT); break;
				case(OP_GREATER_THAN):
					printf("bin_bool: %d > %d\n", n->children[0]->info->val->intT, n->children[1]->info->val->intT); break;
				case(OP_LOWER_EQUALS):
					printf("bin_bool: %d <= %d\n", n->children[0]->info->val->intT, n->children[1]->info->val->intT); break;
				case(OP_GREATER_EQUALS):
					printf("bin_bool: %d >= %d\n", n->children[0]->info->val->intT, n->children[1]->info->val->intT); break;
				case(OP_EQUALS):
					printf("bin_bool: %d == %d\n", n->children[0]->info->val->intT, n->children[1]->info->val->intT); break;
				case(OP_NOT_EQUALS):
					printf("bin_bool: %d != %d\n", n->children[0]->info->val->intT, n->children[1]->info->val->intT); break;
				case(OP_AND):
					printf("bin_bool: %d && %d\n", n->children[0]->info->val->intT, n->children[1]->info->val->intT); break;
				case(OP_OR):
					printf("bin_bool: %d || %d\n", n->children[0]->info->val->intT, n->children[1]->info->val->intT);
			}
			break;
		default:
			printf("ERROR\n");
	}
}

#if MAIN == 1
void main() {
	node * ltrue = mkleaf_true(), * lfalse = mkleaf_false(), * lint = mkleaf_int(42),
		* lint2 = mkleaf_int(14), * nuna, * lid, * narr, * nbinar, * nbinbool;
	checkNode(ltrue);
	checkNode(lfalse);
	if(lfalse->info->val->intT)
		printf("PROBLEM\n");
	checkNode(lint);
	nuna = mknode_unary_arithmetic(OP_MINUS, lint);
	checkNode(nuna);
	lid = mkleaf_identifier("x");
	checkNode(lid);
	narr = mknode_array_index(lid, lint);
	checkNode(narr);
	nbinar = mknode_binary_arithmetic(lint, OP_MULT, lint2);
	checkNode(nbinar);
	nbinbool = mknode_binary_boolean(lint, OP_EQUALS, lint2);
	checkNode(nbinbool);
	free(ltrue);
	free(lfalse);
	free(lint);
	free(lid);
	free(nuna);
	free(narr);
	free(nbinar);
	free(nbinbool);
}
#endif

