#include<vector>
#include<string>
#include<map>
#include<stdio.h>
using namespace std;
typedef vector<int> vi;
//可优化项 带'的逆时针操作做成正向三步
map<string,int> moveTable{
	{"U",0},{"U2",1},{"U'",2},
	{"D",3},{"D2",4},{"D'",5},
	{"F",6},{"F2",7},{"U'",8},
	{"B",9},{"B2",10},{"B'",11},
	{"L",12},{"L2",13},{"L'",14},
	{"R",15},{"R2",16},{"R'",17}
};
//每个面前四个为棱块 后四个为角块 
//角块排列顺序 需要注意匹配算法中的【偶数索引角块delta为2，奇数为1】规则，因此每个面的第五个数，也就是第一个角块要选对
//棱块应该无所谓？
int affectedCubies[][8] = {
	{  0,  1,  2,  3,  0,  1,  2,  3 },   // U
	{  4,  7,  6,  5,  4,  5,  6,  7 },   // D
	{  0,  9,  4,  8,  0,  3,  5,  4 },   // F
	{  2, 10,  6, 11,  2,  1,  7,  6 },   // B
	{  3, 11,  7,  9,  3,  2,  6,  5 },   // L
	{  1,  8,  5, 10,  1,  0,  4,  7 },   // R
};
/*
1. 对于棱：FB会改变方向，其他均不改变
2. 对于角：UD不改变方向，其他的需要讨论，正好每个角在LRFB四个操作中方向一次改变1一次改变22
*/
vi applyMove ( int move, vi state ) {
	int turns = move % 3 + 1;
	int face = move / 3;
	while( turns-- ){
		vi oldState = state;
		for( int i=0; i<8; i++ ){
			int isCorner = i > 3;
			int target = affectedCubies[face][i] + isCorner*12; //isCorner * 12 跳过棱块的编号
			int killer = affectedCubies[face][(i&3)==3 ? i-3 : i+1] + isCorner*12;;//killer表示被移动到这个位置的块
//(face>1 && face<4)转动F面或者B面 棱块方向才会变    (face<2) ? 0 : 2 - (i&1) 转动U面D面 角块方向不会变 偶数索引角块delta为2，奇数为1
			int orientationDelta = (i<4) ? (face>1 && face<4) : (face<2) ? 0 : 2 - (i&1);
			state[target] = oldState[killer];
			//state[target+20] = (oldState[killer+20] + orientationDelta) % (2 + isCorner);
			state[target+20] = oldState[killer+20] + orientationDelta;  //转过来的块的方向加上delta
			if( !turns )
				state[target+20] %= 2 + isCorner;  //方向取余  棱块二循环 角块三循环
		}
	}
	return state;
}
int main(int argc ,char **argv){
	string init[] = { "UF", "UR", "UB", "UL", "DF", "DR", "DB", "DL", "FR", "FL", "BR", "BL",
		"UFR", "URB", "UBL", "ULF", "DRF", "DFL", "DLB", "DBR" };
	vi initState(40,0);
	for(int i = 0;i < 20 ; i++){
		initState[i] = i;
	}
	vi retState(40,0);
	retState =initState;
	for(int i = 1 ; i < argc; i++){
		string s(argv[i]);
		retState = applyMove(moveTable[s],retState);
	}
	//最后返回的状态，前20记录块的序号
	//后20为块的方向，表示逆时针转了几次
	for(int i = 0 ; i < 20; i++){
		string step =  init[retState[i]];
		if(i >= 12){
			for(int j = 0 ; j < retState[i + 20];j++){
				step = step[2] + step.substr(0,2);
			}
		}else{
			if(retState[i + 20] == 1)
				step = step.substr(1) + step[0];
		}
		//printf("%d ",retState[i + 20]);
		printf("%s ",step.c_str());
	}
} 
