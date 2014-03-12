#define DEBUG 0
#define MAIN 0

#include "node.h"
#include "symtable.h"
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

node * mknode_program(int line, int col, node * declList, node * statExpr) {
	attribute * info = malloc(sizeof(attribute));
	node ** children;
	info->val = malloc(sizeof(value));
	if(declList == NULL) {
		info->val->intT = 0;
		children = malloc(sizeof(node));
		children[0] = statExpr;
	} else {
		info->val->intT = declList->info->val->intT; // Number of declarations
		children = malloc(2 * sizeof(node));
		children[0] = declList;
		children[1] = statExpr;
	}
	root = mk_node(line, col, "PROGRAM", info, T_PROGRAM, children);
	return root;
}

node * mknode_function_declaration(int line, int col, node * id, node * paramsList, node * returnType, node * statExpr) {
	attribute * info = malloc(sizeof(attribute));
	node ** children;
	info->val = malloc(sizeof(value));
	if(paramsList == NULL) {
		info->val->intT = 0; // Number of parameters
		children = malloc(3 * sizeof(node));
		children[0] = id;
		children[1] = returnType;
		children[2] = statExpr;
	} else {
		info->val->intT = paramsList->info->val->intT;
		children = malloc(4 * sizeof(node));
		children[0] = id;
		children[1] = paramsList;
		children[2] = returnType;
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

/************Semantic analysis**************/

typedef enum {
  SEM_ERROR,
  SEM_VOID,
  SEM_INTEGER,
  SEM_DECIMAL,
  SEM_BOOL,
  SEM_DEC_ARRAY,
  SEM_INT_ARRAY,
  SEM_RETURN_VOID,
  SEM_RETURN_INT,
  SEM_RETURN_DEC
} sem_type;

sem_type semantic_analysis(node *n, symtable *symbols, sem_type expected_rett);

sem_type semantic_identifier(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: identifier\n");
  var_sym *s = lookup_var(symbols, n->info->label);
  if (!s) {
    printf("%d:%d Unknown identifier %s\n", n->line, n->column, n->info->label);
    return SEM_ERROR;
  }
  switch (get_type(s)) {
    case(INTEGER):
      return SEM_INTEGER;
    case(DECIMAL):
      return SEM_DECIMAL;
    case(INT_ARRAY):
      return SEM_INT_ARRAY;
    case(DEC_ARRAY):
      return SEM_DEC_ARRAY;
  }
}

sem_type semantic_array_index(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: array index\n");
  if (semantic_analysis(n->children[1], symbols, expected_rett) != SEM_INTEGER) {
    printf("%d:%d Array index is not an integer\n", n->line, n->column);
    return SEM_ERROR;
  }
  switch (semantic_analysis(n->children[0], symbols, expected_rett)) {
    case(SEM_INT_ARRAY):
      return SEM_INTEGER;
    case(SEM_DEC_ARRAY):
      return SEM_DECIMAL;
    default:
      printf("%d:%d %s is not an array identifier\n", n->line, n->column, n->children[0]->info->label);
      return SEM_ERROR;
  }
}

sem_type semantic_unary_arithmetic(node *n, symtable *symbols, sem_type expected_rett) {
  if(DEBUG) printf("Semantic analysis: unary arithmetic operator\n");
  switch (semantic_analysis(n->children[0], symbols, expected_rett)) {
    case(SEM_INTEGER):
      return SEM_INTEGER;
    case(SEM_DECIMAL):
      return SEM_DECIMAL;
    default:
      printf("%d:%d Unary operator: not an arithmetic expression\n", n->line, n->column);
      return SEM_ERROR;
  }
}

sem_type semantic_binary_arithmetic(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: binary arithmetic operator\n");
  sem_type a = semantic_analysis(n->children[0], symbols, expected_rett);
  sem_type b = semantic_analysis(n->children[1], symbols, expected_rett);
  if (a == SEM_INTEGER && b == SEM_INTEGER)
    return SEM_INTEGER;
  if (a == SEM_DECIMAL && b == SEM_DECIMAL)
    return SEM_DECIMAL;
  printf("%d:%d Binary operator: incompatible types\n", n->line, n->column);
  return SEM_ERROR;
}

sem_type semantic_unary_boolean(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: unary boolean operator\n");
  if (semantic_analysis(n->children[0], symbols, expected_rett) == SEM_BOOL)
    return SEM_BOOL;
  printf("%d:%d Unary operator: not a boolean expression\n", n->line, n->column);
  return SEM_ERROR;
}

sem_type semantic_binary_boolean(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: binary boolean  operator\n");
  sem_type a = semantic_analysis(n->children[0], symbols, expected_rett);
  sem_type b = semantic_analysis(n->children[1], symbols, expected_rett);
  if (a == SEM_BOOL && b == SEM_BOOL)
    return SEM_BOOL;
  printf("%d:%d Binary operator: not a boolean expression\n", n->line, n->column);
  return SEM_ERROR;
}

sem_type semantic_binary_comparison(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: comparison operator\n");
  sem_type a = semantic_analysis(n->children[0], symbols, expected_rett);
  sem_type b = semantic_analysis(n->children[1], symbols, expected_rett);
  if ((a == SEM_INTEGER && b == SEM_INTEGER) || (a == SEM_DECIMAL && b == SEM_DECIMAL))
    return SEM_BOOL;
  printf("%d:%d Comparison operator: incompatible types\n", n->line, n->column);
  return SEM_ERROR;
}

sem_type semantic_return(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: return\n");
	if (!n->children[0]) {
		if (expected_rett == SEM_RETURN_VOID)
			return SEM_RETURN_VOID;
		printf("%d:%d Incompatible return type\n", n->line, n->column);
    return SEM_ERROR;
  }
	
  switch (semantic_analysis(n->children[0], symbols, expected_rett)) {
    case(SEM_INTEGER):
    	if (expected_rett == SEM_RETURN_INT)
	      return SEM_RETURN_INT;
	    printf("%d:%d Incompatible return type\n", n->line, n->column);
	    return SEM_ERROR;
    case(SEM_DECIMAL):
      if (expected_rett == SEM_RETURN_DEC)
	      return SEM_RETURN_DEC;
	    printf("%d:%d Incompatible return type\n", n->line, n->column);
	    return SEM_ERROR;
  }
  printf("%d:%d Return: not an arithmetic expression\n", n->line, n->column);
  return SEM_ERROR;
}

sem_type semantic_assign(node *n, symtable *symbols, sem_type expected_rett) {
  if(DEBUG) printf("Semantic analysis: assignment\n");
  sem_type a = semantic_analysis(n->children[0], symbols, expected_rett);
  sem_type b = semantic_analysis(n->children[1], symbols, expected_rett);
  if (a == b && (a == SEM_INTEGER || a == SEM_DECIMAL || a == SEM_INT_ARRAY ||a == SEM_DEC_ARRAY))
    return SEM_VOID;
  printf("%d:%d Assignment: incompatible types\n", n->line, n->column);
  return SEM_ERROR;
}

sem_type semantic_free(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: free\n");
  switch (semantic_analysis(n->children[0], symbols, expected_rett)) {
    case(SEM_DEC_ARRAY):
    case(SEM_INT_ARRAY):
      return SEM_VOID;
  }
  printf("%d:%d Free: not an array\n", n->line, n->column);
  return SEM_ERROR;
}

sem_type semantic_new(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: new\n");
  if (semantic_analysis(n->children[1], symbols, expected_rett) != SEM_INTEGER) {
  	printf("%d:%d Array size must be an integer\n", n->line, n->column);
  	return SEM_ERROR;
  }
  if (n->children[0]->nodeType == T_TYPE_ARRAY && n->children[0]->children[0]->nodeType == T_TYPE_INT)
  	return SEM_INT_ARRAY;
  if (n->children[0]->nodeType == T_TYPE_ARRAY && n->children[0]->children[0]->nodeType == T_TYPE_DECIMAL)
  	return SEM_DEC_ARRAY;
	printf("%d:%d Incorrect array type\n", n->line, n->column);
	return SEM_ERROR;
}

int match(sem_type s, var_type t) {
	return ((s == SEM_INTEGER && t == INTEGER)
		|| (s == SEM_DECIMAL && t == DECIMAL)
		|| (s == SEM_INT_ARRAY && t == INT_ARRAY)
 		|| (s == SEM_DEC_ARRAY && t == DEC_ARRAY));
}

int match_args(var_type *types, int argc, node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: match args\n");
	if (n == NULL)
		return 0;
	if (n->info->val->intT == 1) {
		if (match(semantic_analysis(n->children[0], symbols, expected_rett), types[argc - 1]))
			return 1;
		printf("%d:%d Incompatible argument type\n", n->children[0]->line, n->children[0]->column);
		return 0;
	}
	if (match(semantic_analysis(n->children[1], symbols, expected_rett), types[argc - 1]))
		return match_args(types, argc - 1, n->children[0], symbols, expected_rett);
	printf("%d:%d Incompatible argument type\n", n->children[1]->line, n->children[1]->column);
	return 0;
}

sem_type semantic_function_call(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: function call\n");
	int argc = 0;
	node *id = n->children[0];
	fun_sym *f = NULL;
  if (id->nodeType == T_IDENTIFIER)
  	f = lookup_fun(symbols, id->info->label);
  if (f == NULL) {
  	printf("%d:%d Not a valid function identifier\n", n->line, n->column);
  	return SEM_ERROR;
  }
  if (n->children[1])
  	argc = n->children[1]->info->val->intT;
  if (argc != get_args_count(f)) {
  	printf("%d:%d Wrong number of arguments (expected %d, %d given here)\n", n->line, n->column, get_args_count(f), argc);
  	return SEM_ERROR;
  }
  if (!match_args(get_args_type(f), argc, n->children[1], symbols, expected_rett)) {
  	return SEM_ERROR;
  }
  switch (get_rett(f)) {
  	case(INTEGER):
  		return SEM_INTEGER;
  	case(DECIMAL):
  		return SEM_DECIMAL;
  	case(INT_ARRAY):
  		return SEM_INT_ARRAY;
  	case(DEC_ARRAY):
  		return SEM_DEC_ARRAY;
  }
  return SEM_VOID; // NO_RETVAL
}

sem_type semantic_if_statement(node *n, symtable *symbols, sem_type expected_rett){
	if(DEBUG) printf("Semantic analysis: if statemetn\n");
  if (semantic_analysis(n->children[0], symbols, expected_rett) != SEM_BOOL) {
    printf("%d:%d Condition is not a boolean expression\n", n->line, n->column);
    return SEM_ERROR;
  }
  sem_type a = semantic_analysis(n->children[1], symbols, expected_rett);
  sem_type b = semantic_analysis(n->children[2], symbols, expected_rett);
  if (a == b && (a == SEM_RETURN_DEC || a == SEM_RETURN_INT || a == SEM_RETURN_VOID))
    return a;
  return SEM_VOID;
}

sem_type semantic_while_statement(node *n, symtable *symbols, sem_type expected_rett){
  if(DEBUG) printf("Semantic analysis: while statement\n");
  if (semantic_analysis(n->children[0], symbols, expected_rett) != SEM_BOOL) {
    printf("%d:%d Condition is not a boolean expression\n", n->line, n->column);
    return SEM_ERROR;
  }
  if (semantic_analysis(n->children[1], symbols, expected_rett) == SEM_ERROR)
	  return SEM_ERROR;
	else
		return SEM_VOID;
}

sem_type semantic_statement_list(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: statement list\n");
	sem_type previous;
	if (n == NULL)
		return SEM_VOID;
	// TODO from procedure == 1
	if (n->info->val->intT == 1)
		return semantic_analysis(n->children[0], symbols, expected_rett);
	previous = semantic_statement_list(n->children[0], symbols, expected_rett);
	if (previous == SEM_VOID)
		return semantic_analysis(n->children[1], symbols, expected_rett);
	if (previous != SEM_ERROR)
		printf("%d:%d WARNING: Unreachable code\n", n->line, n->column);
	return previous;
}

sem_type semantic_vdecl_list(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: vdecl list\n");
	if (n == NULL)
		return;
	if (n->info->val->intT == 1)
		return semantic_analysis(n->children[0], symbols, expected_rett);
	else {
		sem_type r1, r2;
		r1 = semantic_vdecl_list(n->children[0], symbols, expected_rett);
		r2 = semantic_analysis(n->children[1], symbols, expected_rett);
		if (r1 == SEM_ERROR || r2 == SEM_ERROR)
  		return SEM_ERROR;
  	return r2;
	}
}

sem_type semantic_block_statement(node *n, symtable *symbols, sem_type expected_rett){
  if(DEBUG) printf("Semantic analysis: block statement\n");
	sem_type r1, r2;
  enter_nested_scope(symbols);
  if(DEBUG) printf("semantic_block_statement : %s: %d\n", n->lexeme, n->info->val->intT);
  if (n->info->val->intT == 0) {
  	r1 = semantic_statement_list(n->children[0], symbols, expected_rett);
  	exit_nested_scope(symbols);
  	return r1;
  } else {
  	r1 = semantic_vdecl_list(n->children[0], symbols, expected_rett);
  	r2 = semantic_statement_list(n->children[1], symbols, expected_rett);
  	exit_nested_scope(symbols);
  	if (r1 == SEM_ERROR || r2 == SEM_ERROR)
  		return SEM_ERROR;
  	return r2;
  }
}

sem_type insert_vars(node *n, symtable *symbols, var_type t) {
	if(DEBUG) printf("Semantic analysis: insert vars\n");
	int r;
	char *id;
	if (n == NULL)
		return SEM_VOID;
	if (n->info->val->intT == 1) {
		id = n->children[0]->info->label;
		r = insert_var(symbols, id, t);
	} else {
		insert_vars(n->children[0], symbols, t);
		id = n->children[1]->info->label;
		r = insert_var(symbols, id, t);
	}
	if (r == DUPLICATE_SYM) {
		printf("%d:%d Duplicate variable declaration: %s\n", n->line, n->column, id);
		return SEM_ERROR;
	}
	return SEM_VOID;
}

sem_type semantic_vdecl(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: vdecl\n");
	if (n->children[0]->nodeType == T_TYPE_INT)
		return insert_vars(n->children[1], symbols, INTEGER);
	if (n->children[0]->nodeType == T_TYPE_DECIMAL)
		return insert_vars(n->children[1], symbols, DECIMAL);
  if (n->children[0]->nodeType == T_TYPE_ARRAY && n->children[0]->children[0]->nodeType == T_TYPE_INT)
		return insert_vars(n->children[1], symbols, INT_ARRAY);
  if (n->children[0]->nodeType == T_TYPE_ARRAY && n->children[0]->children[0]->nodeType == T_TYPE_DECIMAL)
		return insert_vars(n->children[1], symbols, DEC_ARRAY);
  printf("%d:%d Incorrect variable type\n", n->line, n->column);
  return SEM_ERROR;
}

sem_type semantic_parameter(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: parameter\n");
	char *id = n->children[0]->info->label;
	int r;
	if (n->children[1]->nodeType == T_TYPE_INT)
		r = insert_var(symbols, id, INTEGER);
	else if (n->children[1]->nodeType == T_TYPE_DECIMAL)
		r = insert_var(symbols, id, DECIMAL);
	else if (n->children[1]->nodeType == T_TYPE_ARRAY && n->children[1]->children[0]->nodeType == T_TYPE_INT)
		r = insert_var(symbols, id, INT_ARRAY);
	else if (n->children[1]->nodeType == T_TYPE_ARRAY && n->children[1]->children[0]->nodeType == T_TYPE_DECIMAL)
		r = insert_var(symbols, id, DEC_ARRAY);
	else {
		printf("%d:%d Argument has wrong type\n", n->line, n->column);
		return SEM_ERROR;
	}
	if (r == DUPLICATE_SYM)
		printf("%d:%d Non-unique argument identifier %s\n", n->line, n->column, id);
	return SEM_VOID;
}

var_type get_param_type(node *n) {
	if(DEBUG) printf("Semantic analysis: get param type\n");
	if (n->children[1]->nodeType == T_TYPE_INT)
		return INTEGER;
	if (n->children[1]->nodeType == T_TYPE_DECIMAL)
		return DECIMAL;
	if (n->children[1]->nodeType == T_TYPE_ARRAY && n->children[1]->children[0]->nodeType == T_TYPE_INT)
		return INT_ARRAY;
	if (n->children[1]->nodeType == T_TYPE_ARRAY && n->children[1]->children[0]->nodeType == T_TYPE_DECIMAL)
		return DEC_ARRAY;
	return INTEGER; // default choice, the wrong type will be signaled during semantic analysis of the node anyway
}

sem_type semantic_param_list(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: param list\n");
  if (n->info->val->intT == 1) {
  	return semantic_parameter(n->children[0], symbols, expected_rett);
  } else {
  	sem_type a = semantic_param_list(n->children[0], symbols, expected_rett);
  	sem_type b = semantic_analysis(n->children[1], symbols, expected_rett);
  	if (a == SEM_ERROR || b == SEM_ERROR)
  		return SEM_ERROR;
  	return SEM_VOID;
  }
}

int get_param_types(node *n, symtable *symbols, sem_type expected_rett, var_type *res, int argc) {
	if(DEBUG) printf("Semantic analysis: get parameters type\n");
	int ret = 1;
	char *id;
	var_type t;
	if (n->info->val->intT == 1) {
    id = n->children[0]->children[0]->info->label;
		t = get_param_type(n->children[0]);
	} else {
	  id = n->children[1]->children[0]->info->label;
	  t = get_param_type(n->children[1]);
	}
	res[argc - 1] = t;
	if (insert_var(symbols, id, t)) {
	  printf("%d:%d Duplicate argument name\n", n->line, n->column);
	  ret = 0;
	}
	if (n->info->val->intT == 1)
	  return ret;
  else
	  return (ret && get_param_types(n->children[0], symbols, expected_rett, res, argc - 1));
}

sem_type semantic_function_declaration(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: fun decl\n");
	sem_type r = SEM_VOID;
	sem_type rett = SEM_VOID;
	int insert;
	sem_type body_rett;
	char *id = n->children[0]->info->label;
	var_type *argt = (var_type *) malloc(n->info->val->intT * sizeof(var_type *));
	int argc = n->info->val->intT;
	enter_nested_scope(symbols);
	if(DEBUG) printf("fun: %d PROC %d FUN %d \n", argc, n->nodeType == T_PROCEDURE_DECLARATION, n->nodeType == T_FUNCTION_DECLARATION);
	if (argc == 0) {
		if (n->children[1]->nodeType == T_TYPE_INT) {
			rett = SEM_RETURN_INT;
			insert = insert_fun(symbols, id, INTEGER, argc, argt);
		} else if (n->children[1]->nodeType == T_TYPE_DECIMAL) {
			rett = SEM_RETURN_DEC;
			insert = insert_fun(symbols, id, DECIMAL, argc, argt);
		}
		body_rett = semantic_block_statement(n->children[2], symbols, rett);
		if (body_rett == SEM_ERROR)
			r = SEM_ERROR;
	} else {
		if (n->children[2]->nodeType == T_TYPE_INT) {
			insert = insert_fun(symbols, id, INTEGER, argc, argt);
			rett = SEM_RETURN_INT;
		} else if (n->children[2]->nodeType == T_TYPE_DECIMAL) {
			insert = insert_fun(symbols, id, DECIMAL, argc, argt);
			rett = SEM_RETURN_DEC;
		}
		if (!get_param_types(n->children[1], symbols, expected_rett, argt, argc))
			r = SEM_ERROR;
		body_rett = semantic_block_statement(n->children[3], symbols, rett);
		if (body_rett == SEM_ERROR)
		  r = SEM_ERROR;
	}
	if (body_rett != rett && body_rett != SEM_ERROR) {
		r = SEM_ERROR;
		printf("%d:%d This function must have a return value\n", n->line, n->column);
	}
	if (insert == DUPLICATE_SYM) {
		printf("%d:%d Duplicate function name\n", n->line, n->column);
		r = SEM_ERROR;
	}
	free(argt);
	exit_nested_scope(symbols);
	return r;
}

semantic_procedure_declaration(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: procedure decl\n");
	sem_type r = SEM_VOID;
	int insert;
	char *id = n->children[0]->info->label;
	var_type *argt = (var_type *) malloc(n->info->val->intT * sizeof(var_type *));
	int argc = n->info->val->intT;
	enter_nested_scope(symbols);
	if(DEBUG) printf("proc: %d PROC %d FUN %d \n", argc, n->nodeType == T_PROCEDURE_DECLARATION, n->nodeType == T_FUNCTION_DECLARATION);
	if (argc == 0) {
		insert = insert_proc(symbols, id, argc, argt);
		r = semantic_statement_list(n->children[1], symbols, expected_rett);
	} else {
		int params = get_param_types(n->children[1], symbols, expected_rett, argt, argc);
		insert = insert_proc(symbols, id, argc, argt);
		if(!params)
			r = SEM_ERROR;
		else {
			if(n->children[2]->nodeType == T_BLOCK_STATEMENT) {
				r = semantic_block_statement(n->children[2], symbols, expected_rett);
			} else {
				r = semantic_analysis(n->children[2], symbols, SEM_RETURN_VOID);
			}
		}
	}
	if (insert == DUPLICATE_SYM) {
		printf("%d:%d Duplicate procedure name\n", n->line, n->column);
		r = SEM_ERROR;
	}
	free(argt);
	exit_nested_scope(symbols);
	return r;
}

sem_type semantic_pdecl(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: pdecl\n");
	switch(n->nodeType) {
		case(T_FUNCTION_DECLARATION):
			return semantic_function_declaration(n, symbols, expected_rett);
		case(T_PROCEDURE_DECLARATION):
			return semantic_procedure_declaration(n, symbols, expected_rett);
		default:
			return SEM_ERROR;
	}
}

sem_type semantic_pdecl_list(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: pdecl list\n");
	if (n->info->val->intT = 1)
		return semantic_pdecl(n->children[0], symbols, expected_rett);
	if (semantic_pdecl_list(n->children[0], symbols, expected_rett) == SEM_ERROR) {
		semantic_analysis(n->children[1], symbols, expected_rett);
		return SEM_ERROR;
	}
	return semantic_analysis(n->children[1], symbols, expected_rett);
}

sem_type semantic_program(node *n, symtable *symbols, sem_type expected_rett) {
	if(DEBUG) printf("Semantic analysis: program\n");
	sem_type t;
	if (n->info->val->intT == 0)
		t = semantic_analysis(n->children[0], symbols, expected_rett);
	else {
		if (semantic_pdecl_list(n->children[0], symbols, expected_rett) == SEM_ERROR) {
			t = SEM_ERROR;
			semantic_analysis(n->children[1], symbols, expected_rett);
		} else {
			t = semantic_analysis(n->children[1], symbols, expected_rett);
		}
	}
	return t;
}

sem_type semantic_analysis(node *n, symtable *symbols, sem_type expected_rett) {
  if(DEBUG) printf("lexa: %s\n", n->lexeme);
  switch(n->nodeType) {
		case(T_VALUE_TRUE):
			return SEM_BOOL;
		case(T_VALUE_FALSE):
			return SEM_BOOL;
		case(T_VALUE_INT):
			return SEM_INTEGER;
		case(T_VALUE_DECIMAL):
			return SEM_DECIMAL;
		case(T_IDENTIFIER):
			return semantic_identifier(n, symbols, expected_rett);
		case(T_ARRAY_INDEX):
			return semantic_array_index(n, symbols, expected_rett);
		case(T_UNARY_ARITHMETIC) :
			return semantic_unary_arithmetic(n, symbols, expected_rett);
		case(T_BINARY_ARITHMETIC) :
			return semantic_binary_arithmetic(n, symbols, expected_rett);
		case(T_UNARY_BOOLEAN) :
			return semantic_unary_boolean(n, symbols, expected_rett);
		case(T_BINARY_BOOLEAN) :
			switch(n->info->op) {
				case(OP_LOWER_THAN):
				case(OP_GREATER_THAN):
				case(OP_LOWER_EQUALS):
				case(OP_GREATER_EQUALS):
				case(OP_EQUALS):
				case(OP_NOT_EQUALS):
					return semantic_binary_comparison(n, symbols, expected_rett);
				case(OP_AND):
				case(OP_OR):
					return semantic_binary_boolean(n, symbols, expected_rett);
			}
		case(T_TYPE_INT):
		  return SEM_VOID;
		case(T_TYPE_DECIMAL):
		  return SEM_VOID;
		case(T_TYPE_ARRAY):
		  return SEM_VOID;
		case(T_RETURN):
		  return semantic_return(n,symbols, expected_rett);
		case(T_ASSIGN):
		  return semantic_assign(n, symbols, expected_rett);
		case(T_FREE):
		  return semantic_free(n, symbols, expected_rett);
		case(T_NEW):
		  return semantic_new(n, symbols, expected_rett);
		case(T_FUNCTION_CALL):
			return semantic_function_call(n, symbols, expected_rett);
		case(T_PARAMETER):
			return semantic_parameter(n, symbols, expected_rett);
		case(T_LIST):
			printf("This shouldn't be called\n");
		  return SEM_ERROR; // this function should not be called on those nodes directly
		case(T_IF_STATEMENT):
		  return semantic_if_statement(n, symbols, expected_rett);
		case(T_WHILE_STATEMENT):
		  return semantic_while_statement(n, symbols, expected_rett);
		case(T_BLOCK_STATEMENT):
			return semantic_block_statement(n, symbols, expected_rett);
		case(T_DECLARATION_STATEMENT):
			return semantic_vdecl(n, symbols, expected_rett);
		case(T_PROGRAM):
			return semantic_program(n, symbols, expected_rett);
    case(T_FUNCTION_DECLARATION):
    	return semantic_function_declaration(n, symbols, expected_rett);
    case(T_PROCEDURE_DECLARATION):
      return semantic_procedure_declaration(n, symbols, expected_rett);
	}
	printf("%d:%d Unknown semantic error\n", n->line, n->column);
	return SEM_ERROR;
}

int start_semantic_analysis(node *root) {
	symtable *t = init_symtable();
	var_type argt[1] = {INTEGER};
	insert_proc(t, "print", 1, argt);
	if (semantic_analysis(root, t, SEM_RETURN_VOID) == SEM_ERROR)
		return 0;
	return (root->nodeType == T_PROGRAM);
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

