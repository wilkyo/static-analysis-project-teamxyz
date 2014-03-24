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
	mk_analysis_list(list);
	analysis_list * aux = (*list);
	while(aux->next != NULL) aux = aux->next;
	aux->next->block = block;
}

void rm_analysis_list(analysis_list ** list, analysis_block *block)
{
	if((*list) != NULL)
	{
		analysis_list * aux = (*list);
		if(aux->block == block)
		{
			analysis_list * aux2 = aux;
			aux = aux->next;
			rm_analysis_block(&(aux2->block));
			free(aux2);
		}
		else
		{
			while(aux->next != NULL)
			{
				if(aux->next->block == block)
				{
					analysis_list * aux2 = aux->next;
					aux->next = aux2->next;
					rm_analysis_block(&(aux2->block));
					free(aux2);
					break;
				}
			}
		}
	}
}

void mk_analysis_block(analysis_block ** block, int label)
{
	(*block) = malloc(sizeof(analysis_block));
	(*block)->label = label;
	(*block)->list = NULL;
}

void rm_analysis_block(analysis_block ** block)
{
	free_int_list((*block)->list);
	free((*block));
	(*block)=NULL;
}

analysis_block* get_analysis_block(analysis_list *_list, int _label)
{
	analysis_list *lcour = _list;
	while(lcour != NULL)
	{
		if(lcour->block != NULL && lcour->block->label == _label)
		{
			return lcour;
		}
		lcour= lcour->next;
	}
	return NULL;
}