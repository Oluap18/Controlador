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

/**
*
*/
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

/**
*	Primeiro argumento, número de linhas do input, segundo argumento
* o input, terceiro argumento, coluna a comparar, quarto argumento comando
* a executar, quinto argumento, linhas a comparar
* aux serve como auxiliar para separar os argumentos em :
* val contêm os valores da coluna que estamos a calcular
* input contêm os inputs
* buf serve como auxiliar para separar os argumentos do input
*/
void main(int argc, char* argv[]){
	int i=0, k, j;
	int n_input = atoi(argv[1]), coluna = atoi(argv[n_input+2]), linha = atoi(argv[n_input+4]);
	char** input = (char**)malloc(sizeof(char[1024])*64);
	char *aux[1024], cmd[1024], buf[1024], resultado[1024];
	int val[n_input];
	strcpy(cmd, argv[n_input+3]);
	for(j=0; j< n_input; j++){
		input[j]=strdup(argv[j+2]);
	}
	for(j=0, k=0; k < n_input; k++){
		strcpy(buf , input[k]);
		aux[j] = strtok(buf, ":\0");
		while(aux[j]!=NULL)
			aux[++j] = strtok(NULL, ":\0");
		val[i++] = atoi(aux[coluna-1]);
	}
	sprintf(resultado, "%d", calc(cmd, linha, val, n_input-1));
	strcat(input[n_input-1], ":");
	strcat(input[n_input-1],resultado);
	strcat(input[n_input-1], "\n");
	write(1, input[n_input-1], strlen(input[n_input-1]));
}