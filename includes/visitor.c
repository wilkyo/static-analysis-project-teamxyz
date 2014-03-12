#define STRLEN 15

#include "visitor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void * visit_value_true_string(visitor * v, int intT) {
	return "TRUE";
}

void * visit_value_false_string(visitor * v, int intT) {
	return "FALSE";
}

void * visit_value_int_string(visitor * v, int intT) {
	char str[STRLEN];
	snprintf(str, STRLEN, "%d", intT);
	char * res = malloc(strlen(str) * sizeof(char));
	strcpy(res, str);
	return res;
}

void * visit_value_decimal_string(visitor * v, float floatT) {
	char str[STRLEN];
	snprintf(str, STRLEN, "%f", floatT);
	char * res = malloc(strlen(str) * sizeof(char));
	strcpy(res, str);
	return res;
}

void * visit_identifier_string(visitor * v, char * label) {
	return label;
}

void * visit_array_index_string(visitor * v, node * arrayId, node * index) {
	char * resArrayId = (char *) visit(arrayId, v);
	char * resIndex = (char *) visit(index, v);
	char * res = malloc(sizeof(char) * (2 + strlen(resArrayId) + strlen(resIndex)));
	strcpy(res, resArrayId);
	res[strlen(resArrayId)] = '[';
	strcpy(res + strlen(resArrayId) + 1, resIndex);
	res[strlen(resArrayId) + strlen(resIndex) + 1] = ']';
	return res;
}

void * visit_if_string(visitor * v, node * condition, node * true, node * false) {
	printf("There\n");
	/*char * resCond = (char *)visit(condition, v);
	char * resTrue = (char *)visit(true, v);
	char * resFalse = (char *)visit(false, v);
	char * res = malloc(sizeof(char) * (16 + strlen(resCond) + strlen(resTrue) + strlen(resFalse)));
	char decoration1[4] = "if (", decoration2[3] = ") {", decoration3[8] = "} else {", decoration4[1] = "}";
	// strcpy en folie
	return res;
	*/
	return NULL;
}

visitor * createVisitor() {
	visitor * v = malloc(sizeof(visitor));
	v->visit_value_true = &visit_value_true_string;
	v->visit_value_false = &visit_value_false_string;
	v->visit_value_int = &visit_value_int_string;
	v->visit_value_decimal = &visit_value_decimal_string;
	v->visit_identifier = &visit_identifier_string;
	v->visit_array_index = &visit_array_index_string;
	v->visit_if_statement = &visit_if_string;
	return v;
}

void main() {
	printf("BEGIN\n");
	visitor * v = createVisitor();

	node * lid, * narr;
	lid = mkleaf_identifier("x");
	//checkNode(lid);
	narr = mknode_array_index(lid, 7);
	//checkNode(narr);
	printf("%s\n", (char *) visit(narr, v));
	printf("END\n");
}
