#ifndef ANALYSIS_BLOCK__H
#define ANALYSIS_BLOCK__H

#include "static_analysis.h"

typedef struct 
{
	list_int *list;
}analysis_block;

typedef struct 
{
	analysis_block *block;
	analysis_list* next;
}analysis_list;


//struct management
void mk_analysis_list(analysis_list ** list);
void add_analysis_list(analysis_list ** list, analysis_block *block);
void rm_analysis_list(analysis_list ** list, analysis_block *block);

void mk_analysis_block(analysis_block ** block);
void rm_analysis_block(analysis_block ** block);
//operateur ensembliste
// union();
// minus();
#endif
