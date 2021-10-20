#include "preparation.h"
#include "algorithm.h"
#include <string.h>
#include <queue>
#include <iostream> 

std::vector<cube_t> middle_edges_perms;
static int factorial_8[8] = {1,2,6,24,120,720,5040,40320};
static int factorial_4[4] ={1,2,6,24};
static int factorial_12_4[4] = {1,9,90,990};


int algorithm::cantor(int* nums,int size,int* factorial_n){
    int ans=0,sum=0;
    for(int i = 0;i < size; i++){
		for(int j = i+1;j < size; j++)
			if(nums[j] < nums[i])
				sum++;
		
	    ans += sum*factorial_n[size - i - 2];
	    sum=0;
    }
    return ans;
}

int algorithm::combine(int m,int n){
	if(m < n) return 0;
	int distribution = 1;
	int Arrangement = 1;
	for(int i = 0 ; i < n ;i++){
		Arrangement *= m-i;
		distribution *= n-i;
	}
	return Arrangement/distribution;
}
//此算法为m选n组合的所有case 进行编号，返回值即为结果序号 
//由于中层棱块编号为后四棱，即下标最大的四个块，因此，此算法
//有一个原则是: 组合填大下标时，序号就小.
//应用于棱块组合，即：当四个棱块完全归位时，结果序号为0
int algorithm::combineIndex(int *nums,int m,int n){

	int tarIndex = 1;
	int ret = 0;
	for(int i = m - 1 ;i >= 0 ;i-- ){
		if(nums[i] > 0){
			ret += combine(m - 1 - i, tarIndex);
			tarIndex++;
		}
	}
	return ret;
}


prepareSearch::prepareSearch(){

}

void prepareSearch::init(){
	cube_t cube;

	memset(ud_edges_perm_move,0xff,sizeof(int) * UD_EDGES_PERM_SIZE * MOVE_COUNT);
	phase2_fill_heuristic(cube,ud_edges_perm,UD_EDGES_PERM_SIZE,phase2_edges1);

	memset(middle_edges_perm_move,0xff,sizeof(int) * MIDDLE_EDGES_PERM_SIZE * MOVE_COUNT);
	phase2_fill_heuristic(cube,ud_edges_perm,MIDDLE_EDGES_PERM_SIZE,phase2_edges2);

	memset(corners_perm_move,0xff,sizeof(int) * CORNORS_PERM_SIZE * MOVE_COUNT);
	phase2_fill_heuristic(cube,corners_perm,CORNORS_PERM_SIZE,phase2_corners);



	memset(middle_edges_perm_orientation_move,0xff,sizeof(int) * MIDDLE_EDGES_PERM_ORIENTATION_SIZE * MOVE_COUNT);
	phase1_edges_fill_heuristic(cube);

	memset(cornors_orientation_move,0xff,sizeof(int) * CORNORS_ORIENTATION_SIZE * MOVE_COUNT);
	phase1_fill_heuristic(cube,cornors_orientation,CORNORS_ORIENTATION_SIZE,phase1_co);

	memset(edges_orientation_move,0xff,sizeof(int) * EDGES_ORIENTATION_SIZE * MOVE_COUNT);
	phase1_fill_heuristic(cube,edges_orientation,EDGES_ORIENTATION_SIZE,phase1_eo);



	phase2_fill_pre();
	phase1_fill_pre();
	
}
int prepareSearch::calculateIndex(cube_t &cube,int type){
	int ret = 0;
	switch(type){
		case phase1_co :{
			for(int i = 0 ; i < 7;i++){
				ret = ret * 3 + cube.co[i];
			}
		}
		break;
		case phase1_eo:{
			for(int i = 0 ; i < 11;i++){
				ret = ret * 2 + cube.eo[i];
			}
		}
		break;
		case phase1_edges:{		
			int comArr[12] = {0};
			int permArr[4] = {0};
			int x = 0;
			for(int i = 0 ; i < 12 ;i++){
				if(cube.ep[i] >= 8){
					comArr[i] = 1;
					permArr[x++] = cube.ep[i] - 8;
				}
			}
			int combina_index  = algorithm::combineIndex(comArr,12,4);
			int permu_Index = algorithm::cantor(permArr,4,factorial_4);			
			ret = combina_index * 24 + permu_Index;

		}
		break;
		case phase2_edges1:{		
			int permArr[8] = {0};
			for(int i = 0;i < 8;i++)
				permArr[i] = cube.ep[i];
			ret = algorithm::cantor(permArr,8,factorial_8);
		}
		break;
		case phase2_edges2:{
			int permArr[4] = {0};
			for(int i = 0 ;i < 4; i++)
				permArr[i] = cube.ep[i + 8] - 8;
			ret = algorithm::cantor(permArr,4,factorial_4);
		}
		break;
		case phase2_corners:{
			int permArr[8] = {0};
			for(int i = 0 ; i < 8 ; i++)
				permArr[i] = cube.cp[i];
			ret = algorithm::cantor(permArr,8,factorial_8);
		}
		break;
	}
	return ret;

}
void prepareSearch::phase2_fill_heuristic(cube_t cube,int8_t *dest,int destSize,enum pahse_type type){
	std::queue<std::pair<cube_t,int>> q;
	memset(dest,0xff,destSize);
	q.push(std::make_pair(cube,0));

	dest[0] = 0;
	if(type == phase2_edges2){
		middle_edges_perms.push_back(cube);
	}
	int count = 0;
	while(!q.empty()){ 
		std::pair<cube_t ,int> front = q.front();
		int fatherIndex =calculateIndex(front.first,type);
		for( int move=0; move<18; move++ ){
			if( MOVELIMIT & (1 << move) ){		

				cube_t current = cubeState::moveRotate(move, front.first);			
				int Index = calculateIndex(current,type);

				
				if(type == phase2_corners){
					corners_perm_move[fatherIndex][move] = Index;
				}else if(type == phase2_edges1){
					ud_edges_perm_move[fatherIndex][move] = Index;
				}else if(type == phase2_edges2){
					middle_edges_perm_move[fatherIndex][move] = Index;
				}
				if(dest[Index] == -1){
					int step = front.second;	
					dest[Index] = step + 1;
					q.push(std::make_pair(current,step+1));
					if(type == phase2_edges2){
						middle_edges_perms.push_back(current);
					}
				}
				count ++;
			}
		}
		q.pop();

	}


}

void prepareSearch::phase2_fill_pre(){
	int searchcount = 0;

	memset(cp_mep,0xff,sizeof(int8_t) * MIDDLE_EDGES_PERM_SIZE *  CORNORS_PERM_SIZE);
	std::queue<std::pair<phase2_pru_t,int>> q;
	phase2_pru_t first_t;
	first_t.corners = 0;
	first_t.edges2 = 0;
	q.push(std::make_pair(first_t,0));
	cp_mep[0] = 0;
	
	while(!q.empty()){ 
		std::pair<phase2_pru_t,int> front = q.front();
		int depth = front.second;
		searchcount ++;
		for( int move=0; move<18; move++ ){
			if( MOVELIMIT & (1 << move) ){			
				phase2_pru_t new_t;	
				new_t.corners = corners_perm_move[front.first.corners][move];
				new_t.edges2 = middle_edges_perm_move[front.first.edges2][move];
				if(cp_mep[new_t.corners * 24 + new_t.edges2] == -1){
					q.push(std::make_pair(new_t,depth + 1));
					cp_mep[new_t.corners * 24 + new_t.edges2] = depth + 1;
				}
			}
		}
		q.pop();
	}

	memset(ep_mep,0xff,sizeof(int8_t) * MIDDLE_EDGES_PERM_SIZE *  UD_EDGES_PERM_SIZE);
	std::queue<std::pair<phase2_pru_t,int>> q2;
	phase2_pru_t first_t2;
	first_t2.edges1 = 0;
	first_t2.edges2 = 0;
	q2.push(std::make_pair(first_t2,0));
	ep_mep[0] = 0;
	while(!q2.empty()){ 
		std::pair<phase2_pru_t,int> front = q2.front();
		int depth = front.second;
		searchcount++;
		for( int move=0; move<18; move++ ){
			if( MOVELIMIT & (1 << move) ){			
				phase2_pru_t new_t;	
				new_t.edges1 = ud_edges_perm_move[front.first.edges1][move];
				new_t.edges2 = middle_edges_perm_move[front.first.edges2][move];
				if(ep_mep[new_t.edges1 * 24 + new_t.edges2] == -1){
					q2.push(std::make_pair(new_t,depth + 1));
					ep_mep[new_t.edges1 * 24 + new_t.edges2] = depth + 1;
				}
			}
		}
		q2.pop();
	}

}
void prepareSearch::phase1_fill_heuristic(cube_t goalstate,int8_t *dest,int destSize,enum pahse_type type){
	std::queue<std::pair<cube_t,int>> q;
	memset(dest,0xff,destSize);
	std::pair<cube_t ,int> firstPair(goalstate,0);
	q.push(firstPair);
	dest[0] = 0;
	int count = 0;
	while(!q.empty()){ 
		std::pair<cube_t ,int> front = q.front();
		int fatherIndex =calculateIndex(front.first,type);
		for( int move=0; move<18; move++ ){
			int step = front.second;
			cube_t currstate = cubeState::moveRotate(move, front.first);
			int Index =calculateIndex(currstate,type);
		    if(type == phase1_co){
				cornors_orientation_move[fatherIndex][move] = Index;
			}else{
				edges_orientation_move[fatherIndex][move] = Index;
			}
			if(dest[Index] == -1){
				dest[Index] = step + 1;
				q.push(std::make_pair(currstate,step+1));
			}	
			count ++;
		}
		q.pop();

	}
}
void prepareSearch::phase1_edges_fill_heuristic(cube_t state){
	std::queue<std::pair<cube_t,int>> q;
	memset(middle_edges_perm_orientation,0xff,MIDDLE_EDGES_PERM_ORIENTATION_SIZE);
	std::pair<cube_t ,int> firstPair(state,0);
	q.push(firstPair);
	int count = 0 ;
	for(int i = 0 ; i <middle_edges_perms.size();i++){
		q.push(std::make_pair(middle_edges_perms[i],0));
		int Index = calculateIndex(middle_edges_perms[i], phase1_edges);
		middle_edges_perm_orientation[Index] = 0;
	}
	while(!q.empty()){ 
		std::pair<cube_t ,int> front = q.front();
		int FatherIndex = calculateIndex(front.first, phase1_edges);
		for( int move=0; move<18; move++ ){
			int step = front.second;
			cube_t currstate = cubeState::moveRotate(move, front.first);
			int Index = calculateIndex(currstate, phase1_edges);
			middle_edges_perm_orientation_move[FatherIndex][move] = Index;
			 if(middle_edges_perm_orientation[Index] == -1)
			 {
			    middle_edges_perm_orientation[Index] = step + 1;
				q.push(std::make_pair(currstate,step+1));
			 }
			 count++;
		}
		q.pop();
	}
}
void prepareSearch::phase1_fill_pre(){
	int searchcount = 0;
	memset(co_mec,0xff,sizeof(int8_t) * CORNORS_ORIENTATION_SIZE * MIDDLE_EDGES_COMBINATION);
	std::queue<std::pair<phase1_pru_t,int>> q;
	phase1_pru_t first_t;
	first_t.co = 0;
	first_t.middle_edges_combination = 0;
	q.push(std::make_pair(first_t,0));
	co_mec[0] = 0;
	while(!q.empty()){ 
		std::pair<phase1_pru_t,int> front = q.front();
		int depth = front.second;
		searchcount ++;
		for( int move=0; move<18; move++ ){		
				phase1_pru_t new_t;	
				new_t.co = cornors_orientation_move[front.first.co][move];
				new_t.middle_edges_combination = middle_edges_perm_orientation_move[front.first.middle_edges_combination][move];
				if(co_mec[new_t.co * MIDDLE_EDGES_COMBINATION + new_t.middle_edges_combination/24] == -1){
					q.push(std::make_pair(new_t,depth + 1));
					co_mec[new_t.co * MIDDLE_EDGES_COMBINATION + new_t.middle_edges_combination/24] = depth + 1;
				}
		}
		q.pop();
	}


	searchcount = 0;
	memset(eo_mec,0xff,sizeof(int8_t) * EDGES_ORIENTATION_SIZE * MIDDLE_EDGES_COMBINATION);
	std::queue<std::pair<phase1_pru_t,int>> q2;
	phase1_pru_t first_t2;
	first_t2.eo = 0;
	first_t2.middle_edges_combination = 0;
	q2.push(std::make_pair(first_t2,0));
	eo_mec[0] = 0;
	while(!q2.empty()){ 
		std::pair<phase1_pru_t,int> front = q2.front();
		int depth = front.second;
		searchcount ++;
		for( int move=0; move<18; move++ ){		
				phase1_pru_t new_t;	
				new_t.eo = edges_orientation_move[front.first.eo][move];
				new_t.middle_edges_combination = middle_edges_perm_orientation_move[front.first.middle_edges_combination][move];
				if(eo_mec[new_t.eo * MIDDLE_EDGES_COMBINATION + new_t.middle_edges_combination/24] == -1){
					q2.push(std::make_pair(new_t,depth + 1));
					eo_mec[new_t.eo * MIDDLE_EDGES_COMBINATION + new_t.middle_edges_combination/24] = depth + 1;
				}
		}
		q2.pop();
	}


}
bool prepareSearch::DFSphase2(search_t& se_t){
		for( int move=0; move<18; move++ ){
			if( MOVELIMIT & (1 << move) ){
				if(move / 3 == se_t.face || move / 3  == se_t.face - 1) continue;
				int phase2_edges1_index = ud_edges_perm_move[se_t.phase2_edges1_index][move];
				int phase2_edges2_index = middle_edges_perm_move[se_t.phase2_edges2_index][move];
				int phase2_corners_index = corners_perm_move[se_t.phase2_corners_index][move];
				//int val = max(ud_edges_perm[phase2_edges1_index],max(middle_edges_perm_orientation_move[phase2_edges2_index],corners_perm[phase2_corners_index]));
				int val =std::max(ep_mep[phase2_edges1_index * 24 + phase2_edges2_index],cp_mep[phase2_corners_index * 24 +phase2_edges2_index] 	);
				if(val + se_t.current_depth + 1 <= se_t.total_depth){
					(*se_t.steps)[se_t.current_depth] = move;
					if(val == 0){
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


bool prepareSearch::phase2(cube_t    cube,steps_t steps){
	for( int i = 0; i<steps.size(); i++ ){
		cube = cubeState::moveRotate(steps[i], cube);
	}
	int step2_len = (20 - steps.size()) > 10 ? 10 : ( 20 - steps.size());
	for(int depth = 0 ;depth <= step2_len; ++depth){
		steps_t steps2(depth);
		search_t search;
		search.face = 6;
		search.phase2_edges1_index = calculateIndex(cube, phase2_edges1);
		search.phase2_edges2_index = calculateIndex(cube, phase2_edges2);
		search.phase2_corners_index = calculateIndex(cube, phase2_corners);
		search.current_depth = 0;
		search.total_depth = depth;
		search.steps = &steps2;
	
		if(DFSphase2(search)){
			printSolution(steps);
			std::cout << std::endl;
			printSolution(steps2);	
			std::cout << std::endl;
			return true;
		}
	}
	return false;
}

bool prepareSearch::DFSphase1(search_t& se_t){

		for( int move=0; move<18; move++ ){
			if(move / 3 == se_t.face ) continue;

			int phase1_co_index = cornors_orientation_move[se_t.phase1_co_index][move];
			int phase1_eo_index = edges_orientation_move[se_t.phase1_eo_index][move];
			int phase1_edges_index = middle_edges_perm_orientation_move[se_t.phase1_edges_index][move];
			
			//int val = std::max(std::max(cornors_orientation[phase1_co_index],edges_orientation[phase1_eo_index]),middle_edges_perm_orientation[phase1_edges_index]); 
			int val = std::max(co_mec[MIDDLE_EDGES_COMBINATION * phase1_co_index + phase1_edges_index/24],
								 eo_mec[MIDDLE_EDGES_COMBINATION * phase1_eo_index   + phase1_edges_index/24]);
			if(val + se_t.current_depth + 1 <= se_t.total_depth){
				(*se_t.steps)[se_t.current_depth] = move;
				if(val == 0){
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

bool prepareSearch::phase(cube_t cube){

	for(int depth = 0 ; ; ++depth){
		steps_t steps(depth);
		search_t search;

		search.face = 6;
		search.initstate =cube;
		search.current_depth = 0;
		search.total_depth = depth;
		search.steps = &steps;
		search.phase1_co_index = calculateIndex(cube,phase1_co);
		search.phase1_eo_index = calculateIndex(cube,phase1_eo);
		search.phase1_edges_index = calculateIndex(cube,phase1_edges);

 		if(DFSphase1(search))
			break;
	}
}
void prepareSearch::printSolution(steps_t steps){
	for( int i = 0; i < steps.size(); i++ ){
		std::cout << " ";
		int movesteps = (steps[i]%3+1);
		if(movesteps ==3){
			std::cout << "UDFBLR"[steps[i]/3] << "'";
		}else if(movesteps == 1){
			std::cout << "UDFBLR"[steps[i]/3];
		}else {
			std::cout << "UDFBLR"[steps[i]/3] << steps[i]%3+1;
		}
	}

}


