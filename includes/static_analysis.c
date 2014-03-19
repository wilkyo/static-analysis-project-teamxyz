#include "static_analysis.h"
#include <stdlib.h>

block_list * _blocks;
flow_list * _flow;

int checkTreeBeforeAnalysis(node * root) {
	return 1;
}

/* Constructeurs */

block_list * mk_block_list(block * b, block_list * list) {
	if(list == NULL) {
		list = malloc(sizeof(block_list));
		list->val = b;
		list->next = NULL;
	} else {
		list->next = malloc(sizeof(block_list));
		list->next->val = b;
		list->next->next = NULL;
	}
	return list;
}

block * mk_block(int label, blockType bType, type sType, int assignedVar, int_list * vars) {
	block * res = malloc(sizeof(block));
	res->label = label;
	res->bType = bType;
	res->sType = sType;
	res->assignedVar = assignedVar;
	res->variables = vars;
	return res;
}

block * mk_block_assign(int label, type sType, int assignedVar, int_list * vars) {
	return mk_block(label, B_ASSIGN, sType, assignedVar, vars);
}

block * mk_block_bool_exp(int label, int_list * vars) {
	return mk_block(label, B_BOOL_EXP, T_BINARY_BOOLEAN, 0, vars);
}

block * mk_block_skip(int label) {
	return mk_block(label, B_SKIP, T_SKIP, 0, NULL);
}

/* Getters */

block * getBlockWithLabel(block_list * list, int label) {
	while(list != NULL) {
		if(list->val->label == label)
			return list->val;
		list = list->next;
	}
	return NULL;
}


/* Analysis */

declaration * getVariablesDeclarations(node * root) {
	return NULL;
}

void initScan(node * root) {
	_blocks = mk_block_list(mk_block_skip(42), NULL);
	_blocks = mk_block_list(mk_block_skip(12), _blocks);
}

block_list * getBlocks() {
	return _blocks;
}

flow_list * getFlow() {
	return _flow;
}

flow_list * getFlowR(flow_list * list) {
	return NULL;
}

int getInit(flow_list * list) {
	return 0;
}

int_list * getFinal(flow_list * list) {
	return NULL;
}


void print_blocks(int initial, flow_list * flows, block_list * blocks) {
}

// Return 1 if success
int start_static_analysis(declaration * vars, int initial, int_list * finals, flow_list * flows, flow_list * flowsR, block_list * blocks) {
	printf("Block: %d\n", getBlockWithLabel(blocks, 42)->label);
	if(getBlockWithLabel(blocks, 66) == NULL)
		printf("Block 66 NULL\n");
	return 0;
}

