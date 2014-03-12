#ifndef VISITOR_H
#define VISITOR_H

#include "node.h"

typedef struct visitor visitor;

struct visitor {
	void * (* visit_value_true)(visitor * v, int intT);
	void * (* visit_value_false)(visitor * v, int intT);
	void * (* visit_value_int)(visitor * v, int intT);
	void * (* visit_value_decimal)(visitor * v, float floatT);
	void * (* visit_identifier)(visitor * v, char * label);
	void * (* visit_array_index)(visitor * v, node * arrayId, node * index);

	void * (* visit_if_statement)(visitor * v, node * condition, node * true, node * false);
	void * (* visit_binary_arithmetic)(visitor * v, node * left, operator op, node * right);
};

void * visit(node * n, visitor * v) {
	switch(n->nodeType) {
		case VALUE_TRUE: return v->visit_value_true(v, n->info->val->intT);
		case VALUE_FALSE: return v->visit_value_false(v, n->info->val->intT);
		case VALUE_INT: return v->visit_value_int(v, n->info->val->intT);
		case VALUE_DECIMAL: return v->visit_value_decimal(v, n->info->val->floatT);
		case IDENTIFIER: return v->visit_identifier(v, n->info->label);
		case ARRAY_INDEX: return v->visit_array_index(v, n->children[0], n->children[1]);

		case IF_STATEMENT: return v->visit_if_statement(v, n->children[0], n->children[1], n->children[2]);
		case BINARY_ARITHMETIC: return v->visit_binary_arithmetic(v, n->children[0], n->info->op, n->children[1]);
	}
}

#endif
