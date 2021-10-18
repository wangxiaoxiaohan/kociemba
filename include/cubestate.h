#include <map>
#include <vector>
#ifndef __CUBE_STATE___
#define __CUBE_STATE___
struct cube_t{
	int8_t ep[12];
	int8_t cp[8];
	int8_t eo[12];
	int8_t co[8];
	cube_t(){
		for(int8_t i = 0 ; i  < 12 ; i++){
			if(i < 8){
				cp[i] = i;
				co[i] = 0;
			}
			ep[i] = i;
			eo[i] = 0;
		}
	}
};
static const int8_t face_edges[6][4] = {
	{  0,  1,  2,  3},   // U   
	{  4,  7,  6,  5},   // D
	{  0,  9,  4,  8},   // F
	{  2, 10,  6, 11},   // B
	{  3, 11,  7,  9},   // L
	{  1,  8,  5, 10},   // R
};
//除了 U面 和D 面的块，F/B/L/R面四个块的编号前两个一定是定是同在U面，后两个同在D面
//方便编写旋转的算法
static const int8_t face_cornors[6][4] ={
	{   0,  1,  2,  3 },   // U   
	{   4,  5,  6,  7 },   // D
	{   0,  3,  5,  4 },   // F
	{   2,  1,  7,  6 },   // B
	{   3,  2,  6,  5 },   // L
	{   1,  0,  4,  7 },   // R
};

class cubeState
{
public:
	cubeState(){}
	~cubeState(){}
	static cube_t moveRotate(int move,cube_t& cube);
private:


};
#endif