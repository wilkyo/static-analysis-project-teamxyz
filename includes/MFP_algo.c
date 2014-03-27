#include "MFP_algo.h"

analysis_list* MFP(flow_list *_flows, int_list *E)
{
	/*---------------initialisation--------------*/
	flow_list *W = (flow_list *)NULL; //init liste_label
	//init_w
	init_w(&W,_flows);//copie de flows

	//liste Analysis initialisation
	analysis_list *analysis_l = NULL;
	mk_analysis_list(&analysis_l);
	//init_analysis_list(&analysis_l, getFlow());
	//initialisation analysis_list

	//TODO trouver un cas ou cela ne marche pas 
	flow_list *flcour = W;
	while(flcour != NULL)
	{
		analysis_block *analysis = get_analysis_block(analysis_l, flcour->val->start);
		if(analysis ==NULL)
		{
			mk_analysis_block(&analysis, flcour->val->start);
			add_analysis_list(&analysis_l,analysis);
		}

		if(is_in(flcour->val->start,E))
		{
			//valeur d'initialisation => fonction a définir
			//peut être a partir du block
			analysis->list = init(flcour->val->start);
		}
		else
		{
			//valeur par defaut => fonction aussi a definir
			analysis->list = bottom(flcour->val->start);
		}
		flcour = flcour->next;
	}

	/*------------------------iteration-------------------*/

	flow *cour=NULL;
	while(!isEmpty_flow_list(W))
	{
		cour = pop_flow_list(&W);

		analysis_block *b1 = get_analysis_block(analysis_l,cour->start);
		analysis_block *b2 = get_analysis_block(analysis_l,cour->end);


		//DEBUG
		if(b1 == NULL) 
		{
			analysis_block *temp=NULL;
			mk_analysis_block(&temp, cour->start);
			add_analysis_list(&analysis_l,temp);
			b1 = get_analysis_block(analysis_l,cour->start);
		}
		if(b2 == NULL) 
		{
			analysis_block *temp=NULL;
			mk_analysis_block(&temp, cour->end);
			add_analysis_list(&analysis_l,temp);
			b2 = get_analysis_block(analysis_l,cour->end);
		}

		if(! MFP_include(fonction_l(b1),b2))
		{
			//analysis_block *ablock_res = union_analysis_list(b1,b2);
			b2->list = union_int_list(b1->list,b2->list);
			//TODO => verifier ce qui se passe en memoire
			//changement d'affectation de 
			//affect_analysis(analysis_union(b1,b2));
		
			//ajout des chemins depuis "end"
			flcour = _flows;
			while(flcour != NULL)
			{
				if(flcour->val->start == cour->end)
				{
					//ajout de flow fcour
					if(! contains(cour, flcour))
					{
						W=mk_flow_list(flcour->val,W);
					}
				}
				flcour = flcour->next;
			}
		}
	}

	/*------------------- resultat ------------------------------*/

	//parcours des labels
	flow_list *fcour = getFlow();

	analysis_list *alcour = analysis_l;
	while(fcour != NULL)
	{
		//a modifier ceci doit être des structures !!!
		analysis_block *block = get_analysis_block(analysis_l,fcour->val->end);

		alcour->block = fonction_l(block);
	}

	return analysis_l;
}

int_list *free_variables(block *block)
{
	if(block != NULL)
		return block->variables;
	return NULL;
}

int_list *gen(analysis_block *ablock){
	if(ablock != NULL)
	{
		block* b = getBlockWithLabel(getBlocks(),ablock->label);
		if(b != NULL)
		{
			switch(b->bType)
			{ 
			case B_SKIP:
				return NULL; break;
			case B_BOOL_EXP:
			case B_ASSIGN:
				return free_variables(b);
				break;
			}
		}
	}
	return NULL;
}

int_list *kill(analysis_block *ablock)
{
	if(ablock != NULL)
	{
		block* b = getBlockWithLabel(getBlocks(),ablock->label);
		if(b != NULL)
		{
			switch(b->bType)
			{ 
				case B_SKIP:
				case B_BOOL_EXP:
					return NULL; 
					break;
				case B_ASSIGN:
					return mk_int_list(b->assignedVar,NULL);
					break;
			}
		}
	}
	return NULL;	
}

analysis_block* fonction_l(analysis_block *ablock)
{
	//LVentry(l) = (LVexit(l-1) - kill(l)) union gen(l)
	//<=> LVentry(l) = (LVentry(l) - kill(l)) union gen(l)
	int_list *l1= minus_int_list(ablock->list, kill(ablock));
	ablock->list = union_int_list(l1,gen(ablock));
	return ablock;
}


void init_w(flow_list **w, flow_list* _flows)
{
	//WARNING : attention a ne jamais free les flows 
	flow_list *fcour = _flows;
	while(fcour != NULL)
	{
		*w = mk_flow_list(fcour->val,*w);
		fcour = fcour -> next;
	}
}

int_list* init(int label)
{
	//TODO
	return NULL;
}

int_list* bottom(int label)
{
	//TODO
	return NULL;	
}

int MFP_include(analysis_block *b1,analysis_block *b2)
{
	int_list *b1_cour = b2->list;
	int_list *b2_cour = b1->list;
	int foreign_element = 0;
	int res = 0;//false

	//parcours liste b2
	while(b2_cour != NULL && !foreign_element)
	{
		b1_cour = b2->list;
		int current = b2_cour->val;
		//pour chaque element on parcours la lise b1 
		//et on regarde  si il est dans b1
		int find = 0;
		while( (b1_cour != NULL) && (!find) )
		{
			if(b1_cour->val == current) 
				find = 1;
			b1_cour = b1_cour->next;
		}
		if(find == 0) foreign_element = 1;
		b2_cour=b2_cour->next;
	}
	return !foreign_element;
}

int is_in(int label, int_list * liste_label)
{
	int_list * cour = liste_label;
	int trouver = 0;
	while(cour !=NULL && !trouver)
	{
		if(cour->val == label) trouver=1;
		cour = cour->next;
	}
	return trouver;
}