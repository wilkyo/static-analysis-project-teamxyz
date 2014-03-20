#include "static_analysis.h"
#include <stdio.h>
#include <stdlib.h>

declaration * _decls;
block_list * _blocks;
flow_list * _flow;
int _init;
int_list * _final;



int checkTreeBeforeAnalysis(node * root) {
	// TODO Wk
	return 1;
}

/* Constructeurs */

declaration * mk_declaration(int id, char * name, declaration * list) {
	declaration * nlist = malloc(sizeof(block_list));
	nlist->vId = id;
	nlist->vName = name;
	nlist->next = list;
	return nlist;
}

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

flow * mk_flow(int start, int end) {
	// TODO JB
}

flow_list * mk_flow_list(flow * f, flow_list * list) {
	// TODO JB
}

flow_list * rm_flow_list(flow * f, flow_list * list) {
	// TODO JB
}


/* Getters */

char * getDeclarationNameWithId(declaration * list, int id) {
	while(list != NULL) {
		if(list->vId == id)
			return list->vName;
		list = list->next;
	}
	return NULL;
}

int getDeclarationIdWithName(declaration * list, char * name) {
	while(list != NULL) {
		if(strcmp(list->vName, name))
			return list->vId;
		list = list->next;
	}
	return -1;
}

block * getBlockWithLabel(block_list * list, int label) {
	while(list != NULL) {
		if(list->val->label == label)
			return list->val;
		list = list->next;
	}
	return NULL;
}

int isEmpty_flow_list(flow_list * list) {
	// TODO JB
}

flow * head_flow_list(flow_list * list) {
	// TODO JB
}

// Supprime et retourne le premier élément
flow * pop_flow_list(flow_list ** list) {
	// TODO JB
}


/* Opérations ensemblistes */

int contains(flow * f, flow_list * list) {
	// TODO JB
}


/* Analysis */

/**
 * Initialise les déclarations.
 * @param n Node contenant les déclarations.
 */
declaration * initDeclarations(node * n) {
	int i = 0;
	while(n->info->val->intT > 1) {
		_decls = mk_declaration(i++, n->children[1]->info->label, _decls);
		n = n->children[0];
	}
	if(n->info->val->intT == 1)
		_decls = mk_declaration(i++, n->children[0]->info->label, _decls);
}

int initAssignBlock(node * n, int lastLabel, type sType, int depth) {
	printf("%d\n", lastLabel);
	block * b = mk_block_assign(++lastLabel, sType, getDeclarationIdWithName(_decls, n->children[0]->info->label), NULL);
	printf("b: %d, %d\n", b->label, b->assignedVar);
	initScanRec(n->children[0], lastLabel, depth + 1);
	initScanRec(n->children[1], lastLabel, depth + 1);
	return lastLabel;
}

int initScanRec(node * n, int lastLabel, int depth);

int initScanList(node *n, int lastLabel, int depth) {
	printf("L%d\n", lastLabel);
	printf("NodeList: %s, %d\n", n->lexeme, n->info->val->intT);
	if (n->info->val->intT == 1) {
		return initScanRec(n->children[0], lastLabel, depth);
	} else {
		lastLabel = initScanList(n->children[0], lastLabel, depth);
		return initScanRec(n->children[1], lastLabel, depth);
	}
}

int initScanRec(node * n, int lastLabel, int depth) {
	printf("R%d\n", lastLabel);
	printf("Node[%d]: %s\n", depth, n->lexeme);
	switch(n->nodeType) {
		case(T_VALUE_TRUE):
			break;
		case(T_VALUE_FALSE):
			break;
		case(T_VALUE_INT):
			break;
		case(T_VALUE_DECIMAL):
			break;
		case(T_IDENTIFIER):
			break;
		case(T_ARRAY_INDEX) :
			break;
		case(T_UNARY_ARITHMETIC) :
			break;
		case(T_BINARY_ARITHMETIC) :
			switch(n->info->op) {
				case(OP_ADD):
					break;
				case(OP_SUB):
					break;
				case(OP_MULT):
					break;
				case(OP_DIV):
					break;
			}
			initScanRec(n->children[0], lastLabel, depth + 1);
			initScanRec(n->children[1], lastLabel, depth + 1);
			break;
		case(T_UNARY_BOOLEAN) :
			;
		case(T_BINARY_BOOLEAN) :
			switch(n->info->op) {
				case(OP_LOWER_THAN):
			;
				case(OP_GREATER_THAN):
			;
				case(OP_LOWER_EQUALS):
			;
				case(OP_GREATER_EQUALS):
			;
				case(OP_EQUALS):
			;
				case(OP_NOT_EQUALS):
			;
				case(OP_AND):
			;
				case(OP_OR):
			;
			}
			lastLabel = initScanRec(n->children[0], lastLabel, depth + 1);
			return initScanRec(n->children[1], lastLabel, depth + 1);
			break;
		case(T_SKIP):
			break;
		case(T_TYPE_INT):
			break;
		case(T_TYPE_DECIMAL):
			break;
		case(T_TYPE_ARRAY):
			break;
		case(T_RETURN):
			break;
		case(T_ASSIGN):
			return initAssignBlock(n, lastLabel, T_ASSIGN, depth);
		case(T_FREE):
			break;
		case(T_NEW):
			break;
		case(T_FUNCTION_CALL):
			break;
		case(T_PARAMETER):
			break;
		case(T_LIST):
			return initScanList(n, lastLabel, depth);
		case(T_IF_STATEMENT):
			break;
		case(T_WHILE_STATEMENT):
			break;
		case(T_BLOCK_STATEMENT):
			lastLabel = initScanRec(n->children[0], lastLabel, depth + 1);
			return initScanRec(n->children[1], lastLabel, depth + 1);
			break;
		case(T_DECLARATION_STATEMENT):
			initDeclarations(n->children[1]);
			return lastLabel;
		case(T_PROGRAM):
			lastLabel = initScanRec(n->children[0], lastLabel, depth + 1);
	printf("P%d\n", lastLabel);
			return initScanRec(n->children[1], lastLabel, depth + 1);
		break;
    case(T_FUNCTION_DECLARATION):
			;
    case(T_PROCEDURE_DECLARATION):
			;
	}
	return lastLabel;
}

void initScan(node * root) {
	// TODO Wk
	initScanRec(root, 0, 0);
	//_blocks = mk_block_list(mk_block_skip(42), NULL);
	//_blocks = mk_block_list(mk_block_skip(12), _blocks);
}

declaration * getVariablesDeclarations() {
	return _decls;
}

block_list * getBlocks() {
	return _blocks;
}

flow_list * getFlow() {
	return _flow;
}

flow_list * getFlowR(flow_list * list) {
	// TODO JB
	return NULL;
}

int getInit(flow_list * list) {
	return _init;
}

void initFinal(flow_list * list) {
	// TODO Wk
}

int_list * getFinal() {
	return _final;
}


void print_blocks(int initial, flow_list * flows, block_list * blocks) {
	// TODO TRT
}

// Return 1 if success
int start_static_analysis(declaration * vars, int initial, int_list * finals, flow_list * flows, flow_list * flowsR, block_list * blocks) {
	//printf("Block: %d\n", getBlockWithLabel(blocks, 42)->label);
	//if(getBlockWithLabel(blocks, 66) == NULL)
	//	printf("Block 66 NULL\n");
	// TODO TRT
	return 0;
}

