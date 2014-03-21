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
	declaration * nlist = malloc(sizeof(declaration));
	nlist->vId = id;
	nlist->vName = name;
	nlist->next = list;
	return nlist;
}

block_list * mk_block_list(block * b, block_list * list) {
	block_list * nlist = malloc(sizeof(block_list));
	nlist->val = b;
	nlist->next = list;
	return nlist;
}

block * mk_block(int label, blockType bType, type sType, int assignedVar, int_list * vars, char * str) {
	block * res = malloc(sizeof(block));
	res->label = label;
	res->bType = bType;
	res->sType = sType;
	res->assignedVar = assignedVar;
	res->variables = vars;
	res->str = str;
	return res;
}

block * mk_block_assign(int label, int assignedVar, int_list * vars, char * str) {
	return mk_block(label, B_ASSIGN, T_ASSIGN, assignedVar, vars, str);
}

block * mk_block_bool_exp(int label, type sType, int_list * vars, char * str) {
	return mk_block(label, B_BOOL_EXP, sType, 0, vars, str);
}

block * mk_block_skip(int label) {
	return mk_block(label, B_SKIP, T_SKIP, 0, NULL, NULL);
}

flow * mk_flow(int start, int end) {
	flow * f = malloc(sizeof(flow));
	f->start =start;
	f->end = end;
	return f;
}

int isEqual_flow(flow * f1, flow * f2)
{
	if(f1 != NULL && f2 != NULL)
		return ((f1->start == f2->start) && (f1->end == f2->end));
	else
		return 0;
}

flow_list * mk_flow_list(flow * f, flow_list * list) {
	if(list == NULL)
	{
		list = malloc(sizeof(flow_list));
		list->val=f;
		list->next=NULL;
	}
	else
	{
		flow_list * nlist = malloc(sizeof(flow_list));
		nlist->val = f;
		nlist->next = list;
		list = nlist;
	}
	return list;
}

flow_list * rm_flow_list(flow * f, flow_list * list) {
	if(list != NULL && f != NULL)
	{
		flow_list * aux = list;
		if(isEqual_flow(list->val,f)==1)
		{
			list = aux->next;
			free(aux->val);
			free(aux);
		}
		else
		{
			while(aux->next != NULL)
			{
				if(isEqual_flow(aux->next->val,f) == 1)
				{
					flow_list * aux2 = aux;
					aux->next = aux->next->next;
					free(aux2->val);
					free(aux);
					break;
				}
			}
		}
	}
	return list;
}

int_list * mk_int_list(int val, int_list * list) {
	int_list * nlist = malloc(sizeof(block_list));
	nlist->val = val;
	nlist->next = list;
	return nlist;
}

int_list * rm_int_list(int_list * list)
{
	if(list != NULL)
	{
		rm_int_list(list->next);
		free(list);
	}
	return NULL;
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
	return (list == NULL);
}

flow * head_flow_list(flow_list * list) {
	if(list != NULL)
		return list->val;
	return NULL;
}

// Supprime et retourne le premier élément
flow * pop_flow_list(flow_list ** list) {
	if(list != NULL)
	{
		flow * f = (*list)->val;
		flow_list * aux = (*list);
		list = &(aux->next);
		free(aux);
		return f;
	}
	return NULL;
}


/* Opérations ensemblistes */

int contains(flow * f, flow_list * list) {
	if(list != NULL)
	{
		if(isEqual_flow(f,list->val) == 1)
			return 1;
		else
			return contains(f,list->next);
	}
	return 0;
}

int_list * union_int_list(int_list * l1, int_list * l2) {
	if(l1 == NULL) return l2;
	if(l2 == NULL) return l1;
	int_list * tmp = l1;
	while(tmp->next != NULL) {
		tmp = tmp->next;
	}
	tmp->next = l2;
	return l1;
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

/**
 * Retourne la liste d'id des variables de l'expression.
 */
int_list * getListVariables(node * n) {
	switch(n->nodeType) {
		case(T_IDENTIFIER) :
			return mk_int_list(getDeclarationIdWithName(_decls, n->info->label), NULL);
		case(T_UNARY_ARITHMETIC) :
			return getListVariables(n->children[0]);
		case(T_BINARY_ARITHMETIC) :
			return union_int_list(getListVariables(n->children[0]), getListVariables(n->children[1]));
		case(T_UNARY_BOOLEAN) :
			return getListVariables(n->children[0]);
		case(T_BINARY_BOOLEAN) :
			return union_int_list(getListVariables(n->children[0]), getListVariables(n->children[1]));
		default:
			return NULL;
	}
}

int initSkipBlock(int lastLabel) {
	block * b = mk_block_skip(++lastLabel);
	_blocks = mk_block_list(b, _blocks);
	printf("b: %d\n", b->label);
	return lastLabel;
}

int initBoolExpBlock(node * n, int lastLabel, type sType, int depth) {
	block * b = mk_block_bool_exp(++lastLabel, sType,
		getListVariables(n->children[0]), node_to_string(n->children[0]));
	_blocks = mk_block_list(b, _blocks);
	printf("b: %d: %s\n", b->label, b->str);
	if(sType == T_IF_STATEMENT) {
		lastLabel = initScanRec(n->children[1], lastLabel, depth + 1);
		lastLabel = initScanRec(n->children[2], lastLabel, depth + 1);
	} else if(sType == T_WHILE_STATEMENT) {
		lastLabel = initScanRec(n->children[1], lastLabel, depth + 1);
	}
	return lastLabel;
}

int initAssignBlock(node * n, int lastLabel, int depth) {
	block * b = mk_block_assign(++lastLabel,
		getDeclarationIdWithName(_decls, n->children[0]->info->label),
		getListVariables(n->children[1]), node_to_string(n->children[1]));
	_blocks = mk_block_list(b, _blocks);
	printf("b: %d, %d: %s\n", b->label, b->assignedVar, b->str);
	return lastLabel;
}

int initScanRec(node * n, int lastLabel, int depth);

int initScanList(node *n, int lastLabel, int depth) {
	//printf("L%d\n", lastLabel);
	printf("NodeList: %s, %d\n", n->lexeme, n->info->val->intT);
	if (n->info->val->intT == 1) {
		return initScanRec(n->children[0], lastLabel, depth);
	} else {
		lastLabel = initScanList(n->children[0], lastLabel, depth);
		return initScanRec(n->children[1], lastLabel, depth);
	}
}

int initScanRec(node * n, int lastLabel, int depth) {
	//printf("R%d\n", lastLabel);
	printf("Node[%d]: %s\n", depth, n->lexeme);
	switch(n->nodeType) {
		case(T_SKIP):
			return initSkipBlock(lastLabel);
		case(T_ASSIGN):
			return initAssignBlock(n, lastLabel, depth);
		case(T_LIST):
			return initScanList(n, lastLabel, depth);
		case(T_IF_STATEMENT):
			return initBoolExpBlock(n, lastLabel, T_IF_STATEMENT, depth);
		case(T_WHILE_STATEMENT):
			return initBoolExpBlock(n, lastLabel, T_WHILE_STATEMENT, depth);
		case(T_BLOCK_STATEMENT):
			if(n->info->val->intT == 0)
				return initScanRec(n->children[0], lastLabel, depth + 1);
			else {
				lastLabel = initScanRec(n->children[0], lastLabel, depth + 1);
				return initScanRec(n->children[1], lastLabel, depth + 1);
			}
		case(T_DECLARATION_STATEMENT):
			initDeclarations(n->children[1]);
			return lastLabel;
		case(T_PROGRAM):
			lastLabel = initScanRec(n->children[0], lastLabel, depth + 1);
			return initScanRec(n->children[1], lastLabel, depth + 1);
	}
	return lastLabel;
}

void initScan(node * root) {
	// TODO Wk
	initScanRec(root, 0, 0);
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
	flow_list * res = NULL;
	if(list != NULL)
	{
		flow * f = mk_flow(list->val->end,list->val->start);
		res = mk_flow_list(f,res);
		res->next = getFlowR(list->next);
		return res;	
	}
	return res;
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

