#ifndef _MFP__H
#define _MFP__H

#include "static_analysis.h"
#include "analysis_block.h"
#include <stdio.h>

analysis_list* MFP();

//fonction "parametrable"

int MFP_include(analysis_block *ab1,analysis_block *ab2);

analysis_block* fonction_l(analysis_block* ablock); 
int_list * kill(analysis_block *block);
int_list * gen(analysis_block *block);

void init_w(flow_list **w, flow_list *_flows);

int_list* init(int label);
int_list* bottom(int label);
int_list* free_variables(block *block);
int is_in(int label, int_list * liste_label);
#endif
