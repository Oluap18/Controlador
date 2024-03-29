#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <fcntl.h> 

struct nodo{
	int pipeAux[2];
	int id;
	int* writer;
	int sizeW;
	char** cmd;
	int pipe[2];
};

int pipM[2];

typedef struct nodo* Nodo;



ssize_t readln(int fildes, char *buf, size_t nbyte){
	int i=0;
	int n;
	while(i<nbyte && (n = read(fildes, buf+i, 1))>0 && *(buf+i)!='\n' && *(buf+i)!='\0'){
		i++;
		buf[i]='\0';
	}
	return i;
}

void rem(int* arr, int p, int size){
	int i;
	for(i=p; i < size; i++)
		arr[i] = arr[i+1];
}

void fanout(){
	int r, i, j, n;
	char buffer[1024];
	char** cmd = (char**) malloc(sizeof(char*)*64);
	while((r=readln(pipM[0], buffer, 1024))){
		char input[1024];
		i=0;
		buffer[r]='\0';
		cmd[i] = strtok(buffer, " \n\0");
		while(cmd[i]!=NULL){
			i++;
			cmd[i] = strtok(NULL, " \n\0");
		}
		while((n=readln(atoi(cmd[0]), input, 1024))){
			input[n]='\0';
			for(j=1; j < i; j++)
				write(atoi(cmd[j]), input, n+1);
		}
	}
}


/**
*	Caso o comando seja window, será guardado os inputs num array de chars, com o
* tamanho do número de linhas a analizar. Aí, irá guardar inputs de operações anteriores.
*/
Nodo initNodo(int id, char** cm){
	Nodo n = (Nodo) malloc(sizeof(struct nodo));
	pipe(n->pipe);
	pipe(n -> pipeAux);
	n -> id = id;
	n -> writer = (int*) malloc(sizeof(int)*10);
	n -> sizeW = 0;
	n -> cmd = (char**) malloc(sizeof(char*)*10);
	n -> cmd = cm;
	char ficheiro[1024];
	char** input;
	int inp_count=0;
	if(fork()==0){
		char buffer[1024];
		char sp[1024];
		char fanOut[1024];
		int r;
		char** comand;
		if(!strcmp("const", n -> cmd[0])){
			comand = (char**) malloc(sizeof(char[1024])*4);
			comand[0] = strdup("./const");
			comand[2] = strdup(n -> cmd[1]);
			comand[3] = NULL;
			if( n -> cmd[2] != NULL)
				strcpy(ficheiro, n -> cmd[2]);
			else
				strcpy(ficheiro, "output.txt");
		}
		else if(!strcmp("filter", n -> cmd[0])){
			comand = (char**) malloc(sizeof(char[1024])*6);
			comand[0] = strdup("./filter");
			comand[2] = strdup(n -> cmd[1]);
			comand[3] = strdup(n -> cmd[2]);
			comand[4] = strdup(n -> cmd[3]);
			comand[5] = NULL;
			if( n -> cmd[4] != NULL)
				strcpy(ficheiro, n -> cmd[4]);
			else
				strcpy(ficheiro, "output.txt");
		}
		else if(!strcmp("spawn", n -> cmd[0])){
			int i = 2;
			comand = (char**) malloc(sizeof(char[1024])*16);
			comand[0] = strdup("./spawn");
			while(n -> cmd[i-1] != NULL ){
				if( !strcmp(n -> cmd[i-1], "[" ))
					break;
				comand[i] = strdup( n -> cmd[i-1]);
				i++;
			}
			comand[i] = NULL;
			if( n -> cmd[i] != NULL)
				strcpy(ficheiro, n -> cmd[i]);
			else
				strcpy(ficheiro, "output.txt");
		}
		else if(!strcmp("window", n -> cmd[0])){
			comand = (char**) malloc(sizeof(char[1024])*atoi(cm[3])+6);
			comand[0] = strdup("./window");
			comand[2] = strdup( n-> cmd[1] );
			comand[3] = strdup( n-> cmd[2] );
			comand[4] = strdup( n-> cmd[3] );
			if( n -> cmd[4] != NULL)
				strcpy(ficheiro, n -> cmd[4]);
			else
				strcpy(ficheiro, "output.txt");
		}
		else{
			int i=0;
			comand = (char**) malloc(sizeof(char[1024])*16);
			while(n -> cmd[i-1] != NULL ){
				comand[i] = strdup( n -> cmd[i]);
				i++;
			}
			comand[i] = NULL;
		}
		close(n->pipe[1]);
		while((r=readln( n->pipe[0], buffer, 1024 ))){
			buffer[r] = '\0';
			fanOut[0] = '\0';
			int i;
			if(buffer[0]!='+' && buffer[0]!='-'){
				inp_count++;
				char aux[10];
				if(!strcmp("window", n -> cmd[0])){
					if(inp_count-2 != atoi(cm[3])){
						sprintf(aux, "%d", inp_count);
						comand[1] = aux;
						for(i = inp_count+3; i > inp_count ; i--){
							comand[i+1] = comand[i];
						}
						comand[inp_count+5] = NULL;
						comand[inp_count+1] = strdup(buffer);
					}
					else{
						comand[2] = comand[3];
						comand[3] = comand[4];
						comand[4] = strdup(buffer);
						inp_count--;
					}
				}
				else comand[1] = strdup(buffer);
				sprintf(sp, "%d", n->pipeAux[0]);
				strcat(fanOut,sp);
				for( i=0 ; i < n->sizeW ; i++){
					strcat(fanOut, " ");
					sprintf(sp, "%d", n->writer[i]);
					strcat(fanOut,sp);
				}
				if(n->sizeW!=0){
					fanOut[strlen(fanOut)+1]='\0';
					write(pipM[1], fanOut, strlen(fanOut)+1);
				}
				if(fork()==0){
					if(!strcmp(n-> cmd[0], "spawn"))
						dup2(n->pipe[0], 0);
					if(n->sizeW!=0){
						close(n->pipeAux[0]);
						dup2(n->pipeAux[1], 1);
						close(n->pipeAux[1]);
					}
					else{
						int fd = open(ficheiro, O_CREAT | O_WRONLY | O_APPEND, 0666);
						sprintf(sp, "%d", id);
						dup2(fd, 1);
						close(fd);
						write(1, "Output do id ", strlen("Output do id "));
						write(1, sp, strlen(sp));
						write(1, ":\n", 2);
					}
					execvp(comand[0], comand);
					perror("Comando inválido.");
					exit(-1);
				}
				wait(NULL);
			}
			else{
				if(buffer[0] == '+'){
					char** cmd = (char**)malloc(sizeof(char[1024])*10);
					i=0;
					cmd[i] = strtok(buffer, " \n\0");
					while(cmd[i] != NULL){
						i++;
						cmd[i] = strtok(NULL, " \n\0");
					}
					i=1;
					while( cmd[i] != NULL){
						n -> writer[n-> sizeW] = atoi(cmd[i]);
						n -> sizeW++;
						i++;
					}
				}
				else{
					char** cmd = (char**)malloc(sizeof(char[1024])*10);
					i=0;
					cmd[i] = strtok(buffer, " \n\0");
					while(cmd[i] != NULL){
						i++;
						cmd[i] = strtok(NULL, " \n\0");
					}
					for(i=0; i < n -> sizeW; i++){
						if(n -> writer[i] == atoi(cmd[1])){
							rem(n -> writer, i, n -> sizeW);
							n -> sizeW--;
						}
					}
				}
			}
		}
		exit(1);
	}
	return n;
}




void connect(Nodo* nodos, int id, char** ids){
	int i=0;
	char buffer[1024];
	char aux[1024];
	Nodo nodo = nodos[id];
	buffer[0] = '+';
	buffer[1] = '\0';
	while(ids[i] != NULL){
		Nodo outro = nodos[atoi(ids[i])];
		sprintf(aux, "%d", outro -> pipe[1]);
		strcat(buffer, " ");
		strcat(buffer, aux);
		i++;
	}
	write(nodo->pipe[1], buffer, strlen(buffer)+1);
	return ;
}




void disconnect(Nodo* nodos, int id, int id2){
	int i;
	char aux[1024];
	char buffer[1024];
	Nodo n = nodos[id];
	Nodo n2 = nodos[id2];
	buffer[0] = '-';
	buffer[1] = '\0';
	sprintf(aux, "%d", n2 -> pipe[1]);
	strcat(buffer, " ");
	strcat(buffer, aux);
	write(n->pipe[1], buffer, strlen(buffer)+1);
	return ;
}



int main(){
	pipe(pipM);
	Nodo* nodos = (Nodo*) malloc(sizeof(struct nodo)*10);
	int c, i=0, size=0;
	char buffer[1024];
	int pid=0;
	char **cmd = (char**) malloc(sizeof(char[1024])*64);
	while(1){
		while((c = readln(0, buffer, 1024))){
			buffer[c]='\0';
			i=0;
			cmd[i] = strtok(buffer, " \n\0");
			while(cmd[i] != NULL){
				i++;
				cmd[i] = strtok(NULL, " \n\0");
			}
			i=0;
			if(!strcmp(cmd[0], "node")){
				if(size%10==0 && size != 10)
					nodos = (Nodo*) realloc(nodos,sizeof(struct nodo)*(size+10));
				nodos[atoi(cmd[1])] = initNodo(atoi(cmd[1]), &cmd[2]);
				size++;
				if(pid!=0)
					write(pipM[1], "\0 ", 1);
				pid=fork();
				if(pid==0){
					fanout();
					exit(1);
				}
			}
			else if(!strcmp(cmd[0], "connect")){
				connect(nodos, atoi(cmd[1]), &cmd[2]);
			}
			else if(!strcmp(cmd[0], "inject")){
				Nodo nodo = nodos[atoi(cmd[1])];
				i=2;
				while( cmd[i] != NULL ){
					write( nodo-> pipe[1] , cmd[i], strlen(cmd[i])+1);
					i++;
				}
			}
			else if(!strcmp(cmd[0], "disconnect")){
				disconnect(nodos, atoi(cmd[1]), atoi(cmd[2]));
			}
		}
		printf("Deseja sair do programa? y or ENTER for yes.\n");
		c = readln(0, buffer, 1024);
		if(buffer[0]=='y' || c==0)
			break;
	}
	for(i=0; i < size; i++){
		close(nodos[i] -> pipe[1]);
		close(nodos[i] -> pipe[0]);
		close(nodos[i] -> pipeAux[1]);
		close(nodos[i] -> pipeAux[0]);
	}
	close(pipM[0]);
	close(pipM[1]);
}