#include "analysis_block.h"
#include <stdlib.h>

//struct management
void mk_analysis_list(analysis_list ** list)
{
	if((*list) == NULL)
	{
		(*list) = malloc(sizeof(analysis_list));
		(*list)->next = NULL;
		(*list)->block = NULL;
	}
	else
	{
		analysis_list * aux	= (*list);
		while(aux->next != NULL) aux = aux->next;
		aux->next = malloc(sizeof(analysis_list)); 
	}
}

//trt : ajout en fin de liste du block block a la liste *list
void add_analysis_list(analysis_list ** list, analysis_block *block)
{
	//TODO ajouter des protection en cas d'envoie de NULL 
	analysis_list *aux = *list;
	analysis_list *prec = *list;
	//parcours
	if(aux != NULL) aux = aux->next;
	while(aux != NULL)
	{
		prec = aux;
		aux= aux->next;
	}
	//creation du block_list, prec dernier bloc de la liste
	mk_analysis_list(&(prec->next));
	//allocation du bloc
	prec->block = block;

	/*
	mk_analysis_list(list);
	analysis_list * aux = (*list);
	while(aux->next != NULL) aux = aux->next;
	aux->next->block = block;
	*/
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
			return lcour->block;
		}
		lcour= lcour->next;
	}
	return NULL;
}

void print_analysis_block(analysis_block *b)
{
	if(b != NULL)
	{
		if( b->label != 0)
		{
			printf("(%d,",b->label);
		}
		if(b->list != NULL)
		{
			int_list *cour = b->list;
			while(cour != NULL)
			{
				printf("%d ", cour->val);
				cour = cour->next;
			}
					
			
		}
		else
		{
			printf("empty ");
		}
		printf(")\n");
	}
}

void print_analysis_list(analysis_list *l)
{
	analysis_list *cour = l;
	while(cour != NULL)
	{
		print_analysis_block(cour->block);
		cour = cour->next;
	}
}
