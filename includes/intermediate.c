#include "symtable.h"
#include "node.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define INTERMEDIATE_DEBUG 0
#define BUF_SZ 64

typedef int label;

label last_fresh_label = 0;

typedef int reg;

reg last_fresh_reg = 0;

typedef int frame_number;

frame_number last_fresh_frame = 0;

typedef float expr_dec_cst;

typedef int expr_int_cst;

typedef reg expr_read_reg;

typedef struct expr_s expr;

typedef struct instr_s instr;

typedef expr *expr_read_memory;

typedef struct {
	operator op;
	expr *val; 
} expr_unop;

typedef struct {
	operator op;
	expr *val1;
	expr *val2; 
} expr_binop;

typedef union {
	expr_dec_cst dec_cst;
	expr_int_cst int_cst;
	expr_read_reg read_reg;
	expr_read_memory read_memory;
	expr_unop unop;
	expr_binop binop;
} expr_val;

typedef enum {
	INT_CST,
	DEC_CST,
	READ_REG,
	READ_MEMORY,
	UNOP,
	BINOP
} expr_type;

typedef struct expr_s {
	expr_type type;
	expr_val val;
} expr;

typedef union {
	label l;
	frame_number f;
	reg r;
	expr *e;
} instr_arg;

typedef enum {
	LABEL,
	REG_ASSIGN,
	EXPR_ASSIGN,
	JUMP,
	GOTO,
	CALL,
	CALL_ASSIGN
} instr_type;

typedef struct instr_s {
	instr_type type;
	int argc;
	instr_arg *argv;
	struct instr_s *next;
} instr;

typedef struct {
	frame_number n;
	label entry;
	label ret;
	int argc;
	reg *argv;
	reg result;
	int stack;
	instr *instructions;
} frame;

/**********Label and register generators**********/

reg get_fresh_register() {
	return ++last_fresh_reg;
}

label get_fresh_label() {
	return ++last_fresh_label;
}

frame_number get_fresh_frame() {
	return ++last_fresh_frame;
}

/*******************Expressions*******************/

expr *intermediate_true() {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: true\n");
	expr *res = (expr *) malloc(sizeof(expr));
	res->type = INT_CST;
	res->val.int_cst = 1;
	return res;
}

expr *intermediate_false() {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: false\n");
	expr *res = (expr *) malloc(sizeof(expr));
	res->type = INT_CST;
	res->val.int_cst = 0;
	return res;
}

expr *intermediate_int_cst(int val) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: int cst\n");
	expr *res = (expr *) malloc(sizeof(expr));
	res->type = INT_CST;
	res->val.int_cst = val;
	return res;
}

expr *intermediate_float_cst(float val) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: float cst\n");
	expr *res = (expr *) malloc(sizeof(expr));
	res->type = DEC_CST;
	res->val.dec_cst = val;
	return res;
}

expr *intermediate_variable(symtable *symbols, char *id) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: variable\n");
	var_sym *v = lookup_var(symbols, id);
	if (v == NULL)
		return NULL;
	expr *res = (expr *) malloc(sizeof(expr));
	res->type = READ_REG;
	res->val.read_reg = get_var_register(v);
	return res;
}

expr *intermediate_unop(operator op, expr *e) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: unop\n");
	expr *res = (expr *) malloc(sizeof(expr));
	res->type = BINOP;
	res->val.unop.op = op;
	res->val.unop.val = e;
	return res;
}

expr *intermediate_binop(operator op, expr *e1, expr *e2) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: binop\n");
	expr *res = (expr *) malloc(sizeof(expr));
	res->type = BINOP;
	res->val.binop.op = op;
	res->val.binop.val1 = e1;
	res->val.binop.val2 = e2;
	return res;
}

expr *intermediate_expr(node *n, symtable *symbols) {
	expr *e1, *e2;
  switch(n->nodeType) {
		case(T_VALUE_TRUE):
			return intermediate_true();
		case(T_VALUE_FALSE):
			return intermediate_false();
		case(T_VALUE_INT):
			return intermediate_int_cst(n->info->val->intT);
		case(T_VALUE_DECIMAL):
			return intermediate_float_cst(n->info->val->floatT);
		case(T_IDENTIFIER):
			return intermediate_variable(symbols, n->info->label);
		case(T_UNARY_ARITHMETIC) :
		case(T_UNARY_BOOLEAN) :
			e1 = intermediate_expr(n->children[0], symbols);
			return intermediate_unop(n->info->op, e1);
		case(T_BINARY_ARITHMETIC) :
		case(T_BINARY_BOOLEAN) :
			e1 = intermediate_expr(n->children[0], symbols);
			e2 = intermediate_expr(n->children[1], symbols);
			return intermediate_binop(n->info->op, e1, e2);
		case(T_ARRAY_INDEX):
			printf("ERROR: intermediate code does not handle arrays\n");
			exit(1);
		default:
			printf("Unknown error during intermediate code production (expression)\n");
	}
	return NULL;
}

void get_params_expr_aux(node *n, symtable *symbols, expr **res, int ith) {
	if (n->info->val->intT == 1) {
		res[ith] = intermediate_expr(n->children[0], symbols);
	} else {
		get_params_expr_aux(n->children[0], symbols, res, ith - 1);
		res[ith] = intermediate_expr(n->children[1], symbols);
	}
}

expr **get_params_expr(node *list, symtable *symbols, int *argc) {
	int length = list->info->val->intT;
	expr **res =  (expr **) malloc(length * sizeof(expr *));
	if (length == 1)
		res[0] = intermediate_expr(list->children[0], symbols);
	else
		get_params_expr_aux(list, symbols, res, length - 1);
	return res;
}

/**********************Print expressions**********************/

void print_expr(expr *e, char *buf) {
	char buf1[BUF_SZ], buf2[BUF_SZ];
	switch(e->type) {
		case(INT_CST):
			snprintf(buf, BUF_SZ, "%d", e->val.int_cst);
			return;
		case(DEC_CST):
			snprintf(buf, BUF_SZ, "%f", e->val.dec_cst);
			return;
		case(READ_REG):
			snprintf(buf, BUF_SZ, "reg%d", e->val.read_reg);
			return;
		case(UNOP):
			print_expr(e->val.unop.val, buf1);
			switch(e->val.unop.op) {
				case(OP_MINUS):
					snprintf(buf, BUF_SZ, "-%s", buf1);
					return;
				case(OP_NOT):
					snprintf(buf, BUF_SZ, "!%s", buf1);
					return;
			}
		case(BINOP):
			print_expr(e->val.binop.val1, buf1);
			print_expr(e->val.binop.val2, buf2);
			switch(e->val.binop.op) {
				case(OP_ADD):
					snprintf(buf, BUF_SZ, "(%s)+(%s)", buf1, buf2);
					return;
				case(OP_SUB):
					snprintf(buf, BUF_SZ, "(%s)-(%s)", buf1, buf2);
					return;
				case(OP_MULT):
					snprintf(buf, BUF_SZ, "(%s)*(%s)", buf1, buf2);
					return;
				case(OP_DIV):
					snprintf(buf, BUF_SZ, "(%s)/(%s)", buf1, buf2);
					return;
				case(OP_LOWER_THAN):
					snprintf(buf, BUF_SZ, "(%s)<(%s)", buf1, buf2);
					return;
				case(OP_GREATER_THAN):
					snprintf(buf, BUF_SZ, "(%s)>(%s)", buf1, buf2);
					return;
				case(OP_LOWER_EQUALS):
					snprintf(buf, BUF_SZ, "(%s)<=(%s)", buf1, buf2);
					return;
				case(OP_GREATER_EQUALS):
					snprintf(buf, BUF_SZ, "(%s)>=(%s)", buf1, buf2);
					return;
				case(OP_EQUALS):
					snprintf(buf, BUF_SZ, "(%s)==(%s)", buf1, buf2);
					return;
				case(OP_NOT_EQUALS):
					snprintf(buf, BUF_SZ, "(%s)!=(%s)", buf1, buf2);
					return;
				case(OP_AND):
					snprintf(buf, BUF_SZ, "(%s)&(%s)", buf1, buf2);
					return;
				case(OP_OR):
					snprintf(buf, BUF_SZ, "(%s)|(%s)", buf1, buf2);
					return;
			}
	}
}

/*******************Instructions*******************/
void concat(instr *i1, instr *i2) {
	instr *i = i1;
	while (i->next != NULL)
		i = i->next;
	i->next = i2;
}

// TODO fix this!!!
void concat_many(int argc, ...) {
	int i;
	va_list argv;
	instr *start, *end;
	for (i=0; i<argc; i++) {
		end = va_arg(argv, instr*);
		if (i == 0) {
			start = end;
		} else {
			concat(start, end);
		}
	}
	va_end(argv);
}

// last argument must be of type instr_arg
instr *instr_init(instr_type type, int argc, ...) {
	int i;
	va_list argv;
	va_start(argv, argc);
	instr *res = (instr *) malloc(sizeof(instr));
	res->type = type;
	res->argc = argc;
	res->argv = (instr_arg *) malloc(argc * sizeof(instr_arg));
	for (i=0; i<argc; i++) {
		instr_arg a = *(va_arg(argv, instr_arg*));
		res->argv[i] = a;
	}
	res->next = NULL;
	va_end(argv);
	return res;
}

instr_arg *arg_init_label(label l) {
	instr_arg *res = (instr_arg *) malloc(sizeof(instr_arg));
	res->l = l;
	return res;
}

instr_arg *arg_init_register(reg r) {
	instr_arg *res = (instr_arg *) malloc(sizeof(instr_arg));
	res->r = r;
	return res;
}

instr_arg *arg_init_frame(frame_number f) {
	instr_arg *res = (instr_arg *) malloc(sizeof(instr_arg));
	res->f = f;
	return res;
}

instr_arg *arg_init_expr(expr *e) {
	instr_arg *res = (instr_arg *) malloc(sizeof(instr_arg));
	res->e = e;
	return res;
}

instr *intermediate_instr(node *n, symtable *symbols, frame *current_frame);

instr *intermediate_return(node *expr, symtable *symbols, reg result, label ret) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: return\n");
	if (expr == NULL) {
		instr_arg *a = arg_init_label(ret);
		return instr_init(GOTO, 1, a);
	} else {
		instr_arg *a = arg_init_register(result);
		instr_arg *b = arg_init_expr(intermediate_expr(expr, symbols));
		instr_arg *c = arg_init_label(result);
		instr *i1 = instr_init(REG_ASSIGN, 2, a, b);
		instr *i2 = instr_init(GOTO, 1, c);
		concat(i1, i2);
		return i1;
	}
}

instr *intermediate_if_statement(node *bexpr, node *s1, node *s2, symtable *symbols, frame *current_frame) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: if statement\n");
	instr_arg *lbl1 = arg_init_label(get_fresh_label());
	instr_arg *lbl2 = arg_init_label(get_fresh_label());
	instr_arg *lbl3 = arg_init_label(get_fresh_label());
	instr_arg *condition = arg_init_expr(intermediate_expr(bexpr, symbols));
	instr *i1 = instr_init(JUMP, 3, condition, lbl1, lbl2);
	instr *i2 = instr_init(LABEL, 1, lbl1);
	instr *i3 = intermediate_instr(s1, symbols, current_frame);
	instr *i4 = instr_init(GOTO, 1, lbl3);
	instr *i5 = instr_init(LABEL, 1, lbl2);
	instr *i6 = intermediate_instr(s2, symbols, current_frame);
	instr *i7 = instr_init(GOTO, 1, lbl3);
	instr *i8 = instr_init(LABEL, 1, lbl3);

	concat(i1, i2);
	concat(i2, i3);
	concat(i3, i4);
	concat(i4, i5);
	concat(i5, i6);
	concat(i6, i7);
	concat(i7, i8);
	//concat_many(8, i1, i2, i3, i4, i5, i6, i7, i8);
	return i1;
}

instr *intermediate_while_statement(node *bexpr, node *s, symtable *symbols, frame *current_frame) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: while statement\n");
	instr_arg *lbltest = arg_init_label(get_fresh_label());
	instr_arg *lblloop = arg_init_label(get_fresh_label());
	instr_arg *lblend = arg_init_label(get_fresh_label());
	instr_arg *condition = arg_init_expr(intermediate_expr(bexpr, symbols));
	instr *i1 = instr_init(LABEL, 1, lbltest);
	instr *i2 = instr_init(JUMP, 3, condition, lblloop, lblend);
	instr *i3 = instr_init(LABEL, 1, lblloop);
	instr *i4 = intermediate_instr(s, symbols, current_frame);
	instr *i5 = instr_init(GOTO, 1, lbltest);
	instr *i6 = instr_init(LABEL, 1, lblend);
	
	concat(i1, i2);
	concat(i2, i3);
	concat(i3, i4);
	concat(i4, i5);
	concat(i5, i6);
	//concat_many(6, i1, i2, i3, i4, i5, i6);
	return i1;
}

instr *intermediate_function_call(node *var, node *fun, symtable *symbols) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: fun call\n");
	if (var->nodeType == T_ARRAY_INDEX) {
		printf("ERROR: intermediate code does not handle arrays\n");
		exit(1);
	}
	int i;
	reg r = get_var_register(lookup_var(symbols, var->children[0]->info->label));
	instr_arg *a = arg_init_register(r);
	char *id = fun->children[0]->info->label;
	fun_sym *f = lookup_fun(symbols, id);
	int argc = 0;
	expr **argv = get_params_expr(fun->children[1], symbols, &argc);
	// cannot use the variable argument function here
	instr *res = (instr *) malloc(sizeof(instr));
	res->type = CALL_ASSIGN;
	res->argc = argc + 1;
	res->argv = (instr_arg *) malloc(res->argc * sizeof(instr_arg));
	res->argv[0].f = get_fun_frame(f);
	for (i=1; i<argc; i++)
		res->argv[i].e = argv[i-1];
	res->next = NULL;
	return res;
}

instr *intermediate_procedure_call(node *fun, symtable *symbols) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: procedure call\n");
	int i;
	char *id = fun->children[0]->info->label;
	fun_sym *f = lookup_fun(symbols, id);
	int argc = 0;
	expr **argv = get_params_expr(fun->children[1], symbols, &argc);
	// cannot use the variable argument function here
	instr *res = (instr *) malloc(sizeof(instr));
	res->type = CALL;
	res->argc = argc + 1;
	res->argv = (instr_arg *) malloc(res->argc * sizeof(instr_arg));
	res->argv[0].f = get_fun_frame(f);
	for (i=1; i<argc; i++)
		res->argv[i].e = argv[i-1];
	res->next = NULL;
	return res;
}

instr *intermediate_assign(node *var, node *expr, symtable *symbols) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: assign\n");
	if (var->nodeType == T_ARRAY_INDEX) {
		printf("ERROR: intermediate code does not handle arrays\n");
		exit(1);
	}
	if (expr->nodeType == T_FUNCTION_CALL)
		return intermediate_function_call(var, expr, symbols);
	reg r = get_var_register(lookup_var(symbols, var->info->label));
	instr_arg *a = arg_init_register(r);
	instr_arg *b = arg_init_expr(intermediate_expr(expr, symbols));
	return instr_init(REG_ASSIGN, 2, a, b);
}

void intermediate_insert_vars(node *n, symtable *symbols, var_type t) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: insert vars\n");
	char *id;
	if (n->info->val->intT == 1) {
		id = n->children[0]->info->label;
		insert_var(symbols, id, t);
	} else {
		id = n->children[1]->info->label;
		intermediate_insert_vars(n->children[0], symbols, t);
		insert_var(symbols, id, t);
	}
	var_sym *s;
	s = lookup_var(symbols, id);
	set_var_register(s, get_fresh_register());
}

// does not return anything but inserts a symbol with a register number
void intermediate_vdecl(node *n, symtable *symbols) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: vdecl\n");
	if (n->children[0]->nodeType == T_TYPE_ARRAY) {
		printf("ERROR: intermediate code does not handle arrays\n");
		exit(1);
	}
	if (n->children[0]->nodeType == T_TYPE_INT)
		intermediate_insert_vars(n->children[1], symbols, INTEGER);
	else if (n->children[0]->nodeType == T_TYPE_DECIMAL)
		intermediate_insert_vars(n->children[1], symbols, DECIMAL);
}

void intermediate_vdecl_list(node *n, symtable *symbols) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: vdecl list\n");
	if (n->info->val->intT == 1) {
		intermediate_vdecl(n->children[0], symbols);
	} else {
		intermediate_vdecl_list(n->children[0], symbols);
		intermediate_vdecl(n->children[1], symbols);
	}
}

instr *intermediate_instruction_list(node *n, symtable *symbols, frame *current_frame) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: instruction list\n");
	if (n->info->val->intT == 1)
		return intermediate_instr(n->children[0], symbols, current_frame);
	instr *i1 = intermediate_instruction_list(n->children[0], symbols, current_frame);
	instr *i2 = intermediate_instr(n->children[1], symbols, current_frame);
	concat(i1, i2);
	return i1;
}

instr *intermediate_block_statement(node *n, symtable *symbols, frame *current_frame) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: block\n");
	enter_nested_scope(symbols);
	instr *res;
	if (n->info->val->intT == 0) {
		res = intermediate_instruction_list(n->children[0], symbols, current_frame);
	} else {
		intermediate_vdecl_list(n->children[0], symbols);
		res = intermediate_instruction_list(n->children[1], symbols, current_frame);
	}
	exit_nested_scope(symbols);
	return res;
}

frame *intermediate_function_declaration(node *n, symtable *symbols);

frame *intermediate_procedure_declaration(node *n, symtable *symbols);

frame *intermediate_pdecl(node *n, symtable *symbols) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: pdecl\n");
	switch (n->nodeType) {
		case(T_PROCEDURE_DECLARATION):
			return intermediate_procedure_declaration(n, symbols);
		case(T_FUNCTION_DECLARATION):
			return intermediate_function_declaration(n, symbols);
	}
}

void intermediate_pdecl_list(node *n, symtable *symbols, FILE *f) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: pdecl list\n");
	if (n->info->val->intT == 1) {
		print_frame(intermediate_pdecl(n->children[0], symbols), f);
	} else {
		intermediate_pdecl_list(n->children[0], symbols, f);
		print_frame(intermediate_pdecl(n->children[1], symbols), f);
	}
}

void print_instruction(instr *i, FILE *f);

instr *intermediate_program(node *n, symtable *symbols, FILE *f) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: program\n");
	if (n->info->val->intT == 0)
		print_instruction(intermediate_instr(n->children[0], symbols, NULL), f);
	else {
		intermediate_pdecl_list(n->children[0], symbols, f);
		fprintf(f, "\n");
		print_instruction(intermediate_instr(n->children[1], symbols, NULL), f);
	}
}

instr *intermediate_instr(node *n, symtable *symbols, frame *current_frame) {
  switch(n->nodeType) {
		case(T_RETURN):
		  return intermediate_return(n->children[0], symbols, current_frame->result, current_frame->ret);
		case(T_IF_STATEMENT):
		  return intermediate_if_statement(n->children[0], n->children[1], n->children[2], symbols, current_frame);
		case(T_WHILE_STATEMENT):
		  return intermediate_while_statement(n->children[0], n->children[1], symbols, current_frame);
		case(T_ASSIGN):
		  return intermediate_assign(n->children[0], n->children[1], symbols);
		case(T_FREE):
		case(T_NEW):
			printf("ERROR: intermediate code does not handle arrays\n");
			exit(1);
		case(T_FUNCTION_CALL):
			// called here only for procedure calls
			return intermediate_procedure_call(n, symbols);
		case(T_PARAMETER):
			printf("This shouldn't be called\n"); // this function should not be called on those nodes directly
		case(T_BLOCK_STATEMENT):
			return intermediate_block_statement(n, symbols, current_frame);
		case(T_DECLARATION_STATEMENT):
			intermediate_vdecl(n, symbols);
			break;
		case(T_PROGRAM):
			printf("This shouldn't be called\n"); // this function should not be called on those nodes directly
    case(T_FUNCTION_DECLARATION):
			printf("This shouldn't be called\n"); // this function should not be called on those nodes directly
    case(T_PROCEDURE_DECLARATION):
			printf("This shouldn't be called\n"); // this function should not be called on those nodes directly
    case(T_LIST):
			printf("This shouldn't be called\n"); // this function should not be called on those nodes directly
    default:
  		printf("Unknown error during intermediate code production (instruction)\n");
	}
	return NULL;
}

var_type intermediate_get_param_type(node *n) {
	if(INTERMEDIATE_DEBUG) printf("Intermediate code: get param type\n");
	if (n->children[1]->nodeType == T_TYPE_INT)
		return INTEGER;
	if (n->children[1]->nodeType == T_TYPE_DECIMAL)
		return DECIMAL;
	printf("ERROR: intermediate code does not handle arrays\n");
	exit(1);
	return INTEGER; // default choice
}

void intermediate_get_param_types(node *n, symtable *symbols, var_type *res, int argc) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: get param types\n");
	char *id;
	var_type t;
	if (n->info->val->intT == 1) {
    id = n->children[0]->children[0]->info->label;
		t = intermediate_get_param_type(n->children[0]);
	} else {
	  id = n->children[1]->children[0]->info->label;
	  t = intermediate_get_param_type(n->children[1]);
	}
	insert_var(symbols, id, t);
	set_var_register(lookup_var(symbols, id), get_fresh_register());
	res[argc - 1] = t;
	if (n->info->val->intT != 1)
	  intermediate_get_param_types(n->children[0], symbols, res, argc - 1);
}

frame *intermediate_function_declaration(node *n, symtable *symbols) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: function declaration\n");
	int i;
	char *id = n->children[0]->info->label;
	var_type *argt = (var_type *) malloc(n->info->val->intT * sizeof(var_type *));
	int argc = n->info->val->intT;
	fun_sym *f;
	type returnType;
	enter_nested_scope(symbols);

	// frame init
	frame *res = (frame *) malloc(sizeof(frame));
	res->n = get_fresh_frame();
	res->entry = get_fresh_label() ;
	res->ret = get_fresh_label();
	res->result = get_fresh_register();
	res->argc = argc;
	res->argv = (reg *) malloc(argc * sizeof(reg));
	for (i=0; i<argc; i++)
		res->argv[i] = last_fresh_reg + i + 1; //ugly hack
	res->stack = 0;
	res->instructions = NULL;
	
	if (argc == 0) {
		returnType = n->children[1]->nodeType;
		res->instructions = intermediate_instr(n->children[2], symbols, res);
	} else {
		intermediate_get_param_types(n->children[1], symbols, argt, argc);
		returnType = n->children[2]->nodeType;
		res->instructions = intermediate_instr(n->children[3], symbols, res);
	}
	if (returnType == T_TYPE_INT)
		insert_fun(symbols, id, INTEGER, argc, argt);
	else if (returnType == T_TYPE_DECIMAL)
		insert_fun(symbols, id, DECIMAL, argc, argt);
	else {
		printf("ERROR: intermediate code does not handle arrays\n");
		exit(1);
	}
	
	f = lookup_fun(symbols, id);
	set_fun_frame(f, res->n);
	free(argt);
	exit_nested_scope(symbols);
	return res;
}

frame *intermediate_procedure_declaration(node *n, symtable *symbols) {
	if (INTERMEDIATE_DEBUG) printf("Intermediate code: function declaration\n");
	int i;
	char *id = n->children[0]->info->label;
	var_type *argt = (var_type *) malloc(n->info->val->intT * sizeof(var_type *));
	int argc = n->info->val->intT;
	fun_sym *f;
	enter_nested_scope(symbols);
	
	// frame init
	frame *res = (frame *) malloc(sizeof(frame));
	res->n = get_fresh_frame();
	res->entry = get_fresh_label() ;
	res->ret = get_fresh_label();
	res->result = get_fresh_register();
	res->argc = argc;
	res->argv = (reg *) malloc(argc * sizeof(reg));
	for (i=0; i<argc; i++)
		res->argv[i] = last_fresh_reg + i + 1; //ugly hack
	res->stack = 0;
	res->instructions = NULL;
	
	if (argc == 0) {
		res->instructions = intermediate_instr(n->children[1], symbols, res);
	} else {
		intermediate_get_param_types(n->children[1], symbols, argt, argc);
		res->instructions = intermediate_instr(n->children[2], symbols, res);
	}
	insert_proc(symbols, id, argc, argt);
	f = lookup_fun(symbols, id);
	set_fun_frame(f, res->n);
	free(argt);
	exit_nested_scope(symbols);
	return res;
}

/*****************Print instruction*************************/

void print_instruction(instr *i, FILE *f) {
	if (i == NULL)
		return;
	int j;
	char buf[BUF_SZ];
	switch (i->type) {
		case(LABEL):
			fprintf(f, "%d:\n", i->argv[0].l);
			break;
		case(REG_ASSIGN):
			print_expr(i->argv[1].e, buf);
			fprintf(f, "\t%d := %s\n", i->argv[0].l, buf);
			break;
		case(EXPR_ASSIGN):
			print_expr(i->argv[0].e, buf);
			fprintf(f, "\t%s := ", buf);
			print_expr(i->argv[1].e, buf);
			fprintf(f, "%s\n", buf);
			break;
		case(JUMP):
			print_expr(i->argv[0].e, buf);
			fprintf(f, "\tjump (%s) %d %d\n", buf, i->argv[1].l, i->argv[2].l);
			break;		
		case(GOTO):
			fprintf(f, "\tgoto %d\n", i->argv[0].l);
			break;
		case(CALL):
			fprintf(f, "call %d", i->argv[0].f);
			j = 1;
			while (j < i->argc) {
				print_expr(i->argv[j].e, buf);
				fprintf(f, " %s", buf);
				j++;
			}
			fprintf(f, "\n");
			break;
		case(CALL_ASSIGN):
			print_expr(i->argv[0].e, buf);
			fprintf(f,"%s := call %d", buf, i->argv[1].f);
			j = 2;
			while (j < i->argc) {
				print_expr(i->argv[j].e, buf);
				fprintf(f, " %s", buf);
				j++;
			}
			fprintf(f,"\n");
			break;
	}
	print_instruction(i->next, f);
}

print_frame(frame *frame, FILE *f) {
	int i;
	fprintf(f, "frame(%d), {entry: %d; return: %d; args:}", frame->n, frame->entry, frame->ret);
	for (i = 0; i < frame->argc; i++) {
		fprintf(f, " reg_%d,", frame->argv[i]);
	}
	fprintf(f, "; result: %d; stack: 0}\n", frame->result);
	print_instruction(frame->instructions, f);
}

produce_intermediate_code(node *root) {
	symtable *t = init_symtable();
  FILE *f = fopen("a.out", "w");
  if (f != NULL) {
		intermediate_program(root, t, f);
		printf("Production of intermediate code successful\n");
	} else {
		printf("WARNING: unable to write output file\n");
	}
}
