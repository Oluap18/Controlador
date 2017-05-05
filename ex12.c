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

int filter(char* op, int val1, int val2){
	if(!strcmp(op, "<"))
		return val1 < val2;
	else{ if(!strcmp(op, "<="))
		return val1 <= val2;
	else{ if(!strcmp(op, ">"))
		return val1 > val2;
	else{ if(!strcmp(op, ">="))
		return val1 >= val2;
	else{ if(!strcmp(op, "="))
		return val1 == val2;
	else{ if(!strcmp(op, "!="))
		return val1 != val2;
	}}}}}
	return 0;
}

void ex12(char* col1, char* op, char* col2){
	char buf[1024], temp[1024];
	char* cmd[1024];
	int i, coluna1 = atoi(col1), coluna2 = atoi(col2), n;
	while((n = readln(0, buf, 1024))>1){
		i=0;
		buf[n] = '\n';
		strcpy(temp, buf);
		cmd[i] = strtok(buf, ":\n");
		while(cmd[i]!=NULL)
			cmd[++i] = strtok(NULL, ":\n");
		if(filter(op, atoi(cmd[coluna1-1]), atoi(cmd[coluna2-1])))
			write(1, temp, strlen(temp));
	}
}

void main(int argc, char* argv[]){
	if(!strcmp(argv[1], "filter"))
		ex12(argv[2], argv[3], argv[4]);
}