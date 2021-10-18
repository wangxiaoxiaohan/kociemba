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

struct search_t
{
	cube_t initstate;
	int  face, total_depth,current_depth;
	steps_t* steps;
	int phase1_co_index;
	int phase1_eo_index;
	int phase1_edges_index;
	int phase2_edges1_index;
	int phase2_edges2_index;
	int phase2_corners_index;
};
enum pahse_type{
	phase1_eo = 0,
	phase1_co,
	phase1_edges,
	phase2_edges1,
	phase2_edges2,
	phase2_corners,
	phase2_edges
};

#endif