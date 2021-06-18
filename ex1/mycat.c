#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

int main(int argc,char* argv[]){
	if(argc!=2){ printf("incorrect filename arg!try again.");return -1;}
	int f=open(argv[1],O_RDONLY);
	if(f==-1) { printf("no such files!try again");return -2;}
	int r;char buf;
	while((r=read(f,&buf,1))>0){
		printf("%c",buf);
	}
	if(r<0) printf("read error!");
	close(f);
	return 0;
}
