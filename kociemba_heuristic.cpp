  #include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <stdlib.h>
#include <cstring>
#include <utility> 
#include <sys/time.h>    
#include <unistd.h>   
#include <stdio.h>

using namespace std;

typedef vector<int> vi;
typedef vi steps_t; 
vector<vi> phase2_edges2_all;


#define MOVE_COUNT 18

static const int phrase2_corners_size = 40320; // 8!
static const int phrase2_edges1_size = 40320;  // 8!
static const int phrase2_edges2_size = 24;	   // 4!

static const int phrase1_edges_size = 12 * 11 * 10 * 9;
static const int phrase1_co_size = 2187;
static const int phrase1_eo_size = 2048;

static const int phase1_middle_edges_combation = 495;//C(12,4)

int8_t phrase1_edges[phrase1_edges_size];
int8_t phrase1_co[phrase1_co_size];//角块方向 这里定义了 3^8种 事实上其中只有3^7的合法的,旋转过程中不会出现，为了方便索引才这样使用
int8_t phrase1_eo[phrase1_eo_size];

int phase1_co_move[phrase1_co_size][MOVE_COUNT];
int phase1_eo_move[phrase1_eo_size][MOVE_COUNT];
int phase1_edges_move[phrase1_edges_size][MOVE_COUNT];

int8_t phase1_pre_cornors[phrase1_co_size * phase1_middle_edges_combation];
int8_t phase1_pre_edges[phrase1_eo_size * phase1_middle_edges_combation];




int8_t phrase2_corners[phrase2_corners_size];
int8_t phrase2_edges1[phrase2_edges1_size];
int8_t phrase2_edges2[phrase2_edges2_size];

int phrase2_corners_move[phrase2_corners_size ][MOVE_COUNT];
int phrase2_edges1_move[phrase2_edges1_size ][MOVE_COUNT];
int phrase2_edges2_move[phrase2_edges2_size][MOVE_COUNT];

int8_t phase2_pre_corners[phrase2_edges2_size *  phrase2_corners_size];
int8_t phase2_pre_edges[phrase2_edges2_size *  phrase2_edges1_size];


int moveLimit = 74943;
int affectedCubies[][8] = {
	{  0,  1,  2,  3,  0,  1,  2,  3 },   // U   
	{  4,  7,  6,  5,  4,  5,  6,  7 },   // D
	{  0,  9,  4,  8,  0,  3,  5,  4 },   // F
	{  2, 10,  6, 11,  2,  1,  7,  6 },   // B
	{  3, 11,  7,  9,  3,  2,  6,  5 },   // L
	{  1,  8,  5, 10,  1,  0,  4,  7 },   // R
};



static vi factorial_8 ={1,2,6,24,120,720,5040,40320};
static vi factorial_4 ={1,2,6,24};
static vi factorial_12_4 = {1,9,90,990};

int phase1_done = 0;
long long total_timePhase2;


enum pahse_type{
	phase1_eo = 0,
	phase1_co,
	phase1_edges,
	phase2_edges1,
	phase2_edges2,
	phase2_corners,
	phase2_edges
};
struct search_t
{
	vi initstate;
	int  face, total_depth,current_depth;
	steps_t* steps;
	int phase1_co_index;
	int phase1_eo_index;
	int phase1_edges_index;
	int phase2_edges1_index;
	int phase2_edges2_index;
	int phase2_corners_index;
};
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


vi applyMove ( int move, vi state ) {
	int turns = move % 3 + 1;
	int face = move / 3;
	while( turns-- ){
		vi oldState = state;
		for( int i=0; i<8; i++ ){
			int isCorner = i > 3;
			int target = affectedCubies[face][i] + isCorner*12;
			int killer = affectedCubies[face][(i&3)==3 ? i-3 : i+1] + isCorner*12;;
			int orientationDelta = (i<4) ? (face>1 && face<4) : (face<2) ? 0 : 2 - (i&1);
			state[target] = oldState[killer];
			//state[target+20] = (oldState[killer+20] + orientationDelta) % (2 + isCorner);
			state[target+20] = oldState[killer+20] + orientationDelta;
			if( !turns )
				state[target+20] %= 2 + isCorner;
		}
	}
	return state;
}
int cantor(vi nums,vi factorial_n){
    int ans=0,sum=0;
    for(int i=0;i<nums.size();i++){
		for(int j=i+1;j<nums.size();j++)
			if(nums[j]<nums[i])
				sum++;
		
        ans+=sum*factorial_n[nums.size()-i - 2];//累积
       // printf("sum %d ans %d\n",sum,ans);
        sum=0;//计数器归零
        
    }
    return ans;
}
int perm(vi nums,vi factorial_n){
	int ans=0,sum=0;
	int totalSize = nums.size();
	int vaildSize =	factorial_n.size();
	for(int i = 0 ; i < nums.size() ;i ++){
		for(int j=i+1;j<nums.size();j++)
            if(nums[j]<nums[i])
                sum++;
        ans+=(nums[i] - sum)*factorial_n[nums[i]-(totalSize - vaildSize)];//
       // printf("sum %d ans %d\n",sum,ans);
        sum=0;//计数器归零
	}
	return ans;

}
int Cnk(int n, int k) {
    int i, j, s;
    if (n < k)
        return 0;
    if (k > n / 2)
        k = n - k;
    for (s = 1, i = n, j = 1; i != n - k; i--, j++) {
        s *= i;
        s /= j;
    }
    return s;
}
void printSolution(steps_t steps){

	for( int i=0; i<steps.size(); i++ ){
		printf("  ");
		int movesteps = (steps[i]%3+1);
		if(movesteps ==3){
			cout << "UDFBLR"[steps[i]/3] << "'";
		}else if(movesteps == 1){
			cout << "UDFBLR"[steps[i]/3];
		}else {
			cout << "UDFBLR"[steps[i]/3] << steps[i]%3+1;
		}
	}

}
int calculateIndex(vi state,int type){
	int ret = 0;
	switch(type){
		//根据魔方的性质 一个合法的魔方状态 块的方向并不能随意组合 当前除了某一个块的其他块方向已经定下来的时候
		//个块方向就是确定的,因此不要需要考虑全部块的方向 角块考虑 7 个 棱块考虑 11个 你可以随意选一个作为不考虑对象
		//这里我将编号的最后一个块作为不考虑的对象
		case phase1_co :
		{
			vi co(state.begin() + 32,state.begin() + 39);
			for(int i = 0 ; i < co.size();i++){
				ret = ret * 3 + co[i];
			}
		}
		break;

		case phase1_eo:
		{
			vi eo(state.begin() + 20,state.begin() + 31);
			for(int i = 0 ; i < eo.size();i++){
				ret = ret * 2 + eo[i];
			}
		}
		break;
		case phase1_edges:
		{
			vi edges_blockPosition(state.begin(),state.begin() + 12);
			
			int permu_Index = 0;
			vi middle_edges;
			int combina_index =  0;
			int x = 0;
			for(int i = 0; i < 12;i++){
				if(edges_blockPosition[i] >= 8){
					//edges_blockPosition[i] = 0;
					middle_edges.push_back(edges_blockPosition[i] - 8);
				}		
			}  
			for(int j = 11 ;j >=0 ;j-- ){
				if(edges_blockPosition[j] >= 8){
					combina_index += Cnk(11 - j, x + 1);
					x++;
				}
			}
			permu_Index = cantor(middle_edges,factorial_4);
			ret = combina_index * 24 + permu_Index;
			//printf("permu_Index %d combina_index %d ret %d\n",permu_Index,combina_index,ret);
			/*
			for(int i = 0; i < 12;i++){
				if(edges_blockPosition[i] < 8){
					edges_blockPosition[i] = 0;
				}		
			} 
			ret = perm(edges_blockPosition,factorial_12_4);
			*/
		}
		break;
		case phase2_edges1:
		{
			vi edgesBlocks(state.begin() ,state.begin() + 8);
			ret = cantor(edgesBlocks,factorial_8);
		}
		break;
		case phase2_edges2:
		{
			vi middleEdgeBlocks(state.begin() +  8 ,state.begin() + 12);
			for(int i = 0 ;i < middleEdgeBlocks.size(); i++){
				//printf("mediaEdgeBlocksIndex[i] %d\n",mediaEdgeBlocks[i]);
				middleEdgeBlocks[i] = middleEdgeBlocks[i] - 8;
			}
			ret = cantor(middleEdgeBlocks,factorial_4);
		}
		break;
		case phase2_corners:
		{
			vi cornorBlocks(state.begin() +  12 ,state.begin() + 20);
			for(int i = 0 ;i < cornorBlocks.size(); i++){
				cornorBlocks[i] = cornorBlocks[i] - 12;
			}
			ret = cantor(cornorBlocks,factorial_8);
		}
		break;

	}
	return ret;

}
void phase2_fill_heuristic(vi state,int8_t *dest,int destSize,enum pahse_type type){
	queue<pair<vi,int>> q;
	memset(dest,0xff,destSize);
	q.push(make_pair(state,0));

	dest[0] = 0;
	if(type == phase2_edges2){
		phase2_edges2_all.push_back(state);
	}
	int count = 0;
	while(!q.empty()){ 
		pair<vi ,int> front = q.front();
		int fatherIndex =calculateIndex(front.first,type);
		for( int move=0; move<18; move++ ){
			if( moveLimit & (1 << move) ){		

				vi currstate = applyMove(move, front.first);			
				int Index = calculateIndex(currstate,type);

				
				if(type == phase2_corners){
					phrase2_corners_move[fatherIndex][move] = Index;
				}else if(type == phase2_edges1){
					phrase2_edges1_move[fatherIndex][move] = Index;
				}else if(type == phase2_edges2){
					phrase2_edges2_move[fatherIndex][move] = Index;
				}
				if(dest[Index] == -1){
					int step = front.second;	
					dest[Index] = step + 1;
					q.push(make_pair(currstate,step+1));
					if(type == phase2_edges2){
						phase2_edges2_all.push_back(currstate);
					}
				}
				count ++;
			}
		}
		q.pop();

	}
	if(type == phase2_edges2){
		//printf("phase2_edges2_all size %d\n",phase2_edges2_all.size());
	}
	printf("phase2 type %d serch count %d \n",type,count);

}

void phase2_fill_pre(){
	int searchcount = 0;


	memset(phase2_pre_corners,0xff,sizeof(int8_t) * phrase2_edges2_size *  phrase2_corners_size);
	queue<pair<phase2_pru_t,int>> q;
	phase2_pru_t first_t;
	first_t.corners = 0;
	first_t.edges2 = 0;
	q.push(make_pair(first_t,0));
	phase2_pre_corners[0] = 0;
	
	while(!q.empty()){ 
		pair<phase2_pru_t,int> front = q.front();
		int depth = front.second;
		searchcount ++;
		for( int move=0; move<18; move++ ){
			if( moveLimit & (1 << move) ){			
				phase2_pru_t new_t;	
				new_t.corners = phrase2_corners_move[front.first.corners][move];
				new_t.edges2 = phrase2_edges2_move[front.first.edges2][move];
				if(phase2_pre_corners[new_t.corners * 24 + new_t.edges2] == -1){
					q.push(make_pair(new_t,depth + 1));
					phase2_pre_corners[new_t.corners * 24 + new_t.edges2] = depth + 1;
				}
			}
		}
		q.pop();
	}

	printf("searchcount %d\n",searchcount);
	memset(phase2_pre_edges,0xff,sizeof(int8_t) * phrase2_edges2_size *  phrase2_edges1_size);
	queue<pair<phase2_pru_t,int>> q2;
	phase2_pru_t first_t2;
	first_t2.edges1 = 0;
	first_t2.edges2 = 0;
	q2.push(make_pair(first_t2,0));
	phase2_pre_edges[0] = 0;
	while(!q2.empty()){ 
		pair<phase2_pru_t,int> front = q2.front();
		int depth = front.second;
		searchcount++;
		for( int move=0; move<18; move++ ){
			if( moveLimit & (1 << move) ){			
				phase2_pru_t new_t;	
				new_t.edges1 = phrase2_edges1_move[front.first.edges1][move];
				new_t.edges2 = phrase2_edges2_move[front.first.edges2][move];
				if(phase2_pre_edges[new_t.edges1 * 24 + new_t.edges2] == -1){
					q2.push(make_pair(new_t,depth + 1));
					phase2_pre_edges[new_t.edges1 * 24 + new_t.edges2] = depth + 1;
				}
			}
		}
		q2.pop();
	}
	printf("searchcount %d\n",searchcount);

}
void phase1_fill_heuristic(vi goalstate,int8_t *dest,int destSize,enum pahse_type type){
	queue<pair<vi,int>> q;
	//to do 初始化queue
	memset(dest,0xff,destSize);
	pair<vi ,int> firstPair(goalstate,0);
	q.push(firstPair);
	dest[0] = 0;
	int count = 0;
	while(!q.empty()){ 
		pair<vi ,int> front = q.front();
		int fatherIndex =calculateIndex(front.first,type);
		for( int move=0; move<18; move++ ){
			int step = front.second;
			vi currstate = applyMove(move, front.first);
			int Index =calculateIndex(currstate,type);
		    if(type == phase1_co){
				phase1_co_move[fatherIndex][move] = Index;
			}else{
				phase1_eo_move[fatherIndex][move] = Index;
			}
			if(dest[Index] == -1){
				dest[Index] = step + 1;
				q.push(make_pair(currstate,step+1));
			}	
			count ++;
		}
		q.pop();

	}
	printf("phase1 type: %d serch count %d \n",type,count);
}
void phase1_edges_fill_heuristic(vi state){
	queue<pair<vi,int>> q;
	memset(phrase1_edges,0xff,phrase1_edges_size);
	pair<vi ,int> firstPair(state,0);
	q.push(firstPair);
	int count = 0 ;
	for(int i = 0 ; i <phase2_edges2_all.size();i++){
		q.push(make_pair(phase2_edges2_all[i],0));
		int Index = calculateIndex(phase2_edges2_all[i], phase1_edges);
		phrase1_edges[Index] = 0;
	}
	while(!q.empty()){ 
		pair<vi ,int> front = q.front();
		int FatherIndex = calculateIndex(front.first, phase1_edges);
		for( int move=0; move<18; move++ ){
			int step = front.second;
			vi currstate = applyMove(move, front.first);
			int Index = calculateIndex(currstate, phase1_edges);
			//printf("phase1_edges Index %d\n",Index);
			phase1_edges_move[FatherIndex][move] = Index;
			 if(phrase1_edges[Index] == -1)
			 {
			    phrase1_edges[Index] = step + 1;
				q.push(make_pair(currstate,step+1));
			 }
			 count++;
		}
		q.pop();
	}
	
	printf("phase1 type: %d serch count %d \n",(int)phase1_edges,count);
}
void phase1_fill_pre(){
	int searchcount = 0;
	memset(phase1_pre_cornors,0xff,sizeof(int8_t) * phrase1_co_size * phase1_middle_edges_combation);
	queue<pair<phase1_pru_t,int>> q;
	phase1_pru_t first_t;
	first_t.co = 0;
	first_t.middle_edges_combination = 0;
	q.push(make_pair(first_t,0));
	phase1_pre_cornors[0] = 0;
	while(!q.empty()){ 
		pair<phase1_pru_t,int> front = q.front();
		int depth = front.second;
		searchcount ++;
		for( int move=0; move<18; move++ ){		
				phase1_pru_t new_t;	
				new_t.co = phase1_co_move[front.first.co][move];
				new_t.middle_edges_combination = phase1_edges_move[front.first.middle_edges_combination][move];
				if(phase1_pre_cornors[new_t.co * phase1_middle_edges_combation + new_t.middle_edges_combination/24] == -1){
					q.push(make_pair(new_t,depth + 1));
					phase1_pre_cornors[new_t.co * phase1_middle_edges_combation + new_t.middle_edges_combination/24] = depth + 1;
				}
		}
		q.pop();
	}
	printf("searchcount %d\n",searchcount);


	searchcount = 0;
	memset(phase1_pre_edges,0xff,sizeof(int8_t) * phrase1_eo_size * phase1_middle_edges_combation);
	queue<pair<phase1_pru_t,int>> q2;
	phase1_pru_t first_t2;
	first_t2.eo = 0;
	first_t2.middle_edges_combination = 0;
	q2.push(make_pair(first_t2,0));
	phase1_pre_edges[0] = 0;
	while(!q2.empty()){ 
		pair<phase1_pru_t,int> front = q2.front();
		int depth = front.second;
		searchcount ++;
		for( int move=0; move<18; move++ ){		
				phase1_pru_t new_t;	
				new_t.eo = phase1_eo_move[front.first.eo][move];
				new_t.middle_edges_combination = phase1_edges_move[front.first.middle_edges_combination][move];
				if(phase1_pre_edges[new_t.eo * phase1_middle_edges_combation + new_t.middle_edges_combination/24] == -1){
					q2.push(make_pair(new_t,depth + 1));
					phase1_pre_edges[new_t.eo * phase1_middle_edges_combation + new_t.middle_edges_combination/24] = depth + 1;
				}
		}
		q2.pop();
	}
	printf("searchcount %d\n",searchcount);


}



//int search_count = 0;
bool DFSphase2(search_t& se_t){
		for( int move=0; move<18; move++ ){
			if( moveLimit & (1 << move) ){
			//	search_count ++;
				if(move / 3 == se_t.face || move / 3  == se_t.face - 1) continue;
				int phase2_edges1_index = phrase2_edges1_move[se_t.phase2_edges1_index][move];
				int phase2_edges2_index = phrase2_edges2_move[se_t.phase2_edges2_index][move];
				int phase2_corners_index = phrase2_corners_move[se_t.phase2_corners_index][move];
				//int estimateval = max(phrase2_edges1[phase2_edges1_index],max(phrase2_edges2[phase2_edges2_index],phrase2_corners[phase2_corners_index]));
				int estimateval =max(phase2_pre_edges[phase2_edges1_index * 24 + phase2_edges2_index],phase2_pre_corners[phase2_corners_index * 24 +phase2_edges2_index] 	);
				if(estimateval + se_t.current_depth + 1 <= se_t.total_depth){
					(*se_t.steps)[se_t.current_depth] = move;
					if(estimateval == 0){
							return true;
					}	
					search_t newSe_t = se_t;
					newSe_t.current_depth += 1;
					newSe_t.phase2_edges1_index = phase2_edges1_index;
					newSe_t.phase2_edges2_index = phase2_edges2_index;
					newSe_t.phase2_corners_index = phase2_corners_index;
					newSe_t.face = move / 3;
					if(DFSphase2(newSe_t))
						return true;			
				} 
			}
		}
			
		return false;
	
}


bool phase2(vi   state,steps_t steps){
	for( int i=0; i<steps.size(); i++ ){
		state = applyMove(steps[i], state);
	}
	int step2_len = (20 - steps.size()) > 10 ? 10 : ( 20 - steps.size());
	for(int depth = 0 ;depth <= step2_len; ++depth){
		steps_t steps2(depth);
		search_t search;
		search.face = 6;
		search.phase2_edges1_index = calculateIndex(state, phase2_edges1);
		search.phase2_edges2_index = calculateIndex(state, phase2_edges2);
		search.phase2_corners_index = calculateIndex(state, phase2_corners);
		search.current_depth = 0;
		search.total_depth = depth;
		search.steps = &steps2;
	
		if(DFSphase2(search)){
			
			printSolution(steps);
			printf("      ");
			printSolution(steps2);	
			printf("\n");
			printf("phase1_done count %d\n",phase1_done);
			return true;
		}
	}
	return false;
}

bool DFSphase1(search_t& se_t){

		for( int move=0; move<18; move++ ){
			if(move / 3 == se_t.face ) continue;

			int phase1_co_index = phase1_co_move[se_t.phase1_co_index][move];
			int phase1_eo_index = phase1_eo_move[se_t.phase1_eo_index][move];
			int phase1_edges_index = phase1_edges_move[se_t.phase1_edges_index][move];
			
			//int estimateval = max(max(phrase1_co[phase1_co_index],phrase1_eo[phase1_eo_index]),phrase1_edges[phase1_edges_index]); 
			int estimateval =max(phase1_pre_cornors[phase1_middle_edges_combation * phase1_co_index + phase1_edges_index/24],
								 phase1_pre_edges[phase1_middle_edges_combation * phase1_eo_index   + phase1_edges_index/24]);
			if(estimateval + se_t.current_depth + 1 <= se_t.total_depth){
				(*se_t.steps)[se_t.current_depth] = move;
				if(estimateval == 0){
					phase1_done ++; 
					if(phase2(se_t.initstate,*se_t.steps))	
						return true;

					
				}
				search_t newSe_t = se_t;
				newSe_t.current_depth += 1;
				newSe_t.face = move / 3;
				newSe_t.phase1_co_index = phase1_co_index;
				newSe_t.phase1_eo_index = phase1_eo_index;
				newSe_t.phase1_edges_index = phase1_edges_index;
				if(DFSphase1(newSe_t)){
						return true;
				}
								
			} 
		}
			
		return false; 
	
}

bool phase(vi state){

	for(int depth = 0 ; ; ++depth){
		printf("!!!!!!!!!!!!!!!!!!!!!depth %d\n",depth);
		steps_t steps(depth);
		search_t search;

		search.face = 6;
		search.initstate =state;
		search.current_depth = 0;
		search.total_depth = depth;
		search.steps = &steps;
		search.phase1_co_index = calculateIndex(state,phase1_co);
		search.phase1_eo_index = calculateIndex(state,phase1_eo);
		search.phase1_edges_index = calculateIndex(state,phase1_edges);

		if(DFSphase1(search))
			break;
	}
}

int main(int argc ,char **argv){

	//--- Define the goal.
	string goal[] = { "UF", "UR", "UB", "UL", "DF", "DR", "DB", "DL", "FR", "FL", "BR", "BL",
		"UFR", "URB", "UBL", "ULF", "DRF", "DFL", "DLB", "DBR" };

	//--- Prepare current (start) and goal state.
	vi currentState( 40 ), goalState( 40 );
	for( int i=0; i<20; i++ ){

		//--- Goal state.
		goalState[i] = i;

		//--- Current (start) state.
		string cubie = argv[i+1];
		while( (currentState[i] = find( goal, goal+20, cubie ) - goal) == 20){
			cubie = cubie.substr( 1 ) + cubie[0];
			currentState[i+20]++;
		}
	}


	memset(phrase2_edges1_move,0xff,sizeof(int) * phrase2_edges1_size * MOVE_COUNT);
	phase2_fill_heuristic(goalState,phrase2_edges1,phrase2_edges1_size,phase2_edges1);

	memset(phrase2_edges2_move,0xff,sizeof(int) * phrase2_edges2_size * MOVE_COUNT);
	phase2_fill_heuristic(goalState,phrase2_edges2,phrase2_edges2_size,phase2_edges2);

	memset(phrase2_corners_move,0xff,sizeof(int) * phrase2_corners_size * MOVE_COUNT);
	phase2_fill_heuristic(goalState,phrase2_corners,phrase2_corners_size,phase2_corners);




	memset(phase1_edges_move,0xff,sizeof(int) * phrase1_edges_size * MOVE_COUNT);
	phase1_edges_fill_heuristic(goalState);

	memset(phase1_co_move,0xff,sizeof(int) * phrase1_co_size * MOVE_COUNT);
	phase1_fill_heuristic(goalState,phrase1_co,phrase1_co_size,phase1_co);
	
	memset(phase1_eo_move,0xff,sizeof(int) * phrase1_eo_size * MOVE_COUNT);
	phase1_fill_heuristic(goalState,phrase1_eo,phrase1_eo_size,phase1_eo);
	
	phase2_fill_pre();
	phase1_fill_pre();
	
	printf("------------------------------------------\n");


	struct timeval timeEnd, timeStart; 
	gettimeofday(&timeStart, NULL );	


	phase(currentState);
	
	gettimeofday( &timeEnd, NULL ); 
	long long total_time = (timeEnd.tv_sec - timeStart.tv_sec) * 1000000 + (timeEnd.tv_usec - timeStart.tv_usec); 
	printf("total time is %lld us\n", total_time);



}
