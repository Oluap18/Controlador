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

void fanout(int argc, char* argv[]){
	int pipp[2], pipf[2], pippr[argc-1], pippw[argc-1];
	pipe(pipp);
	char buffer[1024];
	int c,n;
	if(fork()==0){
		close(pipp[0]);
		dup2(pipp[1],1);
		close(pipp[1]);
		execlp(argv[0], argv[0], NULL);
		exit(-1);
	}
	for(int i=1;i<argc;i++){
		pipe(pipf);
		pippr[i-1] = pipf[0];
		pippw[i-1] = pipf[1];
	}
	while((c =read(pipp[0], buffer, 1024))){
		for(int i=0;i<argc-1;i++){
			write(pippw[i], buffer, c);
			close(pippw[i]);
		}
		for(int n=0;n<argc-1;n++){
			if(fork()==0){
				dup2(pippr[n], 0);
				close(pippr[n]);
				execlp(argv[n+1], argv[n+1], NULL);
				perror("Comando inválido.");
				_exit(-1);
			}
		}
	}
	for(int i=0;i<argc-1;i++){
		close(pippr[i]);
		close(pippw[i]);
	}
	close(pipp[0]);
	for(int i=0;i<argc;i++)
		wait(NULL);
}

void main(int argc, char* argv[]){
	fanout(argc-1, &argv[1]);
}