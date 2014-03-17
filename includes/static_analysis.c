#include "static_analysis.h"

#include <stdlib.h>

/* Inits */

declaration * getVariablesDeclarations(node * root) {
	return NULL;
}
block_list * getBlockList(node * root) {
	return NULL;
}
flow_list * getFlow(node * root) {
	return NULL;
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
	return 0;
}

