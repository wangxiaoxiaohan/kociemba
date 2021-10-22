#include <unistd.h>
#include <stdio.h>

class utils{
public:
	utils();
	~utils();
	static void dump2file(const char *filepath,int size,void *buf);
	static void readFromFile(const char *filepath,int size,void *buf,int position);

};


