#ifndef STATIC_ANALYSIS_H
#define STATIC_ANALYSIS_H
#include "node.h"

typedef enum blockType blockType;
typedef struct declaration declaration;
typedef struct int_list int_list;
typedef struct block_list block_list;
typedef struct block block;
typedef struct flow_list flow_list;
typedef struct flow flow;


/* Types */

// 3 types de blocks.
enum blockType {
	B_ASSIGN, B_BOOL_EXP, B_SKIP
};

/*
 * declaration : liste declaration
 * int x, y, z;
 * On associe chaque variable à un identifieur entier pour l'identifier dans les blocks
 * (son nom importe peu dans un block, juste son id)
 */
struct declaration {
	int vId;
	char * vName;
	declaration * next;
};

struct int_list {
	int val;
	int_list * next;
};

struct block_list {
	block * val;
	block_list * next;
};

struct block {
	int label;
	blockType bType;
	type sType; // Type de statement: pour l'affichage (if, while...)
	int assignedVar; // variable assignée
	int_list * variables; // liste des variables à droite ou dans la booléenne
	char * str;
};

struct flow_list {
	flow * val;
	flow_list * next;
};

// (1, 2)
struct flow {
	int start;
	int end;
};

/* Methods */

/**
 * Cherche si les types définis de l'arbre sont autorisés pour l'analyse statique.
 */
int checkTreeBeforeAnalysis(node * root); // WK

/* Constructeurs */

declaration * mk_declaration(int id, char * name, declaration * list);

block * mk_block_assign(int label, int assignedVar, int_list * vars, char * str);
block * mk_block_bool_exp(int label, type sType, int_list * vars, char * str);
block * mk_block_skip(int label);
block_list * mk_block_list(block * b, block_list * list);

flow * mk_flow(int start, int end);// JB
flow_list * mk_flow_list(flow * f, flow_list * list);// JB
flow_list * rm_flow_list(flow * f, flow_list * list);// JB

int_list * mk_int_list(int val, int_list * list);
int_list * rm_int_list(int_list * list);

/* Getters */

char * getDeclarationNameWithId(declaration * list, int id);
int getDeclarationIdWithName(declaration * list, char * name);

block * getBlockWithLabel(block_list * list, int label);

int isEmpty_flow_list(flow_list * list);
flow * head_flow_list(flow_list * list);
// Supprime et retourne le premier élément
flow * pop_flow_list(flow_list ** list);


/* Opérations ensemblistes */

int contains(flow * f, flow_list * list);
int_list * union_int_list(int_list * l1, int_list * l2);


/* Analysis */

/**
 * Scanne l'arbre et crée les blocks et le flow.
 * Permet de labeller en direct.
 */
void initScan(node * root);// Wk
declaration * getVariablesDeclarations();
block_list * getBlocks();
flow_list * getFlow();
flow_list * getFlowR(flow_list * list);
int getInit();
int_list * getFinal();

// [x := a]1 ; if [x > 0]2...
void print_blocks(int initial, flow_list * flows, block_list * blocks);//JB
// Return 1 if success
int start_static_analysis(declaration * vars, int initial, int_list * finals, flow_list * flows, flow_list * flowsR, block_list * blocks);//TRT

#endif

