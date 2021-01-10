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
  1.初始化启发函数和搜索解法用到的索引号查找代码 要复用
  2.中层棱块的表使用数组
  3.phase1 phase2 api合并
  4.启发表写文件
  5.做一个cube 结构体
  6.同一个面转动的case优化
*/
#define MOVE_COUNT 18

static const int phrase2_corners_size = 40320; // 8!
static const int phrase2_edges1_size = 40320;  // 8!
static const int phrase2_edges2_size = 24;	   // 4!
static const int phrase1_edges_size = 12 * 11 * 10 * 9 * 16;

static const int phrase1_co_size = 6561;
static const int phrase1_eo_size = 1 << 8;



int8_t phrase1_edges[phrase1_edges_size];
//角块方向 这里定义了 3^8种 事实上其中只有3^7的合法的,旋转过程中不会出现，为了方便索引才这样使用
int8_t phrase1_co[phrase1_co_size];
//棱块方向 这里定义了 2^8种 事实上其中只有2^7的合法的,旋转过程中不会出现，为了方便索引才这样使用
int8_t phrase1_eo[phrase1_eo_size];
int8_t phase1_co_move[phrase1_co_size][MOVE_COUNT];
int8_t phase1_eo_move[phrase1_eo_size][MOVE_COUNT];
int8_t phase1_edges_move[phrase1_edges_size][MOVE_COUNT];


int8_t phrase2_corners[phrase2_corners_size ];
int8_t phrase2_edges1[phrase2_edges1_size ];
int8_t phrase2_edges2[phrase2_edges2_size];
int8_t phase2_co_move[phrase1_co_size][MOVE_COUNT];

int applicableMoves[] = { 0, 262143, 259263, 74943, 74898 };
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
static vi power2_8 = {128,64,32,16,8,4,2,1};
static vi power3_8 = {2187,729,243,81,27,9,3,1};
static vi factorial_12_4 = {1,9,90,990};
vector<vi> phase2_edges2_all;
enum pahse_type{
	phase1_eo = 0,
	phase1_co,
	phase1_edges,
	
	phase2_edges1,
	phase2_edges2,
	phase2_corners
};
struct search_t
{
	vi state;
	int  face, total_depth,current_depth;
	steps_t* steps;
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
void phase2_fill_heuristic(vi state,int8_t *dest,int destSize,vi factorial_n,int factorial_size,enum pahse_type type){
	queue<pair<vi,int>> q;
	memset(dest,0xff,destSize);
	q.push(make_pair(state,0));

	dest[0] = 0;
	if(type == phase2_edges2){
		state[28] = state[29] =state[30] =state[31] = 0;
		phase2_edges2_all.push_back(state);
	}
	
	int count = 0;
	while(!q.empty()){ 
		pair<vi ,int> front = q.front();
		for( int move=0; move<18; move++ ){
			if( applicableMoves[3] & (1 << move) ){
				
				vi currstate = applyMove(move, front.first);
			
				//to do 使用位运算
				vi blockPos(currstate.begin() + ((type - 3) * 8 - (type == 5 ? 1 : 0 ) * 4),currstate.begin() + ((type - 2) * 8 - (type == 3 ? 0 : 1 ) * 4));
				//角块的值要减去12 中层棱块减去8
				for(int i = 0 ; i < blockPos.size(); i++)
					blockPos[i] = blockPos[i] - ((type - 2) * 4) +	(type == 3 ? 1 : 0 ) * 4;
	
					
				//int blockPosIndex = findIndex(blockPos,factorial_n,factorial_size);
				int blockPosIndex = cantor(blockPos,factorial_n);
			//	printf("blockPosIndex : %d\n",blockPosIndex);
				if(dest[blockPosIndex] == -1){
					int step = front.second;	
					dest[blockPosIndex] = step + 1;
					q.push(make_pair(currstate,step+1));
					if(type == phase2_edges2){
						currstate[28] = currstate[29] =currstate[30] =currstate[31] = 0;
						phase2_edges2_all.push_back(currstate);
					}
				}
				count ++;
			}
		}
		q.pop();

	}
	printf("serch count %d \n",count);

}
/**/
void phase1_fill_heuristic(vi goalstate,int8_t *dest,int destSize,vi power_n,int power_size,enum pahse_type type){
	queue<pair<vi,int>> q;
	//to do 初始化queue
	memset(dest,0xff,destSize);
	pair<vi ,int> firstPair(goalstate,0);
	q.push(firstPair);
	dest[0] = 0;
	while(!q.empty()){ 
		pair<vi ,int> front = q.front();
		//复用 to do
		int fatherIndex = 0;
		vi fatherblockOrientation(front.first.begin() + 20 + type * 12,front.first.begin() + 28 + type * 12);	
		for(int i = 0 ; i < power_size ; i++){
			fatherIndex +=  fatherblockOrientation[i] *	power_n[i];
		}
		
		for( int move=0; move<18; move++ ){
			int step = front.second;
			vi currstate = applyMove(move, front.first);
			int Index = 0;
			vi blockOrientation(currstate.begin() + 20 + type * 12,currstate.begin() + 28 + type * 12);	
			for(int i = 0 ; i < power_size ; i++){
				Index +=  blockOrientation[i] *	power_n[i];
			}
			
		    if(type == phase1_co)
				phase1_co_move[fatherIndex][move] = Index;
			else
				phase1_eo_move[fatherIndex][move] = Index;
			
			if(dest[Index] == -1){
				dest[Index] = step + 1;
				q.push(make_pair(currstate,step+1));
			}		
		}
		q.pop();

	}

}
//应该把其余八个棱块也考虑进来  
void phase1_edges_fill_heuristic(vi state){
	queue<pair<vi,int>> q;
	//to do 初始化queue
	memset(phrase1_edges,0xff,phrase1_edges_size);
	pair<vi ,int> firstPair(state,0);
	q.push(firstPair);
	printf("phase2_edges2_all size %d\n",phase2_edges2_all.size());
	for(int i = 0 ; i <phase2_edges2_all.size();i++){
		q.push(make_pair(phase2_edges2_all[i],0));
		vi initstate = phase2_edges2_all[i];
		int Index = 0;
		vi edgesPos(initstate.begin(),initstate.begin() + 12);
		for(int i = 0 ; i < 12 ; i++){
			if(!(edgesPos[i] >= 8)) edgesPos[i] =0;
		}
		Index = perm(edgesPos,factorial_12_4);
		//printf("Index %d \n",Index);
		Index = Index * 16 ;
		phrase1_edges[Index] = 0;
		

	}
	while(!q.empty()){ 
		pair<vi ,int> front = q.front();

		//to do 复用
		int FatherIndex = 0;
		int FatherOrIndex = 0;
		vi faedgesPos(front.first.begin(),front.first.begin() + 12);
		for(int i = 0 ; i < 12 ; i++){
			if(!(faedgesPos[i] >= 8)) faedgesPos[i] =0;
		}		
		FatherIndex = perm(faedgesPos,factorial_12_4);

		
		for(int i = 0 ; i < 12 ;i ++){
			if((front.first)[i] >= 8){
				FatherOrIndex +=  power2_8[ (front.first)[i] - 8 + 4] * (front.first)[i + 20] ;
			}
		}
		FatherIndex = FatherIndex * 16 + FatherOrIndex;

	
		for( int move=0; move<18; move++ ){
			int step = front.second;
			vi currstate = applyMove(move, front.first);
			int Index = 0;
			int OrIndex = 0;
			vi edgesPos(currstate.begin(),currstate.begin() + 12);
			for(int i = 0 ; i < 12 ; i++){
				if(!(edgesPos[i] >= 8)) edgesPos[i] =0;
			}		
			Index = perm(edgesPos,factorial_12_4);

			
			for(int i = 0 ; i < 12 ;i ++){
				if(currstate[i] >= 8){
					OrIndex +=  power2_8[ currstate[i] - 8 + 4] * currstate[i + 20] ;
				}
			}
			Index = Index * 16 + OrIndex;

			phase1_edges_move[FatherIndex][move] = Index;
			

			 if(phrase1_edges[Index] == -1)
			 {
			    phrase1_edges[Index] = step + 1;
				//pair<vi,int> newpair(currstate,step + 1);
				q.push(make_pair(currstate,step+1));
				//printf("step count %d \n",step); 
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
		/*
		if(phrase1_co[co_index] == -1){
			printf("@@@@@@@@@@do not find co_estimat_value !!!!!!!co_index :%d ",co_index);
			//exit(0);
			sleep(1);
		}
		*/
		co_estimat_value = phrase1_co[co_index];

	//	printf("co_estimat_value:%d  ",co_estimat_value);




		vi eo(state.begin() + 20,state.begin() + 28);
		int eo_index = 0;
		int eo_estimat_value  = 0;
		for(int i = 0 ; i < eo.size();i++){
			eo_index += power2_8[i] * eo[i];
		}
		/*
		if(phrase1_eo[eo_index] == -1){
			printf("@@@@@@@@@@do not find eo_estimat_value !!!!!!!eo_index :%d ",eo_index);
			//exit(0);
			sleep(1);
		}
		*/
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


bool DFSphase1(search_t& se_t){

		for( int move=0; move<18; move++ ){
			if(move / 3 == se_t.face ) continue;
			//struct timeval timeEnd, timeMid,timeStart; 
			//gettimeofday(&timeStart, NULL );
			vi newstate = applyMove(move, se_t.state);
			//gettimeofday(&timeMid, NULL );
			int estimateval = estimate(newstate,0);
			//gettimeofday(&timeEnd, NULL );
			//total_time_applymove += (timeMid.tv_sec - timeStart.tv_sec) * 1000000 + (timeMid.tv_usec - timeStart.tv_usec); 
			//total_time_estmate += (timeEnd.tv_sec - timeMid.tv_sec) * 1000000 + (timeEnd.tv_usec - timeMid.tv_usec); 
			if(estimateval + se_t.current_depth + 1 <= se_t.total_depth){
				(*se_t.steps)[se_t.current_depth] = move;
				if(estimateval == 0){
						return true;
				}
				
				search_t newSe_t = se_t;
				newSe_t.current_depth += 1;
				newSe_t.state = newstate;
				newSe_t.face = move / 3;
				if(DFSphase1(newSe_t))
					return true;			
			} 
		}
			
		return false;
	
}

//int search_count = 0;
bool DFSphase2(search_t& se_t){
		for( int move=0; move<18; move++ ){
			if( applicableMoves[3] & (1 << move) ){
			//	search_count ++;
				if(move / 3 == se_t.face ) continue;
				vi newstate = applyMove(move, se_t.state);
				int estimateval = estimate(newstate,1);
		
			//	printf("estimateval  %d move %d  se_t.current_depth %d se_t.total_depth %d\n",estimateval,move,se_t.current_depth,se_t.total_depth);
				//printf("estimateval + se_t.current_depth + 1  %d\n",estimateval + se_t.current_depth + 1);
				if(estimateval + se_t.current_depth + 1 <= se_t.total_depth){
					(*se_t.steps)[se_t.current_depth] = move;
					if(estimateval == 0){
						//	printf("find phase current_depth %d!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!",se_t.current_depth);
							return true;
					}	
					search_t newSe_t = se_t;
					newSe_t.current_depth += 1;
					newSe_t.state = newstate;
					newSe_t.face = move / 3;
					if(DFSphase2(newSe_t))
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
		search.state = state;
		search.current_depth = 0;
		search.total_depth = depth;
		search.steps = &steps;
		if(DFSphase1(search)){
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
				state = applyMove(steps[i], state);
			}
			printf("\n");
			break;
			
		}
	}
	/**/	
	//int estimateval = estimate(state,1);
	//printf("estimateval %d\n",estimateval);
	for(int depth = 0 ;/*depth <= 10*/; ++depth){
		printf("!!!!!!!!!!!!!!!!!!!!!depth %d \n",depth);
		steps_t steps(depth);
		search_t search;
		search.face = 6;
		search.state = state;
		search.current_depth = 0;
		search.total_depth = depth;
		search.steps = &steps;
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



	phase2_fill_heuristic(goalState,phrase2_edges1,phrase2_edges1_size, factorial_8,8,phase2_edges1);
	phase2_fill_heuristic(goalState,phrase2_edges2,phrase2_edges2_size, factorial_4,4,phase2_edges2);
	phase2_fill_heuristic(goalState,phrase2_corners,phrase2_corners_size, factorial_8,8,phase2_corners);
	//phase2_fill_edges(goalState);
	//phase2_fill_cornors(goalState);
	
	phase1_fill_heuristic(goalState,phrase1_co,phrase1_co_size,power3_8,8,phase1_co);
	phase1_fill_heuristic(goalState,phrase1_eo,phrase1_eo_size,power2_8,8,phase1_eo);
	phase1_edges_fill_heuristic(goalState);

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
