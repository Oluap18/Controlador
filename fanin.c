#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
//fanout ls wc rev -> output do primeiro argumento, ser redirecionado para todos os outros
//fanit ls ps wc -> output dos argumentos são redirecionados para o último argumento

ssize_t readln(int fildes, char *buf, size_t nbyte){
  int i=0;
  int n;
  while(i<nbyte && (n = read(fildes, buf+i, 1))>0 && *(buf+i)!='\n')
    i++;
  if(n==0) return n;
  return ++i;
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

void main(int argc, char* argv[]){
	fanin(argc-1, &argv[1]);
}