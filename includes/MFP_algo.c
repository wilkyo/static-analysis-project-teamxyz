#include "MFP_algo.h"

analysis_list* MFP()
{
	/*---------------initialisation--------------*/
	flow_list *W = (flow_list *)NULL; //init liste_label
	//init_w
	init_w(&W,getFlow());//copie de flows

	//liste Analysis initialisation
	analysis_list *analysis_l;
	mk_analysis_list(&analysis_l);
	init_analysis_list(&analysis_l, getFlow());
	//initialisation analysis_list

	flow_list *flcour = W;
	while(flcour != NULL)
	{
		analysis_block *analysis = get_analysis_block(analysis_l, flcour->val->start);
		if(is_in(flcour->val->start, getInit()))
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

	//------------------------iteration-------------------

	flow *cour=NULL;
	while(!isEmpty_flow_list(W))
	{
		cour = pop_flow_list(&W);

		analysis_block *b1 = get_analysis_block(analysis_l,cour->start);
		analysis_block *b2 = get_analysis_block(analysis_l,cour->end);

		if(! MFP_include(b1,b2))
		{
			analysis_block *ablock_res = union_analysis_list(b1,b2);
			//TODO => verifier ce qui se passe en memoire
			//changement d'affectation de 
			//affect_analysis(analysis_union(b1,b2));
		}
		//ajout des chemins depuis "end"
		flcour = getFlow();
		while(flcour != NULL)
		{
			if(flcour->val->start == cour->end)
			{
				//ajout de flow fcour
				if(! contains(cour, flcour))
				{
					W=add_flow_list(flcour->val,W);
				}
			}
			flcour = flcour->next;
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
	ablock->list = union_int_list(minus_int_list(ablock, kill(ablock)),gen(ablock));
	return ablock;
}

/**
renvoie l'union de 2 analysis block
block1 => block a gauche de l'union 
block2 => block a droite de l'union 

concrétement la fonction retourne le block1 auquel on a ajoute 
les informations du block2 (non presente dans le block1)
*/
analysis_block *union_analysis_list(analysis_block *block1, analysis_block * block2)
{
	//TODO
	return NULL;
}


/**
renvoie l'union de 2 analysis block
block_left => block a gauche de la soustraction 
block_right => block a droite de la soustraction

concrétement la fonction retourne le block_left auquel on a retire
les informations du block_right (non presente dans le block1)
*/
analysis_block* minus(analysis_block* block_left,analysis_block *block_right)
{
	//TODO
	return NULL;
}

/*
//pour living variable : 
analysis(l) : liste de variable 
=> list_int ou alors char **

block => 
	-> label 
	-> type
	//if type == B_ASSIGN
	-> left_expression => block.assignedVar
	-> right_expression => block.variables// also in BOOL_EXP
	//


gen(block){
	switch(block.bType)
	{ 
	case B_SKIP:
		return "empty"; break;
	case BOOL_EXP:
		return free_varaibles(block.right_expression);
	case B_ASSIGN:
		return free_variables(block.right_expression);
		break;
	}
}

kill(block)
{
	switch(block.bType)
	{ 
		case B_SKIP:
		case BOOL_EXP:
			return "empty"; 
			break;
		case B_ASSIGN:
			return block.left_expression;
			break;
	}	
}

MFP_List* fonction_l(analysis_block* block)
{
	// (LVexit(l) - kill(get_block(l)) union gen(get_block(l)))
	union(minus(block.prec, kill(block.value)),gen(block.value));
}

*/
