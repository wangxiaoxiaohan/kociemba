#include <unistd.h>
#include <stdio.h>

#define DUMP_PATH "/mnt/hgfs/ubuntu_share/cube/my_kociemba/new_kociemba/kociemba/cacheFile"
class utils{
public:
	utils();
	~utils();
	static void dump2file(const char *filepath,int size,void *buf);
	static void readFromFile(const char *filepath,int size,void *buf,int position);

};


