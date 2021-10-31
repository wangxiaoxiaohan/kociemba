#include "utils.h"
utils::utils(){

}
utils::~utils(){

}

void utils::dump2file(const char *filepath,int size,void *buf){
	FILE *fp = fopen(filepath,"a+");
	if(fp == NULL){
		printf("open failed!\n");
		return;
	}
		
	fwrite(buf,1,size,fp);
	fclose(fp);
}
void utils::readFromFile(const char *filepath,int size,void *buf,int position){
	FILE *fp = fopen(filepath,"r");
	if(fp == NULL){
		printf("open failed!\n");
		return;
	}
	fseek(fp,position,SEEK_SET);
	fread(buf,1,size,fp);
	fclose(fp);
}

bool utils::cubeVerify(cube_t& cube){

	int coSum = 0;
	int eoSum = 0;
	int flagC[8] = {0};
	int flagE[12] = {0};
	for(int i = 0 ; i < 12 ;i++){
		
		if(cube.ep[i] >= 12 || cube.ep[i] < 0){
			printf("not legal edge block number : %d !\n",cube.ep[i]);
			return false;		
		}
		if(flagE[cube.ep[i]] == 1){
			printf("edge block repeated! illegal!\n");
			return false;
		}
		flagE[cube.ep[i]] = 1;
		eoSum += cube.eo[i];


		if(i < 8){
			if(cube.cp[i] >= 8 || cube.cp[i] < 0){
				printf("not legal cornor block number : %d !\n",cube.cp[i]);
				return false;		
			}
			if(flagC[cube.cp[i]] == 1){
				printf("cornor block repeated! illegal!\n");
				return false;
			}
			flagC[cube.cp[i]] = 1;	
			coSum += cube.co[i];
		}
			
	}
	if((eoSum % 2 != 0) || (coSum % 3 != 0)){
		printf("block orientation is illegal!\n");
		return false;
	}
	

	int ceSwapCount = 0 ;
	ceSwapCount += swapCount(cube.ep,12);
	ceSwapCount += swapCount(cube.cp,8);
	if(ceSwapCount % 2 != 0){
		printf("block position is  illegal!\n");
		return false;
	} 
	
	
	return true;
}
int utils::swapCount(int8_t *nums,int size){
	int ret = 0;
	int currentPosition = 0;
	int flags[size] = {0};
	while(1){
		int i ; 
		for(i = 0; i < size ; i++){
			if(flags[i] == 0 && nums[i] != i){
				currentPosition = i;
				break;
			}	
		}
		printf("------\n");
		if(i == size) break;
		ret -= 1;
		while(flags[currentPosition] == 0){
			printf("currentPosition:%d\n",currentPosition);
			ret += 1;
			flags[currentPosition] = 1;
			currentPosition = nums[currentPosition];		
			
		}
		
	}
	return ret;
}


