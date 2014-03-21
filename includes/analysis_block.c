#include "analysis_block.h"
#include <stdlib.h>

//struct management
void mk_analysis_list(analysis_list ** list)
{
	if((*list) == NULL)
	{
		(*list) = malloc(sizeof(analysis_list));
	}
	else
	{
		analysis_list * aux	= (*list);
		while(aux->next != NULL) aux = aux->next;
		aux->next = malloc(sizeof(analysis_list)); 
	}
}

void add_analysis_list(analysis_list ** list, analysis_block *block)
{
}

void rm_analysis_list(analysis_list ** list, analysis_block *block)
{
}

void mk_analysis_block(analysis_block ** block, int label)
{
	(*block) = malloc(sizeof(analysis_block));
	(*block)->label = label;
	(*block)->list = NULL;
}

void rm_analysis_block(analysis_block ** block)
{
	(*block)->list = rm_int_list((*block)->list);
	free((*block));
	(*block)=NULL;
}

//operateur ensembliste
// union();
// minus();
