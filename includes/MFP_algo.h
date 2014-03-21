#ifndef _MFP__H
#define _MFP__H

#include "static_analysis.h"
#include "analysis_block.h"
#include <stdio.h>

//fonction "parametrable"

int MFP_include(analysis_block *ab1,analysis_block *ab2);

analysis_block* fonction_l(analysis_block* ablock); 
int_list * kill(analysis_block *block);
int_list * gen(analysis_block *block);
//init();
//bottom();
int_list *free_variables(block *block );
#endif
