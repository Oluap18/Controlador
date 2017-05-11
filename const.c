#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <fcntl.h> 

ssize_t readln(int fildes, char *buf, size_t nbyte){
	int i=0;
	int n;
	while(i<nbyte && (n = read(fildes, buf+i, 1))>0 && *(buf+i)!='\n')
		i++;
	if(n<0) return n;
	return i;
}

void cons(char* arg){
	char buf[1024];
	int r;
	while((r = readln(0, buf, 1024))>1){
		buf[r]= '\0';
		strcat(buf, ":");
		strcat(buf, arg);
		strcat(buf, "\n");
		write(1, buf, strlen(buf));
	}
}

void main(int argc, char* argv[]){
	if(!strcmp(argv[1], "const"))
		cons(argv[2]);
}