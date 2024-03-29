#include "MFP_algo.h"

analysis_list* MFP(flow_list *_flows, int_list *E)
{
	printf("BEGIN MFP\n");
	/*---------------initialisation--------------*/
	flow_list *W = (flow_list *)NULL; //init liste_label
	//init_w
	init_w(&W,_flows);//copie de flows

	//liste Analysis initialisation
	analysis_list *analysis_l = NULL;
	mk_analysis_list(&analysis_l);
	//init_analysis_list(&analysis_l, getFlow());
	//initialisation analysis_list

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
		
		//printf("cour: (%d, %d) \n", cour->start, cour->end);

		analysis_block *b1 = get_analysis_block(analysis_l,cour->start);
		analysis_block *b2 = get_analysis_block(analysis_l,cour->end);


		//DEBUG
		if(b1 == NULL) 
		{
			//printf("b1 NULL %d\n", cour->start);
			analysis_block *temp=NULL;
			mk_analysis_block(&temp, cour->start);
			add_analysis_list(&analysis_l,temp);
			b1 = get_analysis_block(analysis_l,cour->start);
		}
		if(b2 == NULL) 
		{
			//printf("b2 NULL %d\n", cour->end);
			analysis_block *temp=NULL;
			mk_analysis_block(&temp, cour->end);
			add_analysis_list(&analysis_l,temp);
			b2 = get_analysis_block(analysis_l,cour->end);
		}

		analysis_block * fb1 = fonction_l(b1);
		if(! MFP_include(fb1, b2))
		{
			//analysis_block *ablock_res = union_analysis_list(b1,b2);
			b2->list = union_int_list(fb1->list, b2->list);
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
					if(! contains(flcour->val, W))
					{
						W=mk_flow_list(flcour->val,W);
					}
				}
				flcour = flcour->next;
			}
		}
		if (fb1 !=NULL) free(fb1);
	}

	/*------------------- resultat ------------------------------*/

	//printf("Fin itérations\n");
	//inutile ici, vu que l'on travaille avec une seule liste
	//printf("END MFP\n");
	return analysis_l;
}

int_list *free_variables(block *nblock)
{
	if(nblock != NULL)
	{
		int_list * test = mk_int_list(nblock->assignedVar,NULL);
		int_list *res = minus_int_list(nblock->variables,test);
		free(test);
		return res;
	}
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
	analysis_block *res = NULL;
	if(ablock != NULL)
	{
		//LVentry(l) = (LVexit(l-1) - kill(l)) union gen(l)
		//<=> LVentry(l) = (LVentry(l) - kill(l)) union gen(l)
		mk_analysis_block(&(res),ablock->label);
		int_list *l_kill = kill(ablock);
		int_list *l1= minus_int_list(ablock->list, l_kill);
		int_list *l_gen = gen(ablock);
		res->list = union_int_list(l1,gen(ablock));
	}
	return res;
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
