#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>
#include <sys/wait.h>

enum {
	READ = 0,
	WRITE = 1,
};

char *
find_command_path(char *command)
{
	// Buscar el comando en /bin
	char *path = "/bin/";
	char *command_path = malloc(strlen(path) + strlen(command) + 1);

	strcpy(command_path, path);
	strcat(command_path, command);

	if (access(command_path, X_OK) == 0) {
		return command_path;
	}
	// Buscar el comando en /usr/bin
	path = "/usr/bin/";
	free(command_path);	// Liberar la memoria asignada a command_path
	command_path = malloc(strlen(path) + strlen(command) + 1);
	strcpy(command_path, path);
	strcat(command_path, command);

	if (access(command_path, X_OK) == 0) {
		return command_path;
	}
	// Comando no encontrado
	free(command_path);
	return NULL;
}

void
firstpipe(int *fd, char **ex)
{
	int pid;

	if (pipe(fd) < 0) {
		err(EXIT_FAILURE, "cannot make a pipe");
	}

	pid = fork();

	if (pid == 0) {
		close(fd[READ]);

		if (dup2(fd[WRITE], 1) < 0) {
			err(EXIT_FAILURE, "dup failed");
		}
		close(fd[WRITE]);

		if (execv(ex[0], ex) < 0) {
			err(EXIT_FAILURE, "execv failed");
		}
	} else if (pid == -1) {
		err(EXIT_FAILURE, "cannot fork");
	} else {
		close(fd[WRITE]);
	}
}

void
lastpipe(int *fd, char **ex)
{
	int pid;

	pid = fork();

	if (pid == 0) {
		if (dup2(fd[READ], 0) < 0) {
			err(EXIT_FAILURE, "dup failed");
		}
		close(fd[READ]);
		if (execv(ex[0], ex) < 0) {
			err(EXIT_FAILURE, "execv failed");
		}
	} else if (pid == -1) {
		err(EXIT_FAILURE, "cannot fork");
	} else {
		close(fd[READ]);
	}
}

int
main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "Uso: %s regex command [args...]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int command_argc = argc - 2;
	int fd[2], status, wpid;
	char **command_args = malloc(sizeof(char *) * (command_argc + 1));

	char *command_path = find_command_path(argv[2]);
	char *egrep[] = { "/usr/bin/egrep", argv[1], NULL };
	if (command_path == NULL) {
		fprintf(stderr, "Comando no encontrado: %s\n", argv[2]);
		exit(EXIT_FAILURE);
	}

	command_args[0] = command_path;
	if (argc > 3) {
		for (int i = 3; i < argc; i++) {
			command_args[i - 2] = argv[i];
		}
	}
	command_args[command_argc] = NULL;

	firstpipe(fd, command_args);
	lastpipe(fd, egrep);

	/* Esperar a que todos los procesos hijos terminen */
	while ((wpid = wait(&status)) > 0) {
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
			/* Proceso hijo terminado con error */
			free(command_path);
			free(command_args);
			exit(EXIT_FAILURE);
		}
	}
	free(command_path);
	free(command_args);

	exit(EXIT_SUCCESS);
}
