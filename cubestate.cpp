#include<vector>
#include<string>
#include<map>
#include<stdio.h>
#include "cubestate.h"

#define  INCRERASE_CORNOR_ORIENTION_LOOP(input) \
({ \
	int tmp = input;	\
	if(input < 2)			\
		tmp = 1;	\
	else 	\
		tmp = -2; 		\
	(input + tmp);	\
})
#define  DECRERASE_CORNOR_ORIENTION_LOOP(input) \
({ \
	int tmp = input;	\
	if(input > 0)			\
		tmp = -1;	\
	else	\
		tmp = 2;	\
	(input + tmp);	\
})


cube_t cubeState::moveRotate(int move, cube_t & cube){
	int face = move / 3;
	int rotateCount = move % 3 + 1;
	cube_t ret = cube;
	/*************************
	==========================
	postion
	==========================
	*************************/
	for(int i = 0 ; i < 4 ;i++){
			ret.ep[face_edges[face][i]] = cube.ep[face_edges[face][(i + rotateCount) % 4]];
			ret.cp[face_cornors[face][i]] = cube.cp[face_cornors[face][(i + rotateCount) % 4]];
	}
	/***********************
	========================
	oriention
	========================
	***********************/	
	// cornors oriention
	//Counterclockwise
	// F/B/L/R 2->1 ,1->0, 0-2 when block move from D face to U face or U face to D face  
	// F/B/L/R 0->1 ,1->2, 2-0 when block move from D face to D face or U face to U face 

	//clockwise(3 Counterclockwise rotate)
	// F/B/L/R 0->1 ,1->2, 2-0 when block move from D face to U face or U face to D face  
	// F/B/L/R 2->1 ,1->0, 0-2 when block move from D face to D face or U face to U face 
	if(face > 1 && (rotateCount == 1 || rotateCount == 3))
	{
		if(rotateCount == 1){
			ret.co[face_cornors[face][0]] =  DECRERASE_CORNOR_ORIENTION_LOOP(cube.co[face_cornors[face][1]]);
			ret.co[face_cornors[face][1]] =  INCRERASE_CORNOR_ORIENTION_LOOP(cube.co[face_cornors[face][2]]);
			ret.co[face_cornors[face][2]] =  DECRERASE_CORNOR_ORIENTION_LOOP(cube.co[face_cornors[face][3]]);
			ret.co[face_cornors[face][3]] =  INCRERASE_CORNOR_ORIENTION_LOOP(cube.co[face_cornors[face][0]]);
		}else{
			ret.co[face_cornors[face][0]] =  DECRERASE_CORNOR_ORIENTION_LOOP(cube.co[face_cornors[face][3]]);
			ret.co[face_cornors[face][1]] =  INCRERASE_CORNOR_ORIENTION_LOOP(cube.co[face_cornors[face][0]]);
			ret.co[face_cornors[face][2]] =  DECRERASE_CORNOR_ORIENTION_LOOP(cube.co[face_cornors[face][1]]);
			ret.co[face_cornors[face][3]] =  INCRERASE_CORNOR_ORIENTION_LOOP(cube.co[face_cornors[face][2]]);
		}
	}else{
		for(int i = 0 ; i < 4 ;i++){
			ret.co[face_cornors[face][i]] = cube.co[face_cornors[face][(i + rotateCount) % 4]];
		}
	}
	// edges oriention 	
	for(int i = 0 ; i < 4 ;i++){
		//only 90°| 270 ° F/B face turns will change edge blocks oriention   
		if((face >= 2 && face <= 3) && (rotateCount == 1 || rotateCount ==3))
			ret.eo[face_edges[face][i]] = cube.eo[face_edges[face][(i + rotateCount) % 4]] == 1 ? 0:1;
		else
			ret.eo[face_edges[face][i]] = cube.eo[face_edges[face][(i + rotateCount) % 4]];
	}
	return ret;
}


