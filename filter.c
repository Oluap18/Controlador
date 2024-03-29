#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <fcntl.h> 


int parse(char* op, int val1, int val2){
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

/**
*	Primeiro vem o input, depois a coluna 1, o operador a comparar, e a coluna 2
*/
void main(int argc, char* argv[]){
	char buf[1024], temp[1024];
	char* cmd[1024];
	int i, coluna1 = atoi(argv[2]), coluna2 = atoi(argv[4]), n;
	strcat(buf, argv[1]);
	i=0;
	strcpy(temp, buf);
	strcat(temp, "\n");
	cmd[i] = strtok(buf, ":\n\0");
	while(cmd[i]!=NULL)
		cmd[++i] = strtok(NULL, ":\n\0");
	if(parse(argv[3], atoi(cmd[coluna1-1]), atoi(cmd[coluna2-1])))
		write(1, temp, strlen(temp));
	write(1, "\0 ", 1);
	exit(0);
}