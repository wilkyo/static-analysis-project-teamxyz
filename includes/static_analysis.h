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
	B_ASSIGN, B_BOOLEAN, B_SKIP
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

declaration * getVariablesDeclarations(node * root);
block_list * getBlockList(node * root);
flow_list * getFlow(node * root);
flow_list * getFlowR(flow_list * list);
int getInit(flow_list * list);
int_list * getFinal(flow_list * list);

// [x := a]1 ; if [x > 0]2...
void print_blocks(int initial, flow_list * flows, block_list * blocks);
// Return 1 if success
int start_static_analysis(declaration * vars, int initial, int_list * finals, flow_list * flows, flow_list * flowsR, block_list * blocks);

#endif

