#include <string.h>
#include <queue>
#include <iostream> 
#include <unistd.h>
#include "utils.h"
#include "preparation.h"
#include "algorithm.h"

static int factorial_8[8] = {1,2,6,24,120,720,5040,40320};
static int factorial_4[4] ={1,2,6,24};


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
	char buf[80];
	getcwd(buf,sizeof(buf));
	char path[128];
	sprintf(path,"%s/cacheFile",buf);

	if(access(path,F_OK) == 0){
		readCache(path);
	}else{
		init();	
		writeCache(path);
	}
}
prepareSearch::~prepareSearch(){

}

void prepareSearch::writeCache(const char *cachePath){
	utils::dump2file(cachePath,sizeof(int) * CORNORS_ORIENTATION_SIZE * MOVE_COUNT,(void *)cornors_orientation_move);
	utils::dump2file(cachePath,sizeof(int) * EDGES_ORIENTATION_SIZE * MOVE_COUNT,(void *)edges_orientation_move);
	utils::dump2file(cachePath,sizeof(int) * MIDDLE_EDGES_PERM_ORIENTATION_SIZE * MOVE_COUNT,(void *)middle_edges_perm_orientation_move);
	utils::dump2file(cachePath,sizeof(int8_t) * CORNORS_ORIENTATION_SIZE * MIDDLE_EDGES_COMBINATION,(void *)co_mec);
	utils::dump2file(cachePath,sizeof(int8_t) * EDGES_ORIENTATION_SIZE * MIDDLE_EDGES_COMBINATION,(void *)eo_mec);

	utils::dump2file(cachePath,sizeof(int) * CORNORS_PERM_SIZE * MOVE_COUNT,(void *)corners_perm_move);
	utils::dump2file(cachePath,sizeof(int) * UD_EDGES_PERM_SIZE * MOVE_COUNT,(void *)ud_edges_perm_move);
	utils::dump2file(cachePath,sizeof(int) * MIDDLE_EDGES_PERM_SIZE * MOVE_COUNT,(void *)middle_edges_perm_move);
	utils::dump2file(cachePath,sizeof(int8_t) * MIDDLE_EDGES_PERM_SIZE * CORNORS_PERM_SIZE,(void *)cp_mep);
	utils::dump2file(cachePath,sizeof(int8_t) * MIDDLE_EDGES_PERM_SIZE * UD_EDGES_PERM_SIZE,(void *)ep_mep);
}
void prepareSearch::readCache(const char *cachePath){

	int postion = 0;
	utils::readFromFile(cachePath,sizeof(int) * CORNORS_ORIENTATION_SIZE * MOVE_COUNT,(void *)cornors_orientation_move,postion);
	postion += sizeof(int) * CORNORS_ORIENTATION_SIZE * MOVE_COUNT;
	utils::readFromFile(cachePath,sizeof(int) * EDGES_ORIENTATION_SIZE * MOVE_COUNT,(void *)edges_orientation_move,postion);
	postion += sizeof(int) * EDGES_ORIENTATION_SIZE * MOVE_COUNT;
	utils::readFromFile(cachePath,sizeof(int) * MIDDLE_EDGES_PERM_ORIENTATION_SIZE * MOVE_COUNT,(void *)middle_edges_perm_orientation_move,postion);
	postion += sizeof(int) * MIDDLE_EDGES_PERM_ORIENTATION_SIZE * MOVE_COUNT;
	utils::readFromFile(cachePath,sizeof(int8_t) * CORNORS_ORIENTATION_SIZE * MIDDLE_EDGES_COMBINATION,(void *)co_mec,postion);
	postion += sizeof(int8_t) * CORNORS_ORIENTATION_SIZE * MIDDLE_EDGES_COMBINATION;
	utils::readFromFile(cachePath,sizeof(int8_t) * EDGES_ORIENTATION_SIZE * MIDDLE_EDGES_COMBINATION,(void *)eo_mec,postion);
	postion += sizeof(int8_t) * EDGES_ORIENTATION_SIZE * MIDDLE_EDGES_COMBINATION;

	utils::readFromFile(cachePath,sizeof(int) * CORNORS_PERM_SIZE * MOVE_COUNT,(void *)corners_perm_move,postion);
	postion += sizeof(int) * CORNORS_PERM_SIZE * MOVE_COUNT;
	utils::readFromFile(cachePath,sizeof(int) * UD_EDGES_PERM_SIZE * MOVE_COUNT,(void *)ud_edges_perm_move,postion);
	postion += sizeof(int) * UD_EDGES_PERM_SIZE * MOVE_COUNT;
	utils::readFromFile(cachePath,sizeof(int) * MIDDLE_EDGES_PERM_SIZE * MOVE_COUNT,(void *)middle_edges_perm_move,postion);
	postion += sizeof(int) * MIDDLE_EDGES_PERM_SIZE * MOVE_COUNT;
	utils::readFromFile(cachePath,sizeof(int8_t) * MIDDLE_EDGES_PERM_SIZE * CORNORS_PERM_SIZE,(void *)cp_mep,postion);
	postion += sizeof(int8_t) * MIDDLE_EDGES_PERM_SIZE * CORNORS_PERM_SIZE;
	utils::readFromFile(cachePath,sizeof(int8_t) * MIDDLE_EDGES_PERM_SIZE * UD_EDGES_PERM_SIZE,(void *)ep_mep,postion);

}

void prepareSearch::init(){
	cube_t cube;

	memset(ud_edges_perm_move,0xff,sizeof(int) * UD_EDGES_PERM_SIZE * MOVE_COUNT);
	phase2_fill_buffer(cube,ud_edges_perm,UD_EDGES_PERM_SIZE,ud_edges_perm_index);

	memset(middle_edges_perm_move,0xff,sizeof(int) * MIDDLE_EDGES_PERM_SIZE * MOVE_COUNT);
	phase2_fill_buffer(cube,ud_edges_perm,MIDDLE_EDGES_PERM_SIZE,middle_edges_perm_index);

	memset(corners_perm_move,0xff,sizeof(int) * CORNORS_PERM_SIZE * MOVE_COUNT);
	phase2_fill_buffer(cube,corners_perm,CORNORS_PERM_SIZE,cornors_perm_index);



	memset(middle_edges_perm_orientation_move,0xff,sizeof(int) * MIDDLE_EDGES_PERM_ORIENTATION_SIZE * MOVE_COUNT);
	phase1_fill_buffer(cube,middle_edges_perm_orientation,MIDDLE_EDGES_PERM_ORIENTATION_SIZE,me_combine_index);

	memset(cornors_orientation_move,0xff,sizeof(int) * CORNORS_ORIENTATION_SIZE * MOVE_COUNT);
	phase1_fill_buffer(cube,cornors_orientation,CORNORS_ORIENTATION_SIZE,co_index);

	memset(edges_orientation_move,0xff,sizeof(int) * EDGES_ORIENTATION_SIZE * MOVE_COUNT);
	phase1_fill_buffer(cube,edges_orientation,EDGES_ORIENTATION_SIZE,eo_index);


	phase2_fill_pre();
	phase1_fill_pre();

	
}
int prepareSearch::calculateIndex(cube_t &cube,int type){
	int ret = 0;
	switch(type){
		case co_index :{
			for(int i = 0 ; i < 7;i++){
				ret = ret * 3 + cube.co[i];
			}
		}
		break;
		case eo_index:{
			for(int i = 0 ; i < 11;i++){
				ret = ret * 2 + cube.eo[i];
			}
		}
		break;
		case me_combine_index:{		
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
		case ud_edges_perm_index:{		
			int permArr[8] = {0};
			for(int i = 0;i < 8;i++)
				permArr[i] = cube.ep[i];
			ret = algorithm::cantor(permArr,8,factorial_8);
		}
		break;
		case middle_edges_perm_index:{
			int permArr[4] = {0};
			for(int i = 0 ;i < 4; i++)
				permArr[i] = cube.ep[i + 8] - 8;
			ret = algorithm::cantor(permArr,4,factorial_4);
		}
		break;
		case cornors_perm_index:{
			int permArr[8] = {0};
			for(int i = 0 ; i < 8 ; i++)
				permArr[i] = cube.cp[i];
			ret = algorithm::cantor(permArr,8,factorial_8);
		}
		break;
	}
	return ret;

}
void prepareSearch::phase2_fill_buffer(cube_t cube,int8_t *dest,int destSize,enum phase_type type){
	std::queue<std::pair<cube_t,int>> q;
	memset(dest,0xff,destSize);
	q.push(std::make_pair(cube,0));

	dest[0] = 0;
	if(type == middle_edges_perm_index){
		middle_edges_perms.push_back(cube);
	}
	while(!q.empty()){ 
		std::pair<cube_t ,int> front = q.front();
		int fatherIndex =calculateIndex(front.first,type);
		for( int move=0; move<18; move++ ){
			if( MOVELIMIT & (1 << move) ){		

				cube_t current = cubeState::moveRotate(move, front.first);			
				int Index = calculateIndex(current,type);

				
				if(type == cornors_perm_index){
					corners_perm_move[fatherIndex][move] = Index;
				}else if(type == ud_edges_perm_index){
					ud_edges_perm_move[fatherIndex][move] = Index;
				}else if(type == middle_edges_perm_index){
					middle_edges_perm_move[fatherIndex][move] = Index;
				}
				if(dest[Index] == -1){
					int step = front.second;	
					dest[Index] = step + 1;
					q.push(std::make_pair(current,step+1));
					if(type == middle_edges_perm_index){
						middle_edges_perms.push_back(current);
					}
				}
			}
		}
		q.pop();

	}


}

void prepareSearch::phase2_fill_pre(){
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
void prepareSearch::phase1_fill_buffer(cube_t goalstate,int8_t *dest,int destSize,enum phase_type type){
	std::queue<std::pair<cube_t,int>> q;
	memset(dest,0xff,destSize);
	std::pair<cube_t ,int> firstPair(goalstate,0);
	q.push(firstPair);
	dest[0] = 0;
	if(type == me_combine_index){
		for(int i = 0 ; i < MIDDLE_EDGES_PERM_SIZE;i++){
			q.push(std::make_pair(middle_edges_perms[i],0));
			int Index = calculateIndex(middle_edges_perms[i], me_combine_index);
			dest[Index] = 0;
		}
	}

	while(!q.empty()){ 
		std::pair<cube_t ,int> front = q.front();
		int fatherIndex =calculateIndex(front.first,type);
		for( int move=0; move<18; move++ ){
			int step = front.second;
			cube_t currstate = cubeState::moveRotate(move, front.first);
			int Index =calculateIndex(currstate,type);
		    if(type == co_index){
				cornors_orientation_move[fatherIndex][move] = Index;
			}else if(type == eo_index){
				edges_orientation_move[fatherIndex][move] = Index;
			}else{
				middle_edges_perm_orientation_move[fatherIndex][move] = Index;
			}
			if(dest[Index] == -1){
				dest[Index] = step + 1;
				q.push(std::make_pair(currstate,step+1));
			}	
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
bool prepareSearch::DFSphase2(search_info_t& se_t){
		for( int move=0; move<18; move++ ){
			if( MOVELIMIT & (1 << move) ){
				if(move / 3 == se_t.face) continue;
				int ud_edges_perm_index = ud_edges_perm_move[se_t.ud_edges_perm_index][move];
				int middle_edges_perm_index = middle_edges_perm_move[se_t.middle_edges_perm_index][move];
				int cornors_perm_index = corners_perm_move[se_t.cornors_perm_index][move];
				int val =std::max(ep_mep[ud_edges_perm_index * 24 + middle_edges_perm_index],cp_mep[cornors_perm_index * 24 +middle_edges_perm_index] 	);
				if(val + se_t.current_depth  < se_t.total_depth){
					(*se_t.steps).steps[se_t.current_depth] = move;
					if(val == 0){
							return true;
					}	
					search_info_t newSe_t = se_t;
					newSe_t.current_depth += 1;
					newSe_t.ud_edges_perm_index = ud_edges_perm_index;
					newSe_t.middle_edges_perm_index = middle_edges_perm_index;
					newSe_t.cornors_perm_index = cornors_perm_index;
					newSe_t.face = move / 3;
					if(DFSphase2(newSe_t))
						return true;			
				} 
			}
		}
			
		return false;
	
}


bool prepareSearch::phase2(cube_t    cube,moves_t moves){
	for( int i = 0; i < moves.vaildLength; i++ ){
		cube = cubeState::moveRotate(moves.steps[i], cube);
	}
	int phase2_len = (MAX_STEP - moves.vaildLength) > 10 ? 10 : (MAX_STEP - moves.vaildLength);
	for(int depth = 0 ;depth <= phase2_len; depth++){
		moves_t moves2(depth);
		search_info_t search;
		search.face = -1;
		search.ud_edges_perm_index = calculateIndex(cube, ud_edges_perm_index);
		search.middle_edges_perm_index = calculateIndex(cube, middle_edges_perm_index);
		search.cornors_perm_index = calculateIndex(cube, cornors_perm_index);
		search.current_depth = 0;
		search.total_depth = depth;
		search.steps = &moves2;
	
		if(DFSphase2(search)){
			printSolution(moves);
			printSolution(moves2);	
			return true;
		}
	}
	return false;
}

bool prepareSearch::DFSphase1(search_info_t& se_t){

		for( int move = 0; move < 18; move++ ){
			if(move / 3 == se_t.face ) continue;

			int co_index = cornors_orientation_move[se_t.co_index][move];
			int eo_index= edges_orientation_move[se_t.eo_index][move];
			int me_combine_index = middle_edges_perm_orientation_move[se_t.me_combine_index][move];
			
			//int val = std::max(std::max(cornors_orientation[co_index],edges_orientation[phase1_eo_index]),middle_edges_perm_orientation[me_combine_index]); 
			int val = std::max(co_mec[MIDDLE_EDGES_COMBINATION * co_index + me_combine_index/24],
								 eo_mec[MIDDLE_EDGES_COMBINATION * eo_index  + me_combine_index/24]);
			if(val + se_t.current_depth  < se_t.total_depth){
				(*se_t.steps).steps[se_t.current_depth] = move;
				if(val == 0){
					if(phase2(se_t.initstate,*se_t.steps))	
						return true;
				}
				search_info_t newSe_t = se_t;
				newSe_t.current_depth += 1;
				newSe_t.face = move / 3;
				newSe_t.co_index = co_index;
				newSe_t.eo_index= eo_index;
				newSe_t.me_combine_index = me_combine_index;
				if(DFSphase1(newSe_t)){
						return true;
				}
								
			} 
		}
			
		return false; 
	
}

void  prepareSearch::solve(cube_t cube){
	int depth = 0;
	
	while(true){
		moves_t moves(depth);
		search_info_t search;

		search.face = -1;
		search.initstate =cube;
		search.current_depth = 0;
		search.total_depth = depth;
		search.steps = &moves;
		search.co_index = calculateIndex(cube,co_index);
		search.eo_index= calculateIndex(cube,eo_index);
		search.me_combine_index = calculateIndex(cube,me_combine_index);

 		if(DFSphase1(search))
			break;
		depth ++; 
	}

}
void prepareSearch::printSolution(moves_t s){
	for( int i = 0; i < s.vaildLength; i++ ){
		std::cout << " ";
		int movesteps = (s.steps[i]%3+1);
		if(movesteps ==3){
			std::cout << "UDFBLR"[s.steps[i]/3] << "'";
		}else if(movesteps == 1){
			std::cout << "UDFBLR"[s.steps[i]/3];
		}else {
			std::cout << "UDFBLR"[s.steps[i]/3] << s.steps[i]%3+1;
		}
	}
	std::cout << std::endl;;

}


