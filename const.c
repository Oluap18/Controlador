#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <fcntl.h> 

/**
*	Primeiro vem o input
* Segundo vem a constante
*/
void main(int argc, char* argv[]){
	char buffer[1024];
	strcat(buffer, argv[1]);
	strcat(buffer, ":");
	strcat(buffer, argv[2]);
	strcat(buffer, "\n");
	write(1, buffer, strlen(buffer));
}