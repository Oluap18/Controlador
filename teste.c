#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <fcntl.h> 

int main(){
	int pd[2], n;
	pipe(pd);
	char buffer[1024];
	if(fork()==0){
		close(pd[1]);
		while((n = read(pd[0], buffer, 1024))){
			printf("Li do pipe\n");
			write(1, buffer, n);
		}
		printf("Sai do primeiro filho\n");
		_exit(1);
	}
	if(fork()==0){
		close(pd[0]);
		while((n = read(0, buffer, 1024))){
			printf("Li do stdin\n");
			write(pd[1], buffer, n);
		}
		printf("Sai do segundo filho\n");
		_exit(1);
	}
	printf("Espero pelo segundo filho\n");
	//espera pelo segundo filho
	wait(NULL);
	printf("O segundo filho acabou\n");
	//fecha o write do pipe, por isso o read do primeiro filho vai acabar
	close(pd[1]);
	printf("Fechei o pipe do primeiro filho\n");
	//espera pelo primeiro filho
	printf("Espero pelo primeiro filho\n");
	wait(NULL);
	printf("O primeiro filho acabou\n");
}