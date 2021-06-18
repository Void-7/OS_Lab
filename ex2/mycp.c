#include<stdio.h>
#include<unistd.h>
#include <fcntl.h>

int main(int argc,char* argv[]){
	if(argc!=3){ printf("there should be a file name and a pathname arg!try again.");return -1;}
	int f=open(argv[1],O_RDONLY);
	if(f==-1) { printf("no such files!try again");return -2;}
	int r;char buf;
	int w=creat(argv[2],0777);
	if(w==-1){ printf("target file create error!try again");close(f);return -3;}
	while((r=read(f,&buf,1))>0){
		write(w,&buf,1);
	}
	if(r<0) printf("src file read error!");
	else printf("write OK");
	close(f);


	return 0;
}
