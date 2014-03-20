#include "static_analysis.h"
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

block_list * mk_block_list(block * b, block_list * list) {
	if(list == NULL) {
		list = malloc(sizeof(block_list));
		list->val = b;
		list->next = NULL;
	} else {
		block_list * nlist = malloc(sizeof(block_list));
		nlist->val = b;
		nlist->next = list;
		list = nlist;
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


/* Getters */

char * getDeclarationNameWithId(declaration * list, int id) {
	while(list != NULL) {
		if(list->vId == id)
			return list->vName;
		list = list->next;
	}
	return NULL;
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


/* Analysis */

void initScan(node * root) {
	// TODO Wk
	_blocks = mk_block_list(mk_block_skip(42), NULL);
	_blocks = mk_block_list(mk_block_skip(12), _blocks);
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
	return ;
}

int_list * getFinal() {
	return _final;
}


void print_blocks(int initial, flow_list * flows, block_list * blocks) {
	// TODO TRT
}

// Return 1 if success
int start_static_analysis(declaration * vars, int initial, int_list * finals, flow_list * flows, flow_list * flowsR, block_list * blocks) {
	printf("Block: %d\n", getBlockWithLabel(blocks, 42)->label);
	if(getBlockWithLabel(blocks, 66) == NULL)
		printf("Block 66 NULL\n");
	// TODO TRT
	return 0;
}

