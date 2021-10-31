#ifndef _PREPARTION_
#define _PREPARTION_

#include "search.h"

#define MOVE_COUNT 18
#define MOVELIMIT  74943//010 010 010 010 111 111

//prase1
#define CORNORS_PERM_SIZE  40320 // 8! CORNORS_PERM_SIZE
#define UD_EDGES_PERM_SIZE  40320  // 8!
#define MIDDLE_EDGES_PERM_SIZE  24	   // 4!

//phrase2
#define MIDDLE_EDGES_PERM_ORIENTATION_SIZE 11880  // A(4,12); 
#define CORNORS_ORIENTATION_SIZE  2187 //3^7
#define EDGES_ORIENTATION_SIZE  2048 //2^11

#define MIDDLE_EDGES_COMBINATION 495//C(4,12)



class prepareSearch{
public:
	prepareSearch();
	~prepareSearch();
	void solve(cube_t cube);

	
private:
	static int calculateIndex(cube_t &cube,int type);
	void init();
	void phase2_fill_buffer(cube_t cube,int8_t *dest,int destSize,enum phase_type type);
	void phase2_fill_pre();
	void phase1_fill_buffer(cube_t goalstate,int8_t *dest,int destSize,enum phase_type type);
	void phase1_fill_pre();
	bool DFSphase2(search_info_t& se_t);
	bool phase2(cube_t    cube,moves_t steps);
	bool DFSphase1(search_info_t& se_t);
	
	void printSolution(moves_t steps);	
	void writeCache(const char *cachePath);
	void readCache(const char *cachePath);


	//phase1 完成的状态(1.棱块方向全部归位 2。角块方向全部归位 3 中层四个棱块都在中层，以下三个数组分别对应)
	int8_t cornors_orientation[CORNORS_ORIENTATION_SIZE];//角块方向 只考虑7个即可，若7个方向已经定下来 剩下一个也就是已知的
	int8_t edges_orientation[EDGES_ORIENTATION_SIZE];//棱块方向同理 只考虑11个即可
	int8_t middle_edges_perm_orientation[MIDDLE_EDGES_PERM_ORIENTATION_SIZE];//考虑中层四个棱块位置上的棱块编号

	//每种case 经过某种移动后的case 将其记录下来 避免在搜索过程中每次都去进行计算，能够极大提升计算效率
	int cornors_orientation_move[CORNORS_ORIENTATION_SIZE][MOVE_COUNT];
	int edges_orientation_move[EDGES_ORIENTATION_SIZE][MOVE_COUNT];
	int middle_edges_perm_orientation_move[MIDDLE_EDGES_PERM_ORIENTATION_SIZE][MOVE_COUNT];
	
	
	//为何是 CORNORS_ORIENTATION_SIZE * phase1_middle_edges_combation（495）/C(4,12)而不是
	//		 CORNORS_ORIENTATION_SIZE * MIDDLE_EDGES_PERM_ORIENTATION_SIZE(12* 11 * 10 * 9)/A(4,12)	?
	//中层四棱的位置 不必考虑其内部分布（也就是将四个块视作相同的块） 只要他们四个都到了中层 
	//就达到了 phase1 的要求 因此，四个块若占据固定的四个位置，其内部分布不必考虑 
	// 在阶段一的搜索中 考虑这一点 能将搜索次数减少 2-3个数量级
	int8_t co_mec[CORNORS_ORIENTATION_SIZE * MIDDLE_EDGES_COMBINATION];
	int8_t eo_mec[EDGES_ORIENTATION_SIZE * MIDDLE_EDGES_COMBINATION];
	


	
	//phase2 就是考虑块的分布 
	int8_t corners_perm[CORNORS_PERM_SIZE];
	int8_t ud_edges_perm[UD_EDGES_PERM_SIZE];
	int8_t middle_edges_perm[MIDDLE_EDGES_PERM_SIZE];
	
	int corners_perm_move[CORNORS_PERM_SIZE ][MOVE_COUNT];
	int ud_edges_perm_move[UD_EDGES_PERM_SIZE ][MOVE_COUNT];
	int middle_edges_perm_move[MIDDLE_EDGES_PERM_SIZE][MOVE_COUNT];
	
	int8_t cp_mep[MIDDLE_EDGES_PERM_SIZE *  CORNORS_PERM_SIZE];
	int8_t ep_mep[MIDDLE_EDGES_PERM_SIZE *  UD_EDGES_PERM_SIZE];

	
	std::vector<cube_t> middle_edges_perms;

};


#endif
