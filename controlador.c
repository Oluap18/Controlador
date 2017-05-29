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
	int* reader;
	int sizeW;
	int sizeR;
	char** cmd;
	int[2] pipe;
};

typedef struct nodo* Nodo;



ssize_t readln(int fildes, char *buf, size_t nbyte){
	int i=0;
	int n;
	while(i<nbyte && (n = read(fildes, buf+i, 1))>0 && *(buf+i)!='\n')
		i++;
	if(n<0) return n;
	return i;
}



Nodo initNodo(int id, char** cm){
	Nodo n = (Nodo) malloc(sizeof(struct nodo));
	n -> id = id;
	n -> writer = (int*) malloc(sizeof(int)*10);
	n -> sizeW = 0;
	n -> reader = (int*) malloc(sizeof(int)*10);
	n -> sizeR = 0;
	n -> cmd = (char**) malloc(sizeof(char*)*10);
	n -> cmd = cm;
	pipe(n->pipe);
	return n;
}



void fanin(int argc, char* argv[]){
	int pipp[2], pipf[2], pipw[argc-1], pipr[argc-1];
	pipe(pipp);
	int i;
	for(i=0;i<argc-1;i++){
		pipe(pipf);
		pipr[i] = pipf[0];
		pipw[i] = pipf[1];
	}
	for(i=0;i<argc-1;i++){
		if(fork()==0){
			dup2(pipw[i], 1);
			close(pipw[i]);
			execlp(argv[i], argv[i], NULL);
			perror("Comando inválido.");
			_exit(-1);
		}
		wait(NULL);
		close(pipw[i]);
		if(fork()==0){
			dup2(pipr[i], 0);
			close(pipr[i]);
			execlp(argv[argc-1], argv[argc-1], NULL);
			perror("Comando inválido.");
			_exit(-1);
		}
		close(pipr[i]);
		wait(NULL);
	}
}



void fanout(int argc, char* argv[]){
	int pipp[2], pipf[2], pipr[argc-1], pipw[argc-1];
	pipe(pipp);
	char buffer[1024];
	int c,n;
	if(fork()==0){
		close(pipp[0]);
		dup2(pipp[1],1);
		close(pipp[1]);
		execlp(argv[0], argv[0], NULL);
		perror("Comando inválido.");
		exit(-1);
	}
	close(pipp[1]);
	for(int i=0;i<argc-1;i++){
		pipe(pipf);
		pipr[i] = pipf[0];
		pipw[i] = pipf[1];
	}
	while((c =read(pipp[0], buffer, 1024))){
		for(int i=0;i<argc-1;i++)
			write(pipw[i], buffer, c);
		for(int n=0;n<argc-1;n++){
			close(pipw[n]);
			if(fork()==0){
				dup2(pipr[n], 0);
				close(pipr[n]);
				execlp(argv[n+1], argv[n+1], NULL);
				perror("Comando inválido.");
				_exit(-1);
			}
			close(pipr[n]);
		}
	}
	close(pipp[0]);
	for(int i=0; i< argc; i++){
		wait(NULL);
	}
}




void connect(Nodo* nodos, int id, char** ids){
	int i=0;
	Nodo nodo = nodos[id];
	int pipe[2] = nodo -> pipe;
	while(ids[i] != NULL){
		if(nodo -> sizeW%10 == 0 && nodo -> sizeW != 0)
			nodo -> writer = (int*) realloc(nodo->writer,sizeof(int)*(nodo -> sizeW + 10));
		Nodo outro = nodos[atoi(ids[i])];
		nodo -> writer[nodo -> sizeW] = outro -> pipe[0];
		nodo -> sizeW++;
		i++;
	}
	for(i=0; ids[i] != NULL; i++){
		nodo = nodos[atoi(ids[i])];
		nodo -> reader[nodo -> sizeR] = pipe[1];
		printf("%d -> %d\n", atoi(ids[i]), nodo -> reader[nodo -> sizeR]);
		nodo -> sizeR++;
	}
}



void disconnect(Nodo* nodos, int id, int id2){
	int i;
	Nodo n = nodos[id];
	Nodo n2 = nodos[id2];
	for(i=0; i < n -> sizeW; i++){
		if(n -> writer[i] == n2 -> pipe[0]){
			remove(n -> writer[i], i, n -> sizeW);
			n -> sizeW--;
		}
	}
	for(i=0; i < n2 -> sizeR; i++){
		if(n2 -> reader[i] == n -> pipe[1]){
			remove(n2 -> reader[i], i, n2 -> sizeR);
			n2 -> sizeR--;
		}
	}
}



void remove(int* arr, int p, int size){
	int i;
	for(i=p; i < size; i++)
		arr[i] = arr[i+1];
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
			nodos[atoi(cmd[1])] = initNodo(atoi(cmd[1]), &cmd[2])
			i=0;
			while(nodos[atoi(cmd[1])] -> cmd[i]!=NULL){
				printf("%s\n", nodos[atoi(cmd[1])] -> cmd[i]);
				i++;
			}
			size++;
		}
		if(!strcmp(cmd[0], "connect")){
			connect(nodos, atoi(cmd[1]), &cmd[2]);
		}
	}
}