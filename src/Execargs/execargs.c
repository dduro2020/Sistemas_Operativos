#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

void
print_error()
{
	printf("usage: execargs secs command [command ...]\n");
	exit(EXIT_FAILURE);
}

void
print_time_exceded()
{
	printf("error: time exceded, time must be between 0-100 ...\n");
	exit(EXIT_FAILURE);
}

void
print_process_error(char *path, char *argument)
{
	fprintf(stderr, "error: %s %s \n", path, argument);
	exit(EXIT_FAILURE);

}

void
search_time_error(char p, int time)
{
	if (p != '\0') {
		print_error();
	}
	if (time > 100 || time < 0) {
		print_time_exceded();
	}
}

void
search_args_error(int num_args)
{
	if (num_args == 1) {
		print_error();
	}
}

void
search_process_error(int fail, char *path)
{
	char *error = "";

	if (fail == -1) {
		fprintf(stderr, "%s: ", path);
		perror(error);
		exit(EXIT_FAILURE);
	}
}

int
extract_real_time(char *time_value)
{
	int real_time;
	char *p;
	int base = 10;

	real_time = strtol(time_value, &p, base);

	search_time_error(*p, real_time);

	return real_time;
}

void
child_creation_error()
{
	char *error = "";

	perror(error);
	exit(EXIT_FAILURE);
}

void
divide_terms(char *path, char **ex)
{
	char *string;

	path = strtok_r(path, " ", &string);

	if (strcmp(string, "") == 0) {
		string = ".";
	}
	ex[0] = path;                   //guarda el comando del proceso
	ex[1] = string;                 //guarda los argumentos del comando, sino los hubiese guardaría un .
	ex[2] = NULL;                   //guardamos null en última posicion
}

void
run_process(int time, char *path)
{

	int ret, wstatus;
	char *ex[3] = { "", "", "" };
	divide_terms(path, ex);

	pid_t pid;

	pid = fork();

	if (pid == 0) {
		ret = execv(path, ex);
		search_process_error(ret, path);

	} else if (pid > 0) {
		wait(&wstatus);
		sleep(time);
		if (WEXITSTATUS(wstatus)) {
			print_process_error(path, ex[1]); //en ex[1] estan guardados los argumentos que se le pasan
		}
	} else {
		child_creation_error();
	}
}

int
main(int argc, char *argv[])
{
	int time, i;

	search_args_error(argc);
	time = extract_real_time(argv[1]);

	for (i = 2; i < argc; i++) {
		run_process(time, argv[i]);
	}
	exit(EXIT_SUCCESS);
}
