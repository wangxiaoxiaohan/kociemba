#include <unistd.h>
#include <stdio.h>
#include "cubestate.h"

class utils{
public:
	utils();
	~utils();
	static void dump2file(const char *filepath,int size,void *buf);
	static void readFromFile(const char *filepath,int size,void *buf,int position);
	bool cubeVerify(cube_t& cube);
private:
	int swapCount(int8_t *nums,int size);

};


