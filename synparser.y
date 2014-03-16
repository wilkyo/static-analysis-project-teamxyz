%{
	#include <stdio.h>
	int yylex(void);
	void yyerror(const char * msg);
	int line;
	int col;
	extern FILE * yyin;
	#include "includes/node.h"
	#include "includes/dotoutput.h"
	#include "includes/intermediate.h"
	extern node * root;
%}

%code requires{typedef struct node node;}
%expect 1
%union { char* stringT; int intT; float floatT; node *nodeT; }
%token L_PARENT R_PARENT L_BRACKET R_BRACKET
%token COMMA SEMI COLON MINUS PLUS MULT DIV ASSIGN NEQ LE GE LT GT EQ NOT AND OR
%token TYPE_INT TYPE_BOOL TYPE_FLOAT
%token PROGRAM START END RETURN IF THEN ELSE WHILE DO PROCEDURE RES CALL
%token VALUE_TRUE VALUE_FALSE VALUE_INT VALUE_DECIMAL IDENTIFIER
%type <stringT> IDENTIFIER
%type <intT> VALUE_INT VALUE_TRUE VALUE_FALSE
%type <intT> PLUS MINUS MULT DIV LE GE LT GT AND OR NOT EQ NEQ
%type <floatT> VALUE_DECIMAL
%type <nodeT> arithmetic_expression boolean_expression type lhs
%type <nodeT> instruction assign_value function_call statement block_statement program pdecl vdecl
%type <nodeT> pdecl_list vdecl_list expression_list identifier_list parameter_list

%left OR
%left AND
%left EQ NEQ
%left LT GT LE GE
%left PLUS MINUS
%left MULT DIV
%right UNARY
%left POSTFIX

%start program
%%

arithmetic_expression
	: VALUE_INT { $$ = mkleaf_int(line, col, $1); }
	| VALUE_DECIMAL { $$ = mkleaf_decimal(line, col, $1); }
	| IDENTIFIER { $$ = mkleaf_identifier(line, col, $1); }
	| IDENTIFIER L_BRACKET arithmetic_expression R_BRACKET %prec POSTFIX { $$ = mknode_array_index(line, col, mkleaf_identifier(line, col, $1), $3); }
	| MINUS arithmetic_expression %prec UNARY { $$ = mknode_unary_arithmetic(line, col, OP_MINUS, $2); }
	| arithmetic_expression PLUS arithmetic_expression { $$ = mknode_binary_arithmetic(line, col, $1, OP_ADD, $3); }
	| arithmetic_expression MINUS arithmetic_expression { $$ = mknode_binary_arithmetic(line, col, $1, OP_SUB, $3); }
	| arithmetic_expression MULT arithmetic_expression { $$ = mknode_binary_arithmetic(line, col, $1, OP_MULT, $3); }
	| arithmetic_expression DIV arithmetic_expression { $$ = mknode_binary_arithmetic(line, col, $1, OP_DIV, $3); }
	| L_PARENT arithmetic_expression R_PARENT { $$ = $2; }
	;

boolean_expression
  : VALUE_TRUE { $$ = mkleaf_true(line, col); }
	| VALUE_FALSE { $$ = mkleaf_false(line, col); }
	| arithmetic_expression LT arithmetic_expression { $$ = mknode_binary_boolean(line, col, $1, OP_LOWER_THAN, $3); }
	| arithmetic_expression GT arithmetic_expression { $$ = mknode_binary_boolean(line, col, $1, OP_GREATER_THAN, $3); }
	| arithmetic_expression LE arithmetic_expression { $$ = mknode_binary_boolean(line, col, $1, OP_LOWER_EQUALS, $3); }
	| arithmetic_expression GE arithmetic_expression { $$ = mknode_binary_boolean(line, col, $1, OP_GREATER_EQUALS, $3); }
	| arithmetic_expression EQ arithmetic_expression { $$ = mknode_binary_boolean(line, col, $1, OP_EQUALS, $3); }
	| arithmetic_expression NEQ arithmetic_expression { $$ = mknode_binary_boolean(line, col, $1, OP_NOT_EQUALS, $3); }
	| NOT boolean_expression %prec UNARY { $$ = mknode_unary_boolean(line, col, NOT, $2); }
	| boolean_expression AND boolean_expression { $$ = mknode_binary_boolean(line, col, $1, OP_AND, $3); }
	| boolean_expression OR boolean_expression { $$ = mknode_binary_boolean(line, col, $1, OP_OR, $3); }
  | L_PARENT boolean_expression R_PARENT { $$ = $2; }
	;

type
  : TYPE_INT { $$ = mkleaf_type_int(line, col); }
  | TYPE_FLOAT { $$ = mkleaf_type_decimal(line, col); }
  | TYPE_INT L_PARENT VALUE_INT R_PARENT { $$ = mknode_type_array(line, col, mkleaf_int(line, col, $3)); }
	;

lhs
  : IDENTIFIER { $$ = mkleaf_identifier(line, col, $1); }
  | IDENTIFIER L_BRACKET arithmetic_expression R_BRACKET { $$ = mknode_array_index(line, col, mkleaf_identifier(line, col, $1), $3); }
	;

instruction
  : RETURN arithmetic_expression { $$ = mknode_return(line, col, $2); }
  | RETURN { $$ = mknode_return(line, col, NULL); }
  | lhs ASSIGN assign_value { $$ = mknode_assign(line, col, $1, $3); }
  | function_call { $$ = $1; }
  ;

assign_value
  : arithmetic_expression { $$ = $1; }
  | type L_BRACKET arithmetic_expression R_BRACKET { $$ = mknode_new(line, col, $1, $3); }
  | function_call { $$ = $1; }
  ;

function_call
  : IDENTIFIER L_PARENT expression_list R_PARENT { $$ = mknode_function_call(line, col, mkleaf_identifier(line, col, $1), $3); }
  | IDENTIFIER L_PARENT R_PARENT { $$ = mknode_function_call(line, col, mkleaf_identifier(line, col, $1), NULL); }
  ;

expression_list
  : arithmetic_expression { $$ = mknode_list(line, col, NULL, $1); }
  | expression_list COMMA arithmetic_expression { $$ = mknode_list(line, col, $1, $3); }
	;

statement
  : instruction { $$ = $1; }
  | IF boolean_expression THEN statement ELSE statement { $$ = mknode_if(line, col, $2, $4, $6); }
  | WHILE boolean_expression DO L_PARENT block_statement R_PARENT { $$ = mknode_while(line, col, $2, $5) ; }
  | START vdecl_list block_statement END { $$ = mknode_start(line, col, $2, $3); }
  | START block_statement END { $$ = mknode_start(line, col, NULL, $2); }
	;

vdecl_list
  : vdecl { $$ = mknode_list(line, col, NULL, $1); }
  | vdecl_list vdecl { $$ = mknode_list(line, col, $1, $2); }
	;

block_statement
	: statement { $$ = mknode_list(line, col, NULL, $1); }
	| block_statement SEMI statement { $$ = mknode_list(line, col, $1, $3); }
	;

vdecl
  : type identifier_list SEMI { $$ = mknode_vdecl(line, col, $1, $2); }
	;

identifier_list
	: IDENTIFIER { $$ = mknode_list(line, col, NULL, mkleaf_identifier(line, col, $1)); }
	| identifier_list COMMA IDENTIFIER { $$ = mknode_list(line, col, $1, mkleaf_identifier(line, col, $3)); }
	;

program
  : PROGRAM IDENTIFIER pdecl_list statement { $$ = mknode_program(line, col, $2, $3, $4); }
	| PROGRAM IDENTIFIER statement { $$ = mknode_program(line, col, $2, NULL, $3); }
	;

pdecl_list
	: pdecl { $$ = mknode_list(line, col, NULL, $1); }
	| pdecl_list pdecl { $$ = mknode_list(line, col, $1, $2); }
	;

pdecl
  : PROCEDURE IDENTIFIER L_PARENT parameter_list COMMA RES type R_PARENT statement { $$ = mknode_function_declaration(line, col, mkleaf_identifier(line, col, $2), $4, $7, $9); }
  | PROCEDURE IDENTIFIER L_PARENT RES type R_PARENT statement { $$ = mknode_function_declaration(line, col, mkleaf_identifier(line, col, $2), NULL, $5, $7); }
  | PROCEDURE IDENTIFIER L_PARENT parameter_list R_PARENT statement { $$ = mknode_procedure_declaration(line, col, mkleaf_identifier(line, col, $2), $4, $6); }
  | PROCEDURE IDENTIFIER L_PARENT R_PARENT statement { $$ = mknode_procedure_declaration(line, col, mkleaf_identifier(line, col, $2), NULL, $5) ; }
	;

parameter_list
  : IDENTIFIER COLON type { $$ = mknode_list(line, col, NULL, mknode_parameter(line, col, mkleaf_identifier(line, col, $1), $3)); }
	| parameter_list COMMA IDENTIFIER COLON type { $$ = mknode_list(line, col, $1, mknode_parameter(line, col, mkleaf_identifier(line, col, $3), $5)); }
	;
%%
void yyerror(const char * msg) {
	printf( "%d:%d %s\n", line, col, msg);
}

int main(int argc, char ** argv) {
	line = 1;
	col = 0;
	if (argc > 1)
		yyin = fopen(argv[1],"r");
	if(yyin != NULL) {
		if (!yyparse()) {
			printf("Syntaxic analysis successful\n");
			//if (start_semantic_analysis(root))
			//	printf("Semantic analysis successful\n");
			fclose(yyin);
			if(PROD_INTER) // Car ne fonctionne pas pour tous
				produce_intermediate_code(root);
			to_dot("ast.dot", root);
			return 0;
		}
		fclose(yyin);
		return 1;
	} else {
		printf("Unable to open file %s\n", argv[1]);
		return 1;
	}
}
