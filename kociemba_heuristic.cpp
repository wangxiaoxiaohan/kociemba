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

/*需要优化的地方 ：
  3.phase1 phase2 api合并
  4.启发表写文件
  5.做一个cube 结构体
*/
#define MOVE_COUNT 18
#define MOVE_COUNT_2 10

static const int phrase2_corners_size = 40320; // 8!
static const int phrase2_edges1_size = 40320;  // 8!
static const int phrase2_edges2_size = 24;	   // 4!
static const int phrase2_edges_size = 24 * 40320;	   // 4!

static const int phrase1_edges_size = 12 * 11 * 10 * 9 * 16;
static const int phrase1_co_size = 6561;
static const int phrase1_eo_size = 4096;

int8_t phrase1_edges[phrase1_edges_size];
int8_t phrase1_co[phrase1_co_size];//角块方向 这里定义了 3^8种 事实上其中只有3^7的合法的,旋转过程中不会出现，为了方便索引才这样使用
int8_t phrase1_eo[phrase1_eo_size];

int phase1_co_move[phrase1_co_size][MOVE_COUNT];
int phase1_eo_move[phrase1_eo_size][MOVE_COUNT];
int phase1_edges_move[phrase1_edges_size][MOVE_COUNT];


int8_t phrase2_corners[phrase2_corners_size ];
int8_t phrase2_edges1[phrase2_edges1_size ];
int8_t phrase2_edges2[phrase2_edges2_size];


int8_t phrase2_edges[phrase2_edges_size];


int phrase2_corners_move[phrase2_corners_size ][MOVE_COUNT_2];
int phrase2_edges1_move[phrase2_edges1_size ][MOVE_COUNT_2];
int phrase2_edges2_move[phrase2_edges2_size][MOVE_COUNT_2];

int phrase2_edges_move[phrase2_edges_size][MOVE_COUNT_2];


//int applicableMoves[] = { 0, 262143, 259263, 74943, 74898 };
int moveLimit = 74943;
int affectedCubies[][8] = {
	{  0,  1,  2,  3,  0,  1,  2,  3 },   // U   
	{  4,  7,  6,  5,  4,  5,  6,  7 },   // D
	{  0,  9,  4,  8,  0,  3,  5,  4 },   // F
	{  2, 10,  6, 11,  2,  1,  7,  6 },   // B
	{  3, 11,  7,  9,  3,  2,  6,  5 },   // L
	{  1,  8,  5, 10,  1,  0,  4,  7 },   // R
};
typedef vector<int> vi;
typedef vi steps_t; 


static vi factorial_8 ={1,2,6,24,120,720,5040,40320};
static vi factorial_4 ={1,2,6,24};
static vi factorial_12_4 = {1,9,90,990};
static vi power2_8 = {128,64,32,16,8,4,2,1};
static vi power2_12 = {2048,1024,512,256,128,64,32,16,8,4,2,1};
static vi power3_8 = {2187,729,243,81,27,9,3,1};

vector<vi> phase2_edges2_all;
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
	vi state;
	vi initstate;
	int  face, total_depth,current_depth;
	steps_t* steps;
	int phase1_co_index;
	int phase1_eo_index;
	int phase1_edges_index;
	int phase2_edges1_index;
	int phase2_edges2_index;
	int phase2_corners_index;
	int phase2_edges_index;
};


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
int calculateIndex(vi state,int type){
	int ret = 0;
	switch(type){
		case phase1_co :
		{
			vi co(state.begin() + 32,state.begin() + 40);
			for(int i = 0 ; i < co.size();i++){
				ret += power3_8[i] * co[i];
			}
		}
		break;
		case phase1_eo:
		{
			vi eo(state.begin() + 20,state.begin() + 32);
			for(int i = 0 ; i < eo.size();i++){
				ret += power2_12[i] * eo[i];
			}
		}
		break;
		case phase1_edges:
		{
			vi edges_blockPosition(state.begin(),state.begin() + 12);
			int OrIndex = 0;

			for(int i = 0; i < 12;i++){
				if(edges_blockPosition[i] < 8)
					edges_blockPosition[i] = 0;
			}  
			ret = perm(edges_blockPosition,factorial_12_4);
			for(int i = 0 ; i < 12 ;i ++){
				if(state[i] >= 8)
					OrIndex +=	power2_8[ state[i] - 8 + 4] * state[i + 20] ;	
			}
			ret = ret * 16 + OrIndex;
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
		case phase2_edges:
		{
			
			vi edges1Blocks(state.begin()  ,state.begin() + 8);
			for(int i = 0;i < edges1Blocks.size(); i++){
				edges1Blocks[i] = edges1Blocks[i] - 8;
			}
			int edges1_index = cantor(edges1Blocks,factorial_8);
			
			
			vi edges2Blocks(state.begin() + 8,state.begin() +12);
			for(int i = 0;i < edges2Blocks.size(); i++){
				edges2Blocks[i] = edges2Blocks[i] - 12;
			}
			int edges2_index = cantor(edges2Blocks,factorial_4);

			ret = edges1_index * 24 + edges2_index;
		
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
				}else if(type == phase2_edges){
					phrase2_edges_move[fatherIndex][move] = Index;
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
		printf("phase2_edges2_all size %d\n",phase2_edges2_all.size());
	}

	printf("serch count %d \n",count);

}
/**/
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
		// to do 复用
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
	printf("serch count %d \n",count);
}

//应该把其余八个棱块也考虑进来  
void phase1_edges_fill_heuristic(vi state){
	queue<pair<vi,int>> q;
	memset(phrase1_edges,0xff,phrase1_edges_size);
	pair<vi ,int> firstPair(state,0);
	q.push(firstPair);
	printf("phase2_edges2_all size %d\n",phase2_edges2_all.size());
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
			phase1_edges_move[FatherIndex][move] = Index;
			 if(phrase1_edges[Index] == -1)
			 {
			    phrase1_edges[Index] = step + 1;
				q.push(make_pair(currstate,step+1));
			 }
		}
		q.pop();
	}
		
}

int estimate(vi state,int tpye){
	if(tpye == 0){
		vi co(state.begin() + 32,state.begin() + 40);
		int co_index = 0;
		int co_estimat_value  = 0;
		for(int i = 0 ; i < co.size();i++){
			co_index += power3_8[i] * co[i];
		}

		co_estimat_value = phrase1_co[co_index];

	//	printf("co_estimat_value:%d  ",co_estimat_value);




		vi eo(state.begin() + 20,state.begin() + 28);
		int eo_index = 0;
		int eo_estimat_value  = 0;
		for(int i = 0 ; i < eo.size();i++){
			eo_index += power2_8[i] * eo[i];
		}

		eo_estimat_value = phrase1_eo[eo_index];

	//	printf("eo_estimat_value:%d  ",eo_estimat_value);




		int edges_estimate_value = 0 ;
		int edges_Index = 0;
		vi edges_blockPosition(state.begin(),state.begin() + 12);
	
		int OrIndex = 0;

		for(int i = 0; i < 12;i++){
			if(edges_blockPosition[i] < 8)
				edges_blockPosition[i] = 0;
		}  
		edges_Index = perm(edges_blockPosition,factorial_12_4);
		for(int i = 0 ; i < 12 ;i ++){
			if(state[i] >= 8)
				OrIndex +=	power2_8[ state[i] - 8 + 4] * state[i + 20] ;	
		}

		edges_Index = edges_Index * 16 + OrIndex;

		edges_estimate_value = phrase1_edges[edges_Index];
	
	//	printf("edges_Index:%d edges_estimate_value:%d  \n",edges_Index,edges_estimate_value);
		
		return   max(co_estimat_value,max(eo_estimat_value,edges_estimate_value)) ;

	}else{	
		
		vi edgesBlocks(state.begin() ,state.begin() + 8);
		int edgesBlocksIndex = cantor(edgesBlocks,factorial_8);

	//	printf("phrase2_edges1[edgesBlocksIndex]:%d  ",phrase2_edges1[edgesBlocksIndex]);
		
		vi mediaEdgeBlocks(state.begin() +  8 ,state.begin() + 12);
		for(int i = 0 ;i < mediaEdgeBlocks.size(); i++){
			//printf("mediaEdgeBlocksIndex[i] %d\n",mediaEdgeBlocks[i]);
			mediaEdgeBlocks[i] = mediaEdgeBlocks[i] - 8;
		}
		int mediaEdgeBlocksIndex = cantor(mediaEdgeBlocks,factorial_4);

	//	printf("phrase2_edges2[mediaEdgeBlocksIndex]:%d  ",phrase2_edges2[mediaEdgeBlocksIndex]);

		vi cornorBlocks(state.begin() +  12 ,state.begin() + 20);
		for(int i = 0 ;i < cornorBlocks.size(); i++){
			cornorBlocks[i] = cornorBlocks[i] - 12;
		}
		int cornorBlocksIndex = cantor(cornorBlocks,factorial_8);

	//	printf("phrase2_corners[cornorBlocksIndex]:%d  ",phrase2_corners[cornorBlocksIndex]);
 
		//int maxVal  = max(phrase2_edges1[edgesBlocksIndex * 24 + mediaEdgeBlocksIndex],phrase2_corners[cornorBlocksIndex * 24 + mediaEdgeBlocksIndex]);
		int maxVal = max(phrase2_edges1[edgesBlocksIndex],max(phrase2_corners[cornorBlocksIndex],phrase2_edges2[mediaEdgeBlocksIndex]));
		return maxVal;

	}
	return 0;

}

int total_serch_count = 0;
long long total_time_applymove;
long long total_time_estmate;




//int search_count = 0;
bool DFSphase2(search_t& se_t){
		for( int move=0; move<18; move++ ){
			if( moveLimit & (1 << move) ){
			//	search_count ++;
				if(move / 3 == se_t.face ) continue;
				vi newstate = applyMove(move, se_t.state);
				int estimateval = estimate(newstate,1);
			//	int phase2_edges1_index = phrase2_edges1_move[se_t.phase2_edges1_index][move];
			//	int phase2_edges2_index = phrase2_edges2_move[se_t.phase2_edges2_index][move];
			//	int phase2_edges2_index = phrase2_edges_move[se_t.phase2_edges_index][move];
			//	int phase2_corners_index = phrase2_corners_move[se_t.phase2_corners_index][move];
			//	int estimateval = max(phrase2_edges[phase2_edges2_index],phrase2_corners[phase2_corners_index]);
				if(estimateval + se_t.current_depth + 1 <= se_t.total_depth){
					(*se_t.steps)[se_t.current_depth] = move;
					if(estimateval == 0){
						//	printf("find phase current_depth %d!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!",se_t.current_depth);
							return true;
					}	
					search_t newSe_t = se_t;
					newSe_t.current_depth += 1;
				//	newSe_t.phase2_edges1_index = phase2_corners_index;
				//	newSe_t.phase2_edges2_index = phase2_edges2_index;
				//	newSe_t.phase2_edges_index = phase2_edges2_index;
				//	newSe_t.phase2_corners_index = phase2_corners_index;
					newSe_t.state = newstate;
					newSe_t.face = move / 3;
					if(DFSphase2(newSe_t))
						return true;			
				} 
			}
		}
			
		return false;
	
}
int phase1_done = 0;

bool phase2(vi   state,steps_t steps){
	for( int i=0; i<steps.size(); i++ ){
		state = applyMove(steps[i], state);
	}
	for(int depth = 0 ;depth <= 10; ++depth){
		steps_t steps2(depth);
		search_t search;
		search.face = 6;
		search.state = state;
		search.current_depth = 0;
		search.total_depth = depth;
		search.steps = &steps2;
	
		if(DFSphase2(search) && (steps.size() + steps2.size()) <= 21){
			
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
			printf("      ");
			for( int i=0; i<depth; i++ ){
				printf("  ");
				int movesteps = (steps2[i]%3+1);
				if(movesteps ==3){
					cout << "UDFBLR"[steps2[i]/3] << "'";
				}else if(movesteps == 1){
					cout << "UDFBLR"[steps2[i]/3];
				}else {
					cout << "UDFBLR"[steps2[i]/3] << steps2[i]%3+1;
				}
			}
			printf("\n");
			printf("phase1_done count %d",phase1_done);
			return true;
		}
	}
	return false;
}
bool DFSphase1(search_t& se_t){

		for( int move=0; move<18; move++ ){
			if(move / 3 == se_t.face ) continue;
			//struct timeval timeEnd, timeMid,timeStart; 
			//gettimeofday(&timeStart, NULL );
			//vi newstate = applyMove(move, se_t.state);
			//gettimeofday(&timeMid, NULL );
			//int estimateval = estimate(newstate,0);
			//gettimeofday(&timeEnd, NULL );
			//total_time_applymove += (timeMid.tv_sec - timeStart.tv_sec) * 1000000 + (timeMid.tv_usec - timeStart.tv_usec); 
			//total_time_estmate += (timeEnd.tv_sec - timeMid.tv_sec) * 1000000 + (timeEnd.tv_usec - timeMid.tv_usec); 
			int phase1_co_index = phase1_co_move[se_t.phase1_co_index][move];
			int phase1_eo_index = phase1_eo_move[se_t.phase1_eo_index][move];
			int phase1_edges_index = phase1_edges_move[se_t.phase1_edges_index][move];
			
			int estimateval = max(max(phrase1_co[phase1_co_index],phrase1_eo[phase1_eo_index]),phrase1_edges[phase1_edges_index]); 
			//printf("%d %d %d %d %d  %d %d move %d\n",phase1_co_index,phrase1_co[phase1_co_index],phase1_eo_index,phrase1_eo[phase1_eo_index],phase1_edges_index,phrase1_edges[phase1_edges_index],estimateval,move);
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
			//	newSe_t.state = newstate;
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
		//search.state = state;
		search.initstate =state;
		search.current_depth = 0;
		search.total_depth = depth;
		search.steps = &steps;
		search.phase1_co_index = calculateIndex(state,phase1_co);
		search.phase1_eo_index = calculateIndex(state,phase1_eo);
		//printf("search.phase1_eo_index %d\n",search.phase1_eo_index);
		search.phase1_edges_index = calculateIndex(state,phase1_edges);

		if(DFSphase1(search))
			break;
	}
	/*
	for(int depth = 0 ;; ++depth){
		printf("!!!!!!!!!!!!!!!!!!!!!depth %d \n",depth);
		steps_t steps(depth);
		search_t search;
		search.face = 6;
		search.state = state;
		search.current_depth = 0;
		search.total_depth = depth;
		search.steps = &steps;
	//	search.phase2_edges1_index = calculateIndex(state,phase2_edges1);
	//	search.phase2_edges2_index = calculateIndex(state,phase2_edges2);
	//	search.phase2_corners_index = calculateIndex(state,phase2_corners);
	//	search.phase2_edges_index = calculateIndex(state,phase2_edges);
		
		if(DFSphase2(search)){
			for( int i=0; i<depth; i++ ){
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
			printf("\n");
			break;
		}
	}
	*/
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

	
	phase2_fill_heuristic(goalState,phrase2_edges1,phrase2_edges1_size,phase2_edges1);
	phase2_fill_heuristic(goalState,phrase2_edges2,phrase2_edges2_size,phase2_edges2);

	memset(phrase2_corners_move,-1,sizeof(int) * phrase2_corners_size * MOVE_COUNT_2);
	phase2_fill_heuristic(goalState,phrase2_corners,phrase2_corners_size,phase2_corners);

//	memset(phrase2_edges_move,-1,sizeof(int) * phrase2_edges_size * MOVE_COUNT_2);
//	phase2_fill_heuristic(goalState,phrase2_edges,phrase2_edges_size,phase2_edges);



	memset(phase1_edges_move,-1,sizeof(int) * phrase1_edges_size * MOVE_COUNT);
	phase1_edges_fill_heuristic(goalState);

	memset(phase1_co_move,-1,sizeof(int) * phrase1_co_size * MOVE_COUNT);
	phase1_fill_heuristic(goalState,phrase1_co,phrase1_co_size,phase1_co);
	
	memset(phase1_eo_move,-1,sizeof(int) * phrase1_eo_size * MOVE_COUNT);
	phase1_fill_heuristic(goalState,phrase1_eo,phrase1_eo_size,phase1_eo);

	

	
	
	printf("------------------------------------------\n");

	struct timeval timeEnd, timeStart; 
	gettimeofday(&timeStart, NULL );	


	phase(currentState);

	gettimeofday( &timeEnd, NULL ); 
	long long total_time = (timeEnd.tv_sec - timeStart.tv_sec) * 1000000 + (timeEnd.tv_usec - timeStart.tv_usec); 
	printf("total time is %lld ms\n", total_time/1000);
	//printf("total_time_applymove is %lld ms\n", total_time_applymove/1000);
	//printf("total_time_estmate is %lld ms\n", total_time_estmate/1000);

}
