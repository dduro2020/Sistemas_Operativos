#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <dirent.h>
#include <sys/wait.h>

enum {
	Pathsize = 1024,
};

int
count(char *command)
{
	int positions = 2;
    char *p;
    p = command;
	while (*p != '\0') {
        if (*p == ' '){
            positions++;
        }
		p++;		
	}
	return positions;
}

void
divide_terms(int positions, char *command, char **ex)
{
	char *string;
	int i;
    ex[0] = strtok_r(command, " ", &string);
	for(i = 1; i < positions - 1; i++){
        ex[i] = strtok_r(string, " ", &string);
        printf(" mira %s\n", ex[i]);
	} 

}

void
extractpath(char *path, char *command, char *newcommand)
{
	strcpy(newcommand, path);
	strcat(newcommand, command);
}

void
run_process(char *path, char *command1){

	int uno = count(command1);
    printf("%d\n", uno);

    char **ex = malloc(uno*sizeof(char));

	char newcommand1[Pathsize];

	extractpath(path, command1, newcommand1);
    printf("%s\n", newcommand1);

	divide_terms(uno, newcommand1, ex);
    
    ex[uno - 1] = NULL;

    for (int j = 0; j < uno - 1; j++){
        printf("%s\n", ex[j]);
    }
    if (execv(ex[0], ex) < 0){
        errx(EXIT_FAILURE, "error: %s not found",ex[0]);
    }
	free(ex);
}

int
main(int argc, char *argv[])
{
	if(argc != 2){
        errx(EXIT_FAILURE, "usage: %s cmd ",argv[0]);
    }
	char *path = "/bin/";
	run_process(path, argv[1]);

	exit(EXIT_SUCCESS);
}