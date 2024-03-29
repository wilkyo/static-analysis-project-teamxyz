#include "static_analysis.h"
#include "MFP_algo.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

declaration * _decls;
block_list * _blocks;
flow_list * _flow;
int _initLabel;
int _lastLabel;
int_list * _final;


int checkTreeBeforeAnalysisChildren(node * parent, int n) {
	int i = 0;
	int res = 0;
	for(;i < n ; i++) {
		if(!checkTreeBeforeAnalysis(parent->children[i]))
			return 0;
	}
	return 1;
}

int checkTreeBeforeAnalysis(node * n) {
	switch(n->nodeType) {
		case(T_DECLARATION_STATEMENT):
			return checkTreeBeforeAnalysisChildren(n, 2);
		case(T_FREE):
			return checkTreeBeforeAnalysisChildren(n, 1);
		case(T_WHILE_STATEMENT):
			return checkTreeBeforeAnalysisChildren(n, 2);
		case(T_IF_STATEMENT):
			return checkTreeBeforeAnalysisChildren(n, 3);
		case(T_RETURN):
			return checkTreeBeforeAnalysisChildren(n, n->children[0] == NULL ? 0 : 1);
		case(T_BLOCK_STATEMENT):
			return checkTreeBeforeAnalysisChildren(n, n->info->val->intT == 0 ? 1 : 2);
		case(T_LIST):
			if (n->info->val->intT > 1)
				return checkTreeBeforeAnalysisChildren(n, 2);
			else if (n->info->val->intT > 0)
				return checkTreeBeforeAnalysisChildren(n, 1);
			break;
		case(T_PROGRAM):
			return checkTreeBeforeAnalysisChildren(n, (n->info->val->intT == 0 ? 1 : 2)
				+ (n->children[0] == NULL ? 0 : 1));
		case(T_TYPE_ARRAY):
		case(T_ARRAY_INDEX) :
			printf("Array detected\n");
			return 0;
		case(T_FUNCTION_CALL):
		case(T_PROCEDURE_DECLARATION):
		case(T_FUNCTION_DECLARATION):
		case(T_PARAMETER):
			printf("Procedure detected\n");
			return 0;
		case(T_VALUE_TRUE):
		case(T_VALUE_FALSE):
		case(T_VALUE_INT):
		case(T_VALUE_DECIMAL):
		case(T_UNARY_ARITHMETIC) :
		case(T_UNARY_BOOLEAN) :
		case(T_BINARY_ARITHMETIC) :
		case(T_BINARY_BOOLEAN) :
		case(T_ASSIGN):
		case(T_NEW):
		default:
			return 1;
	}
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
	char * str = NULL;
	asprintf(&str, "[skip]%d", label);
	return mk_block(label, B_SKIP, T_SKIP, 0, NULL, str);
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
		flow_list * aux = list;
		while(aux->next != NULL) aux = aux->next;
		aux->next = malloc(sizeof(flow_list));
		aux->next->val = f;
		aux->next->next = NULL;
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

int_list * rm_int_list(int val, int_list * list)
{
	if(list != NULL)
	{
		if(list->val == val)
		{
			int_list * aux = list;
			list = aux->next;
			free(aux);
		}
		else
		{
			int_list * aux = list;
			while(aux->next != NULL)
			{
				if(aux->next->val == val)
				{
					int_list * aux2 = aux->next;
					aux->next = aux2->next;
					free(aux2);
					break;
				}	
			}
		}
	}
	return list;
}

/* Destructeur */

void free_int_list(int_list * list)
{
	if(list != NULL)
	{
		free_int_list(list->next);
		free(list);
	}	
}

void free_flow(flow * f)
{
	if(f != NULL)
		free(f);
}

void free_blocks(block * b)
{
	if(b != NULL)
	{
		free_int_list(b->variables);
		free(b->str);
		free(b);
	}
}

void free_blocks_list(block_list * list)
{
	if(list != NULL)
	{
		free_blocks_list(list->next);
		free_blocks(list->val);
		free(list);		
	}
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
		if(!strcmp(list->vName, name))
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

flow_list * getFlowsWithStart(flow_list * list, int start) {
	flow_list * flows = NULL;
	flow_list * tmp = list;
	while(tmp != NULL) {
		if(tmp->val->start == start)
			flows = mk_flow_list(tmp->val, flows);
		tmp = tmp->next;
	}
	return flows;
}

int_list * getFlowsEnd(flow_list * list) {
	flow_list * tmp = list;
	int_list * res = NULL;
	while(tmp != NULL) {
		res = mk_int_list(tmp->val->end, res);
		tmp = tmp->next;
	}
	return res;
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
	if(list != NULL && *list != NULL)
	{
		flow * f = (*list)->val;
		flow_list * aux = (*list);
		(*list) = aux->next;
		free(aux);
		return f;
	}
	return NULL;
}

int pop_int_list(int_list ** list) {
	if(list != NULL && *list != NULL)
	{
		int i = (*list)->val;
		int_list * aux = (*list);
		(*list) = aux->next;
		free(aux);
		return i;
	}
	return -1;
}

int get_last_label(int_list * list) {
	int last = -1;
	while(list != NULL) {
		if(list->val > last) last = list->val;
		list = list->next;
	}
	return last;
}

int get_size_int_list(int_list * list) {
	int size = 0;
	while(list != NULL) {
		size++;
		list = list->next;
	}
	return size;
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

int contains_int_list(int i, int_list * list) {
	if(list != NULL)
	{
		if(i == list->val)
			return 1;
		else
			return contains_int_list(i, list->next);
	}
	return 0;
}

int_list * union_int_list(int_list * l1, int_list * l2) {
	if(l1 == NULL) return l2;
	if(l2 == NULL) return l1;
	int_list * tmp = l2;

	int_list *res = copy_int_list(l1);


	while(tmp != NULL) {
		if(!contains_int_list(tmp->val, l1)) {
			res = mk_int_list(tmp->val, res);
		}
		tmp = tmp->next;
	}
	return res;
}

int_list * copy_int_list(int_list * list)
{
	int_list * res = NULL;
	int_list * aux = list;
	while(aux != NULL)
	{
		res = mk_int_list(aux->val, res);
		aux = aux->next;
	}
	return res;
}

int_list * minus_int_list(int_list * l1, int_list * l2)
{
	if(l1 == NULL) return NULL;
	if(l2 == NULL) return l1;
	int_list * res = copy_int_list(l1);
	int_list * aux = l2;
	while(aux != NULL)
	{
		res = rm_int_list(l2->val, res);
		aux = aux->next;
	}
	return res;
}

/* Analysis */

/**
 * Initialise les déclarations.
 * @param n Node contenant les déclarations.
 */
void initDeclarations(node * n) {
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


int_list * initScanRec(node * n, int_list * lastLabels, int depth);

int_list * initSkipBlock(int_list * lastLabels) {
	int currentLabel = _lastLabel + 1;
	int label;
	//printf("size: %d\n", get_size_int_list(lastLabels));
	while((label = pop_int_list(&lastLabels)) != -1) {
		if(currentLabel != _initLabel) {
			flow * f = mk_flow(label, currentLabel);
			_flow = mk_flow_list(f, _flow);
			//printf("FLOW(%d, %d)\n", f->start, f->end);
		}
	}
	block * b = mk_block_skip(currentLabel);
	_blocks = mk_block_list(b, _blocks);
	lastLabels = mk_int_list(currentLabel, lastLabels);
	_lastLabel++;
	//printf("b: %d\n", b->label);
	return lastLabels;
}

int_list * initBoolExpBlock(node * n, int_list * lastLabels, type sType, int depth) {
	int currentLabel = _lastLabel + 1;
	int label;
	//printf("size: %d\n", get_size_int_list(lastLabels));
	while((label = pop_int_list(&lastLabels)) != -1) {
		if(currentLabel != _initLabel) {
			flow * f = mk_flow(label, currentLabel);
			_flow = mk_flow_list(f, _flow);
			//printf("FLOW(%d, %d)\n", f->start, f->end);
		}
	}
	block * b = mk_block_bool_exp(currentLabel, sType,
		getListVariables(n->children[0]), node_to_string(n->children[0]));
	_blocks = mk_block_list(b, _blocks);
	lastLabels = mk_int_list(currentLabel, lastLabels);
	_lastLabel++;
	//printf("b: %d: %s\n", b->label, b->str);
	if(sType == T_IF_STATEMENT) {
		lastLabels = initScanRec(n->children[1], lastLabels, depth + 1);
		int afterThen = pop_int_list(&lastLabels);
		lastLabels = initScanRec(n->children[2], mk_int_list(b->label, lastLabels), depth + 1);
		lastLabels = mk_int_list(afterThen, lastLabels);
	} else if(sType == T_WHILE_STATEMENT) {
		lastLabels = initScanRec(n->children[1], lastLabels, depth + 1);
		// On rebranche tous les labels de fin de boucle sur la condition de boucle
		while((label = pop_int_list(&lastLabels)) != -1) {
			flow * f = mk_flow(label, b->label);
			_flow = mk_flow_list(f, _flow);
			//printf("FLOW(%d, %d)\n", f->start, f->end);
		}
		lastLabels = mk_int_list(b->label, lastLabels);
	}
	return lastLabels;
}

int_list * initAssignBlock(node * n, int_list * lastLabels, int depth) {
	int currentLabel = _lastLabel + 1;
	int label;
	//printf("size: %d\n", get_size_int_list(lastLabels));
	while((label = pop_int_list(&lastLabels)) != -1) {
		if(currentLabel != _initLabel) {
			flow * f = mk_flow(label, currentLabel);
			_flow = mk_flow_list(f, _flow);
			//printf("FLOW(%d, %d)\n", f->start, f->end);
		}
	}
	block * b = mk_block_assign(currentLabel,
		getDeclarationIdWithName(_decls, n->children[0]->info->label),
		getListVariables(n->children[1]), node_to_string(n->children[1]));
	_blocks = mk_block_list(b, _blocks);
	lastLabels = mk_int_list(currentLabel, lastLabels);
	_lastLabel++;
	//printf("b: %d, %d: %s\n", b->label, b->assignedVar, b->str);
	return lastLabels;
}

int_list * initScanList(node *n, int_list * lastLabels, int depth) {
	//printf("L%d\n", lastLabel);
	//printf("NodeList: %s, %d\n", n->lexeme, n->info->val->intT);
	if (n->info->val->intT == 1) {
		return initScanRec(n->children[0], lastLabels, depth);
	} else {
		lastLabels = initScanList(n->children[0], lastLabels, depth);
		return initScanRec(n->children[1], lastLabels, depth);
	}
}

int_list * initScanRec(node * n, int_list * lastLabels, int depth) {
	//printf("R%d\n", lastLabel);
	//printf("Node[%d]: %s\n", depth, n->lexeme);
	switch(n->nodeType) {
		case(T_SKIP):
			return initSkipBlock(lastLabels);
		case(T_ASSIGN):
			return initAssignBlock(n, lastLabels, depth);
		case(T_LIST):
			return initScanList(n, lastLabels, depth);
		case(T_IF_STATEMENT):
			return initBoolExpBlock(n, lastLabels, T_IF_STATEMENT, depth);
		case(T_WHILE_STATEMENT):
			return initBoolExpBlock(n, lastLabels, T_WHILE_STATEMENT, depth);
		case(T_BLOCK_STATEMENT):
			if(n->info->val->intT == 0)
				return initScanRec(n->children[0], lastLabels, depth + 1);
			else {
				lastLabels = initScanRec(n->children[0], lastLabels, depth + 1);
				return initScanRec(n->children[1], lastLabels, depth + 1);
			}
		case(T_DECLARATION_STATEMENT):
			initDeclarations(n->children[1]);
			return lastLabels;
		case(T_PROGRAM):
			lastLabels = initScanRec(n->children[0], lastLabels, depth + 1);
			// Si gestion des procédures, lastLabels sera modifié et initLabel ne vaudra pas 1
			_initLabel = get_last_label(lastLabels) + 1;
			//lastLabels = mk_int_list(_initLabel, lastLabels);
			return initScanRec(n->children[1], lastLabels, depth + 1);
		default:;
	}
	return lastLabels;
}

void initScan(node * root) {
	// TODO Wk
	_initLabel = 0;
	_lastLabel = 0;
	_final = initScanRec(root, mk_int_list(_lastLabel, NULL), 0);
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
	return _initLabel;
}

int_list * getFinal() {
	return _final;
}

int print_blockSkip(block * b, block_list * blocks, flow_list * flows, int currentLabel, int_list * lastLabels) {
	printf("%s ", b->str);
	return currentLabel + 1;
}

int print_blockAssign(block * b, block_list * blocks, flow_list * flows, int currentLabel, int_list * lastLabels) {
	printf("[%s := %s]%d ", getDeclarationNameWithId(_decls, b->assignedVar), b->str, currentLabel);
	return currentLabel + 1;
}

int getElseEnd(flow_list * flows, int beginElse) {
	int cpt = beginElse;
	flow_list * res = getFlowsWithStart(flows, beginElse);
	flow * f = pop_flow_list(&res);
	while(f->end > beginElse) {
		res = getFlowsWithStart(flows, ++cpt);
		f = pop_flow_list(&res);
		if(f == NULL)
			break;
	}
	return cpt;
}

int print_blockBoolIf(block * b, block_list * blocks, flow_list * flows, int currentLabel, int_list * lastLabels) {
	printf("if [%s]%d then (", b->str, currentLabel);
	flow_list * fls = getFlowsWithStart(flows, currentLabel);
	pop_flow_list(&fls); // flow vers then
	flow * prec = pop_flow_list(&fls); // flow vers else
	int ifEnd = prec->end - 1;
	fls = getFlowsWithStart(flows, ifEnd); // Récupération de l'après if then else
	prec = pop_flow_list(&fls); // afterElse
	int afterElse;
	int elseEnd;
	// Patch en cas de fin de programme par un if then else.
	if(prec == NULL) {
		// On met le dernier
		elseEnd = -1000;
	} else {
		afterElse = prec->end;
		elseEnd = afterElse - 1;
	}
	if(elseEnd < ifEnd)
		elseEnd = getElseEnd(flows, ifEnd + 1);
	//printf("ifEnd %d\n", ifEnd);
	//printf("elseEnd %d\n", elseEnd);
	int_list * ifList = mk_int_list(ifEnd, NULL);
	currentLabel = print_blocksRec(blocks, flows, currentLabel+1, ifList);
	printf(") else (");
	free(ifList);
	ifList = mk_int_list(elseEnd, NULL);
	currentLabel = print_blocksRec(blocks, flows, currentLabel + 1, ifList);
	free(ifList);
	printf(") ; ");
	//printf("CURRENT %d", currentLabel);
	return currentLabel + 1;
}

int print_blockBoolWhile(block * b, block_list * blocks, flow_list * flows, int currentLabel, int_list * lastLabels) {
	printf("while [%s]%d do (", b->str, currentLabel);
	flow_list * fls = getFlowsWithStart(flows, currentLabel);
	pop_flow_list(&fls);
	flow * f = pop_flow_list(&fls);
	int endWhile;
	if(f == NULL) {
		// On met le dernier
		endWhile = -1000;
	} else {
		endWhile = f->end - 1;
	}
	//printf("(%d)\n ", endWhile);
	flow_list * whileList = mk_int_list(endWhile, NULL);
	currentLabel = print_blocksRec(blocks, flows, currentLabel+1, whileList);
	free(whileList);
	printf(") ; ");
	//printf("(%d)\n ", currentLabel);
	return currentLabel + 1;
}

int print_blocksRec(block_list * blocks, flow_list * flows, int currentLabel, int_list * lastLabels) {
	block * b = getBlockWithLabel(blocks, currentLabel);
	if(b == NULL) return currentLabel;
	switch(b->bType) {
		case(B_ASSIGN):
			currentLabel = print_blockAssign(b, blocks, flows, currentLabel, lastLabels);
			break;
		case(B_BOOL_EXP):
			if(b->sType == T_IF_STATEMENT) {
				currentLabel = print_blockBoolIf(b, blocks, flows, currentLabel, lastLabels);
			} else if(b->sType == T_WHILE_STATEMENT) {
				currentLabel = print_blockBoolWhile(b, blocks, flows, currentLabel, lastLabels);
			}
			break;
		case(B_SKIP):
			currentLabel = print_blockSkip(b, blocks, flows, currentLabel, lastLabels);
			break;
	}
	if(contains_int_list(currentLabel - 1, lastLabels))
		return currentLabel - 1;
	return print_blocksRec(blocks, flows, currentLabel, lastLabels);
}

void print_blocks(block_list * blocks, flow_list * flows, int_list * last) {
	int currentLabel = print_blocksRec(blocks, flows, 1, last);
	
	printf("\n");
	//printf("%s\n", getDeclarationNameWithId(_decls, 1));
}

void print_flows(flow_list * list) {
	if(list != NULL) {
		printf("(%d, %d)", list->val->start, list->val->end);
		while(list->next != NULL) {
			list = list->next;
			printf(", (%d, %d)", list->val->start, list->val->end);
		}
		printf("\n");
	}
}

void print_variables( declaration *vars)
{
	declaration *cour = vars;
	while(cour != NULL)
	{
		printf("(%d,%s)",cour->vId, cour->vName);
		cour = cour->next;
	}
	printf("\n");
}

// Return 1 if success
int start_static_analysis(declaration * vars, int initial, int_list * finals, flow_list * flows, flow_list * flowsR, block_list * blocks) {

	//print kill et gen
	
	//print_kill_gen(blocks,vars);

	analysis_list* results = NULL;
	results = MFP(flowsR,finals);
	//affichage de la liste de resultat
	printf("liste des variables : \n");
	print_variables(vars);
	printf("resultat  (label, list_var_id): \n");
	print_analysis_list(results);
	return 1;
}

void print_kill_gen(block_list * list, declaration *dec_list)
{
		block_list *cour  = list;
		printf("label \t| kill \t|gen \n");
		while(cour != NULL)
		{
			if(cour -> val != NULL)
			{
				printf("%d \t |",cour->val->label);
				//recuperation kill et gen
				analysis_block * gen_bloc=NULL;
				mk_analysis_block(&gen_bloc,cour->val->label);
				int_list *l1 = gen(gen_bloc);
				if(l1 == NULL)
					printf("empty ");
				while(l1 != NULL)
				{
					//aller chercher
					if(l1 != NULL)
						printf("%s \t",getDeclarationNameWithId(dec_list,l1->val));
					else
						printf("| empty \t");

					l1 = l1->next;
				}
				//on ne peut kill qu'une seule variable
				analysis_block * kill_bloc=NULL;
					mk_analysis_block(&kill_bloc,cour->val->label);
					int_list *lgen = kill(kill_bloc);
					if(lgen != NULL)
						printf("| %s ",getDeclarationNameWithId(dec_list,lgen->val));
					else
						printf("| empty ");
				printf("\n");
			
				cour = cour -> next;
			}
		}
}
