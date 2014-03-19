%{
	#include <stdio.h>
	int yylex(void);
	void yyerror(const char * msg);
	int line;
	int col;
	extern FILE * yyin;
	#include "includes/node.h"
	#include "includes/static_analysis.h"
	#include "includes/dotoutput.h"
	extern node * root;
%}

%code requires{typedef struct node node;}
%expect 1
%union { char* stringT; int intT; float floatT; node *nodeT; }
%token L_PARENT R_PARENT L_BRACKET R_BRACKET
%token COMMA SEMI COLON MINUS PLUS MULT DIV ASSIGN NEQ LE GE LT GT EQ NOT AND OR
%token TYPE_INT TYPE_BOOL TYPE_FLOAT
%token PROGRAM START END RETURN IF THEN ELSE WHILE DO PROCEDURE RES CALL SKIP
%token VALUE_TRUE VALUE_FALSE VALUE_INT VALUE_DECIMAL IDENTIFIER
%type <stringT> IDENTIFIER
%type <intT> VALUE_INT VALUE_TRUE VALUE_FALSE
%type <intT> PLUS MINUS MULT DIV LE GE LT GT AND OR NOT EQ NEQ
%type <floatT> VALUE_DECIMAL
%type <nodeT> arithmetic_expression boolean_expression type ident
%type <nodeT> aexpr statement statements block program pdecl vdecl
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

program
	: PROGRAM START vdecl_list statements END { $$ = mknode_program(line, col, NULL, NULL, mknode_start(line, col, $3, $4)); }
	| PROGRAM pdecl_list START vdecl_list statements END { $$ = mknode_program(line, col, NULL, $2, mknode_start(line, col, $4, $5)); }
	| PROGRAM IDENTIFIER START vdecl_list statements END { $$ = mknode_program(line, col, mkleaf_identifier(line, col, $2), NULL, mknode_start(line, col, $4, $5)); }
	| PROGRAM IDENTIFIER pdecl_list START vdecl_list statements END { $$ = mknode_program(line, col, mkleaf_identifier(line, col, $2), $3, mknode_start(line, col, $5, $6)); }
	;

pdecl_list
	: pdecl { $$ = mknode_list(line, col, NULL, $1); }
	| pdecl_list pdecl { $$ = mknode_list(line, col, $1, $2); }
	;

pdecl
	: PROCEDURE IDENTIFIER L_PARENT parameter_list COMMA RES type IDENTIFIER R_PARENT START statements END { $$ = mknode_function_declaration(line, col, mkleaf_identifier(line, col, $2), $4, mknode_parameter(line, col, mkleaf_identifier(line, col, $8), $7), $11); }
	| PROCEDURE IDENTIFIER L_PARENT parameter_list R_PARENT START statements END { $$ = mknode_procedure_declaration(line, col, mkleaf_identifier(line, col, $2), $4, $7); }
	;

parameter_list
	: type IDENTIFIER { $$ = mknode_list(line, col, NULL, mknode_parameter(line, col, mkleaf_identifier(line, col, $2), $1)); }
	| parameter_list COMMA type IDENTIFIER { $$ = mknode_list(line, col, $1, mknode_parameter(line, col, mkleaf_identifier(line, col, $4), $3)); }
	;

vdecl_list
	: vdecl { $$ = mknode_list(line, col, NULL, $1); }
	| vdecl_list vdecl { $$ = mknode_list(line, col, $1, $2); }
	;

vdecl
	: type identifier_list SEMI { $$ = mknode_vdecl(line, col, $1, $2); }
	;

identifier_list
	: IDENTIFIER { $$ = mknode_list(line, col, NULL, mkleaf_identifier(line, col, $1)); }
	| identifier_list COMMA IDENTIFIER { $$ = mknode_list(line, col, $1, mkleaf_identifier(line, col, $3)); }
	;

type
	: TYPE_INT { $$ = mkleaf_type_int(line, col); }
	| TYPE_BOOL { $$ = mkleaf_type_bool(line, col); }
	| TYPE_FLOAT { $$ = mkleaf_type_decimal(line, col); }
	| TYPE_INT L_BRACKET VALUE_INT R_BRACKET { $$ = mknode_type_array(line, col, mkleaf_int(line, col, $3)); }
	;

block
	: statement { $$ = $1; }
	| L_PARENT statements R_PARENT { $$ = mknode_start(line, col, NULL, $2); }
	;

statements
	: statement { $$ = mknode_list(line, col, NULL, $1); }
	| statements SEMI statement { $$ = mknode_list(line, col, $1, $3); }
	;

statement
	: SKIP { $$ = mkleaf_skip(line, col); }
	| ident ASSIGN aexpr { $$ = mknode_assign(line, col, $1, $3); }
	| IF boolean_expression THEN block { $$ = mknode_if(line, col, $2, $4, NULL); }
	| IF boolean_expression THEN block ELSE block { $$ = mknode_if(line, col, $2, $4, $6); }
	| WHILE boolean_expression DO block { $$ = mknode_while(line, col, $2, $4) ; }
	| CALL IDENTIFIER L_PARENT expression_list R_PARENT { $$ = mknode_function_call(line, col, mkleaf_identifier(line, col, $2), $4); }
	;

ident
	: IDENTIFIER { $$ = mkleaf_identifier(line, col, $1); }
	| IDENTIFIER L_BRACKET aexpr R_BRACKET { $$ = mknode_array_index(line, col, mkleaf_identifier(line, col, $1), $3); }
	;

aexpr
	: VALUE_INT { $$ = mkleaf_int(line, col, $1); }
	| VALUE_DECIMAL { $$ = mkleaf_decimal(line, col, $1); }
	| IDENTIFIER { $$ = mkleaf_identifier(line, col, $1); }
	| IDENTIFIER L_BRACKET arithmetic_expression R_BRACKET %prec POSTFIX { $$ = mknode_array_index(line, col, mkleaf_identifier(line, col, $1), $3); }
	| aexpr PLUS aexpr { $$ = mknode_binary_arithmetic(line, col, $1, OP_ADD, $3); }
	| aexpr MINUS aexpr { $$ = mknode_binary_arithmetic(line, col, $1, OP_SUB, $3); }
	| aexpr MULT aexpr { $$ = mknode_binary_arithmetic(line, col, $1, OP_MULT, $3); }
	| aexpr DIV aexpr { $$ = mknode_binary_arithmetic(line, col, $1, OP_DIV, $3); }
	;

expression_list
	: arithmetic_expression { $$ = mknode_list(line, col, NULL, $1); }
	| expression_list COMMA arithmetic_expression { $$ = mknode_list(line, col, $1, $3); }
	;

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
			fclose(yyin);
	
			if(checkTreeBeforeAnalysis(root)) {
				// Analyse statique
				declaration * vars = getVariablesDeclarations(root);
				initScan(root);
				block_list * blocks = getBlocks();
				flow_list * flows = getFlow();
				flow_list * flowsR = getFlowR(flows);
				int initial = getInit(flows);
				int_list * finals = getFinal(flows);

				if (start_static_analysis(vars, initial, finals, flows, flowsR, blocks))
					printf("Static analysis successful\n");
				else
					printf("Static analysis failed\n");
			}

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
