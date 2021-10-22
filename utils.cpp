#include "utils.h"
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

