 #include <stdlib.h> 
#include <stdio.h>
#include "preparation.h"
#include <cstring>
#include "search.h"
#include <memory>
#include <sys/time.h>   
#include <iostream> 

std::map<std::string,int> str2move{
	{"U",0},{"U2",1},{"U'",2},
	{"D",3},{"D2",4},{"D'",5},
	{"F",6},{"F2",7},{"U'",8},
	{"B",9},{"B2",10},{"B'",11},
	{"L",12},{"L2",13},{"L'",14},
	{"R",15},{"R2",16},{"R'",17}
};

int main(int argc ,char **argv){
	cube_t cube;

	std::shared_ptr<prepareSearch> prs(new prepareSearch());
	std::shared_ptr<cubeState> mover(new cubeState());
	for(int i = 1 ; i < argc; i++){
		std::string move(argv[i]);
		cube = mover->moveRotate(str2move[move],cube);		
	}
	
	struct timeval timeEnd, timeStart; 
	gettimeofday(&timeStart, NULL );	
	prs->solve(cube);
	
	gettimeofday( &timeEnd, NULL ); 
	long long total_time = (timeEnd.tv_sec - timeStart.tv_sec) * 1000000 + (timeEnd.tv_usec - timeStart.tv_usec); 
	std::cout<< "search time is " << total_time << "us" << std::endl;

	
	
}

