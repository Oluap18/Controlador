#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <fcntl.h> 

struct nodo{
	int id;
	int* writer;
	int sizeW;
	char** cmd;
	int pipe[2];
	char** input;
	int inp_point;
};

typedef struct nodo* Nodo;



ssize_t readln(int fildes, char *buf, size_t nbyte){
	int i=0;
	int n;
	while(i<nbyte && (n = read(fildes, buf+i, 1))>0 && *(buf+i)!='\n' && *(buf+i)!='\0'){
		i++;
		buf[i]='\0';
	}
	if(n<0) return n;
	return i;
}

void rem(int* arr, int p, int size){
	int i;
	for(i=p; i < size; i++)
		arr[i] = arr[i+1];
}



Nodo initNodo(int id, char** cm){
	Nodo n = (Nodo) malloc(sizeof(struct nodo));
	pipe(n->pipe);
	n -> id = id;
	n -> writer = (int*) malloc(sizeof(int)*10);
	n -> sizeW = 0;
	n -> cmd = (char**) malloc(sizeof(char*)*10);
	n -> cmd = cm;
	if(!strcmp("window", cm[0]))
		n -> input = (char**) malloc(sizeof(char[1024])*atoi(cm[3]));
	else n-> input = (char**) malloc(sizeof(char[1024])*1);
	n -> inp_point = 0;
	if(fork()==0){
		char buffer[1024];
		int r;
		char** comand;
		if(!strcmp("const", n -> cmd[0])){
			comand = (char**) malloc(sizeof(char[1024])*4);
			comand[0] = strdup("./const");
			comand[2] = strdup(n -> cmd[1]);
			comand[3] = NULL;
		}
		if(!strcmp("filter", n -> cmd[0])){
			comand = (char**) malloc(sizeof(char[1024])*6);
			comand[0] = strdup("./filter");
			comand[2] = strdup(n -> cmd[1]);
			comand[3] = strdup(n -> cmd[2]);
			comand[4] = strdup(n -> cmd[3]);
			comand[5] = NULL;
		}
		if(!strcmp("spawn", n -> cmd[0])){
			int i = 2;
			comand = (char**) malloc(sizeof(char[1024])*10);
			comand[0] = strdup("./spawn");
			while(n -> cmd[i-1] != NULL ){
				comand[i] = strdup( n -> cmd[i-1]);
				i++;
			}
			comand[i] = NULL;
		}
		close(n->pipe[1]);
		while((r=readln( n->pipe[0], buffer, 1024 ))){
			buffer[r] = '\0';
			comand[1] = strdup(buffer);
			if(fork()==0){
				execvp(comand[0], comand);
				perror("Comando inválido.");
				exit(-1);
			}
		}
	}
	return n;
}




void connect(Nodo* nodos, int id, char** ids){
	int i=0;
	Nodo nodo = nodos[id];
	while(ids[i] != NULL){
		if(nodo -> sizeW%10 == 0 && nodo -> sizeW != 0)
			nodo -> writer = (int*) realloc(nodo->writer,sizeof(int)*(nodo -> sizeW + 10));
		Nodo outro = nodos[atoi(ids[i])];
		nodo -> writer[nodo -> sizeW] = outro -> pipe[0];
		nodo -> sizeW++;
		i++;
	}
}



void disconnect(Nodo* nodos, int id, int id2){
	int i;
	Nodo n = nodos[id];
	Nodo n2 = nodos[id2];
	for(i=0; i < n -> sizeW; i++){
		if(n -> writer[i] == n2 -> pipe[0]){
			rem(n -> writer, i, n -> sizeW);
			n -> sizeW--;
		}
	}
}



int main(){
	Nodo* nodos = (Nodo*) malloc(sizeof(struct nodo)*10);
	int c, i=0, size=0;
	char buffer[1024];
	char **cmd = (char**) malloc(sizeof(char[1024])*64);
	while((c = readln(0, buffer, 1024))){
		buffer[c]='\0';
		i=0;
		cmd[i] = strtok(buffer, " \n\0");
		while(cmd[i] != NULL){
			i++;
			cmd[i] = strtok(NULL, " \n\0");
		}
		i=0;
		/*while(cmd[i] != NULL){
			printf("cmd[%d] -> %s\n",i, cmd[i]);
			i++;
		}*/
		if(!strcmp(cmd[0], "node")){
			if(size%10==0 && size != 10)
				nodos = (Nodo*) realloc(nodos,sizeof(struct nodo)*(size+10));
			nodos[atoi(cmd[1])] = initNodo(atoi(cmd[1]), &cmd[2]);
			size++;
		}
		if(!strcmp(cmd[0], "connect")){
			connect(nodos, atoi(cmd[1]), &cmd[2]);
		}
		if(!strcmp(cmd[0], "inject")){
			Nodo nodo = nodos[atoi(cmd[1])];
			i=2;
			while( cmd[i] != NULL ){
				write( nodo-> pipe[1] , cmd[i], strlen(cmd[i])+1);
				i++;
			}
		}
	}
}