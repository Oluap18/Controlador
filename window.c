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

int calc(char* cmd, int linhas, int* valores, int j){
	int i;
	int res=0;
	if((j-linhas)<0)
		i= 0;
	else
		i=j-linhas;
	if(!strcmp(cmd, "avg")){
		if(j==0 || linhas==0)
			return 0;
		for(; i<j; i++)
			res+=valores[i];
		if(j<linhas)
			return res/j;
		else
			return res/linhas;
	}
	else{ if(!strcmp(cmd, "max")){
		for(; i<j; i++){
			if(i==0)
				res = valores[i];
			if(res<valores[i])
				res = valores[i];
		}
		return res;
	}
	else{ if(!strcmp(cmd, "min")){
		for(; i<j; i++){
			if(i==0)
				res = valores[i];
			if(res>valores[i])
				res = valores[i];
		}
		return res;
	}
	else{ if(!strcmp(cmd, "sum")){
		for(; i<j; i++)
			res+=valores[i];
		return res;
	}}}}
	return 0;
}

void window(char* coluna, char* cmd, char* linha){
	char *args[1024], buf[1024], *aux[1024], resultado[1024];
	int n, i, col = atoi(coluna), lin = atoi(linha),j,k, v=0;
	while((n = readln(0, buf, 1024))>1){
		buf[n]= '\0';
		args[i++] = strdup(buf);
	}
	int val[i];
	for(j=0, k=0; k < i; k++){
		strcpy(buf , args[k]);
		aux[j] = strtok(buf, ":\0");
		while(aux[j]!=NULL)
			aux[++j] = strtok(NULL, ":\0");
		val[v++] = atoi(aux[col-1]);
	}
	for(j=0;j<i;j++){
		sprintf(resultado, "%d", calc(cmd, lin, val, j));
		strcat(args[j], ":");
		strcat(args[j],resultado);
		strcat(args[j], "\n");
		write(1, args[j], strlen(args[j]));
	}
}

void main(int argc, char* argv[]){
	if(!strcmp(argv[1], "window"))
		window(argv[2], argv[3], argv[4]);
}