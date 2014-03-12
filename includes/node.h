#ifndef NODE_H
#define NODE_H

typedef union value value;
typedef union attribute attribute;
typedef enum operator operator;
typedef enum type type;
typedef struct node node;

node * root;

union value {
	int intT;
	float floatT;
};

enum operator {
	OP_MINUS, OP_ADD, OP_SUB, OP_MULT, OP_DIV,
	OP_LOWER_THAN, OP_GREATER_THAN, OP_LOWER_EQUALS, OP_GREATER_EQUALS,
	OP_EQUALS, OP_NOT_EQUALS, OP_NOT, OP_AND, OP_OR
};

union attribute {
	value * val;
	char * label;
	operator op;
};

enum type {
	T_VALUE_TRUE, T_VALUE_FALSE, T_VALUE_INT, T_VALUE_DECIMAL, T_IDENTIFIER, T_ARRAY_INDEX,
	T_TYPE_INT, T_TYPE_DECIMAL, T_TYPE_ARRAY, T_RETURN, T_ASSIGN, T_FREE, T_NEW,
	T_FUNCTION_CALL, T_PARAMETER, T_LIST,
	T_UNARY_ARITHMETIC, T_BINARY_ARITHMETIC, T_UNARY_BOOLEAN, T_BINARY_BOOLEAN,
	T_IF_STATEMENT, T_WHILE_STATEMENT, T_BLOCK_STATEMENT, T_DECLARATION_STATEMENT, T_PROGRAM,
	T_FUNCTION_DECLARATION, T_PROCEDURE_DECLARATION
};

struct node {
	char * lexeme;
	type nodeType;
	attribute * info;
	node ** children;
	int line;
	int column;
};


/**************** Internal methods ********************/

/**
 * \fn node * mk_leaf_id(int line, int col, char * lexeme, char * label, type nodeType);
 * \brief Creates a node with its identifier, the label and its type.
 *
 * \param lexeme The identifier of the node
 * \param label The name of the id
 * \param nodeType The type of the node
 * \return A pointer to the node created
 */
node * mk_leaf_id(int line, int col, char * lexeme, char * label, type nodeType);

/**
 * \fn node * mk_leaf_cst(int line, int col, char * lexeme, value * val, type nodeType);
 * \brief Creates a node with its identifier, the value and its type.
 *
 * \param lexeme The identifier of the node
 * \param val The value of the node
 * \param nodeType The type of the node
 * \return A pointer to the node created
 */
node * mk_leaf_cst(int line, int col, char * lexeme, value * val, type nodeType);

/**
 * \fn node * mk_leaf_type(int line, int col, char * lexeme, type nodeType);
 * \brief Creates a type node with its identifier and its type.
 *
 * \param lexeme The identifier of the node
 * \param nodeType The type of the node
 * \return A pointer to the node created
 */
node * mk_leaf_type(int line, int col, char * lexeme, type nodeType);

/**
 * \fn node * mk_node(int line, int col, char * lexeme, attribute * info, type nodeType, node ** children);
 * \brief Creates a node with its identifier, the attribute, its type and its children.
 *
 * \param lexeme The identifier of the node
 * \param info The attribute of the node
 * \param nodeType The type of the node
 * \param children The nodes under itself in the tree
 * \return A pointer to the node created
 */
node * mk_node(int line, int col, char * lexeme, attribute * info, type nodeType, node ** children);


/*********************   Methods for the parser   *********************/

/**
 * \fn node * mkleaf_int(int line, int col, int intT);
 * \brief Creates a node of type integer.
 *
 * \param intT The value of the integer
 * \return A pointer to the node created
 */
node * mkleaf_int(int line, int col, int intT);

/**
 * \fn node * mkleaf_decimal(int line, int col, float floatT);
 * \brief Creates a node of type decimal.
 *
 * \param floatT The value of the decimal
 * \return A pointer to the node created
 */
node * mkleaf_decimal(int line, int col, float floatT);

/**
 * \fn node * mkleaf_identifier(int line, int col, char * identifier);
 * \brief Creates a node of type identifier.
 *
 * \param identifier The label of the identifier
 * \return A pointer to the node created
 */
node * mkleaf_identifier(int line, int col, char * identifier);

/**
 * \fn node * mkleaf_true(int line, int col, );
 * \brief Creates a node of type boolean true.
 *
 * \return A pointer to the node created
 */
node * mkleaf_true(int line, int col);

/**
 * \fn node * mkleaf_false(int line, int col, );
 * \brief Creates a node of type boolean false.
 *
 * \return A pointer to the node created
 */
node * mkleaf_false(int line, int col);

/**
 * \fn node * mkleaf_type_int(int line, int col, );
 * \brief Creates a node of type integer type.
 *
 * \return A pointer to the node created
 */
node * mkleaf_type_int(int line, int col);

/**
 * \fn node * mkleaf_type_decimal(int line, int col, );
 * \brief Creates a node of type decimal type.
 *
 * \return A pointer to the node created
 */
node * mkleaf_type_decimal(int line, int col);

/**
 * \fn node * mknode_type_array(int line, int col, node * expr);
 * \brief Creates a node of type array type.
 * 
 * \param expr The array's length
 * \return A pointer to the node created
 */
node * mknode_type_array(int line, int col, node * expr);

/**
 * \fn node * mknode_return(int line, int col, node * expr);
 * \brief Creates a node of type return.
 * 
 * \param expr The returned element
 * \return A pointer to the node created
 */
node * mknode_return(int line, int col, node * expr);

/**
 * \fn node * mknode_assign(int line, int col, node * left, node * right);
 * \brief Creates a node of type assign.
 * 
 * \param left The left operand
 * \param right The right operand
 * \return A pointer to the node created
 */
node * mknode_assign(int line, int col, node * left, node * right);

/**
 * \fn node * mknode_free(int line, int col, node * expr);
 * \brief Creates a node of type free.
 * 
 * \param expr The freed element
 * \return A pointer to the node created
 */
node * mknode_free(int line, int col, node * expr);

/**
 * \fn node * mknode_new(int line, int col, node * eltType, node * expr);
 * \brief Creates a node of type new.
 * 
 * \param eltType The element type
 * \param expr The instanciation
 * \return A pointer to the node created
 */
node * mknode_new(int line, int col, node * eltType, node * expr);

/**
 * \fn node * mknode_function_call(int line, int col, node * identifier, node * params);
 * \brief Creates a node of type function call.
 * 
 * \param identifier The identifier of the function
 * \param params The parameters list
 * \return A pointer to the node created
 */
node * mknode_function_call(int line, int col, node * identifier, node * params);

/**
 * \fn node * mknode_parameter(int line, int col, node * idParam, node * typeParam);
 * \brief Creates a node of type parameter.
 * 
 * \param idParam The identifier of the parameter
 * \param typeParam The type of the parameter
 * \return A pointer to the node created
 */
node * mknode_parameter(int line, int col, node * idParam, node * typeParam);

/**
 * \fn node * mknode_list(int line, int col, node * list, node * expr);
 * \brief Creates a node of type list.
 * 
 * \param list The list including the expression
 * \param expr The expression to include in the list
 * \return A pointer to the node created
 */
node * mknode_list(int line, int col, node * list, node * expr);

/**
 * \fn node * mknode_array_index(int line, int col, node * array, int index);
 * \brief Creates a node of type array.
 *
 * \param arrayId The identifier of the array
 * \param index The arithmetic expression used in the array
 * \return A pointer to the node created
 */
node * mknode_array_index(int line, int col, node * arrayId, node * index);

/**
 * \fn node * mknode_unary_arithmetic(int line, int col, operator op, node * expr);
 * \brief Creates a node of type unary arithmetic.
 *
 * \param op The operator of the unary expression
 * \param expr The expression affected by the unary
 * \return A pointer to the node created
 */
node * mknode_unary_arithmetic(int line, int col, operator op, node * expr);

/**
 * \fn node * mknode_binary_arithmetic(int line, int col, node * left, operator op, node * right);
 * \brief Creates a node of type binary arithmetic.
 *
 * \param left The left expression affected by the binary
 * \param op The operator of the binary expression
 * \param right The right expression affected by the binary
 * \return A pointer to the node created
 */
node * mknode_binary_arithmetic(int line, int col, node * left, operator op, node * right);

/**
 * \fn node * mknode_unary_boolean(int line, int col, operator op, node * expr);
 * \brief Creates a node of type unary boolean.
 *
 * \param op The operator of the unary expression
 * \param expr The expression affected by the unary
 * \return A pointer to the node created
 */
node * mknode_unary_boolean(int line, int col, operator op, node * expr);

/**
 * \fn node * mknode_binary_boolean(int line, int col, node * left, operator op, node * right);
 * \brief Creates a node of type binary boolean.
 *
 * \param left The left expression affected by the binary
 * \param op The operator of the binary expression
 * \param right The right expression affected by the binary
 * \return A pointer to the node created
 */
node * mknode_binary_boolean(int line, int col, node * left, operator op, node * right);

/**
 * \fn node * mknode_if(int line, int col, node * boolExpr, node * thenExpr, node * exprElse);
 * \brief Creates a node of type if statement.
 *
 * \param boolExpr The boolean expression
 * \param thenExpr The statement if its true
 * \param elseExpr The statement if its false
 * \return A pointer to the node created
 */
node * mknode_if(int line, int col, node * boolExpr, node * thenExpr, node * elseExpr);

/**
 * \fn node * mknode_while(int line, int col, node * boolExpr, node * statementExpr);
 * \brief Creates a node of type while statement.
 *
 * \param boolExpr The boolean expression
 * \param statementExpr The statement while its true
 * \return A pointer to the node created
 */
node * mknode_while(int line, int col, node * boolExpr, node * statExpr);

/**
 * \fn node * mknode_start(int line, int col, node * declList, node * blockStatement);
 * \brief Creates a node of type block statement.
 *
 * \param declList The declarations list
 * \param blockStatement The statements
 * \return A pointer to the node created
 */
node * mknode_start(int line, int col, node * declList, node * blockStatement);

/**
 * \fn node * mknode_vdecl(int line, int col, node * typeDecl, node * idList);
 * \brief Creates a node of type declaration of identifiers.
 *
 * \param typeDecl The declarations type
 * \param idList The Identifiers list
 * \return A pointer to the node created
 */
node * mknode_vdecl(int line, int col, node * typeDecl, node * idList);

/**
 * \fn node * mknode_program(int line, int col, node * declList, node * statExpr);
 * \brief Creates a node of type program.
 *
 * \param declList The declarations list of the program
 * \param statExpr The statement of the program
 * \return A pointer to the node created
 */
node * mknode_program(int line, int col, node * declList, node * statExpr);

/**
 * \fn node * mknode_function_declaration(int line, int col, node * id, node * paramsList, node * returnType, node * statExpr);
 * \brief Creates a node of type function declaration.
 *
 * \param id The identifier of the function
 * \param paramsList The parameters list
 * \param returnType The return type
 * \param statExpr The statement of the function
 * \return A pointer to the node created
 */
node * mknode_function_declaration(int line, int col, node * id, node * paramsList, node * returnType, node * statExpr);

/**
 * \fn node * mknode_procedure_declaration(int line, int col, node * id, node * paramsList, node * statExpr);
 * \brief Creates a node of type procedure declaration.
 *
 * \param id The identifier of the procedure
 * \param paramsList The parameters list
 * \param statExpr The statement of the procedure
 * \return A pointer to the node created
 */
node * mknode_procedure_declaration(int line, int col, node * id, node * paramsList, node * statExpr);

int start_semantic_analysis(node *root) ;

#endif
