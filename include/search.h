#ifndef __SEARCH_SOLUTION___
#define __SEARCH_SOLUTION___
#include "cubestate.h"
typedef struct{
	int corners;
	int edges1;
	int edges2;
}phase2_pru_t;
typedef struct{
	int co;
	int eo;
	int middle_edges_combination;
}phase1_pru_t;
typedef std::vector<int> steps_t; 
//typedef struct{
//	int steps[32];
//} steps_t;
struct search_info_t
{
	cube_t initstate;
	int  face, total_depth,current_depth;
	steps_t* steps;
	int co_index;
	int eo_index;
	int me_combine_index;
	int ud_edges_perm_index;
	int middle_edges_perm_index;
	int cornors_perm_index;
};
enum phase_type{
	eo_index = 0,
	co_index,
	me_combine_index,
	ud_edges_perm_index,
	middle_edges_perm_index,
	cornors_perm_index
};

#endif