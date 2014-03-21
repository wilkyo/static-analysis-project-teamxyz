#ifndef ANALYSIS_BLOCK__H
#define ANALYSIS_BLOCK__H

#include "static_analysis.h"

typedef struct analysis_block analysis_block;
typedef struct analysis_list analysis_list;

struct analysis_block{
	int_list *list;
	int label;
};

struct  analysis_list{
	analysis_block *block;
	analysis_list* next;
};

//struct management
void mk_analysis_list(analysis_list ** list);
void add_analysis_list(analysis_list ** list, analysis_block *block);
void rm_analysis_list(analysis_list ** list, analysis_block *block);

void mk_analysis_block(analysis_block ** block, int label);
void rm_analysis_block(analysis_block ** block);
//operateur ensembliste
// union();
// minus();
#endif
