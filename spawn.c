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


void main(int argc, char* argv[]){
	//spawn(argc-2, &argv[2]);
	char args[1024], buf[1024], *aux[1024], *coluna;
	int r,i=0, j, k, col, status;
	strcpy(buf, argv[1]);
	strcpy(args, buf);

	aux[k] = strtok(buf, ":\0");
	while(aux[k]!=NULL)
		aux[++k] = strtok(NULL, ":\0");

	if(fork()==0){
		for(j=2;j<argc;j++){
			if(argv[j][0]=='$'){
				coluna = strtok(argv[j], "$");
				col = atoi(coluna);
				strcpy(argv[j],aux[col-1]);
			}
		}
		execvp(argv[2], &argv[2]);
		perror("ERRO: Comando inválido.");
		_exit(-1);
	}
		
	wait(&status);
	sprintf(buf, "%d", WEXITSTATUS(status));
	strcat(args, ":");
	strcat(args, buf);
	strcat(args, "\n");
	write(1, args, strlen(args));
}