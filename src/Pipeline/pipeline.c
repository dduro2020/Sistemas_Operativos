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
	READ = 0,
	WRITE = 1,
};

int
count_char(char *command)
{
	int count = 2;
	char *p;

	p = command;
	while (*p != '\0') {
		if (*p == ' ') {
			count++;
		}
		p++;
	}
	return count;
}

void
divide_terms(int positions, char *command, char **ex)
{
	char *string;
	int i;

	ex[0] = strtok_r(command, " ", &string);
	ex[positions - 1] = NULL;
	for (i = 1; i < positions - 1; i++) {
		ex[i] = strtok_r(string, " ", &string);
	}
}

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
	int pid, status;

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
		if (wait(&status) < 0) {
			err(EXIT_FAILURE, "child error");
		}
	}
}

void
pipes(int *fd1, int *fd2, char **ex)
{
	int pid, status;

	if (pipe(fd2) < 0) {
		err(EXIT_FAILURE, "cannot make a pipe");
	}
	pid = fork();

	if (pid == 0) {
		close(fd2[READ]);

		if (dup2(fd1[READ], 0) < 0) {
			err(EXIT_FAILURE, "dup failed");
		}
		close(fd1[READ]);

		if (dup2(fd2[WRITE], 1) < 0) {
			err(EXIT_FAILURE, "dup failed");
		}
		close(fd2[WRITE]);

		if (execv(ex[0], ex) < 0) {
			err(EXIT_FAILURE, "execv failed");
		}

	} else if (pid == -1) {
		err(EXIT_FAILURE, "cannot fork");
	} else {
		close(fd1[READ]);
		close(fd2[WRITE]);
		if (wait(&status) < 0) {
			err(EXIT_FAILURE, "child error");
		}
	}
}

void
lastpipe(int *fd, char **ex)
{
	int pid, status;

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
		if (wait(&status) < 0) {
			err(EXIT_FAILURE, "child error");
		}
	}
}

int
main(int argc, char *argv[])
{
	if (argc != 4) {
		fprintf(stderr, "usage: %s cmd cmd cmd\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int i;
	char **ex = NULL;
	int positions;
	int fd[2];
	int prev_fd[2];

	// Dividir el primer comando en tokens
	positions = count_char(argv[1]);
	ex = malloc(sizeof(char *) * (positions + 1));
	divide_terms(positions, argv[1], ex);
	ex[0] = find_command_path(ex[0]);
	if (ex[0] == NULL) {
		fprintf(stderr, "error: command not found in /bin, /usr/bin\n");
		free(ex);
		exit(EXIT_FAILURE);
	}
	// Ejecutar el primer comando
	firstpipe(fd, ex);
	free(ex[0]);
	free(ex);
	memcpy(prev_fd, fd, sizeof(fd));

	// Ejecutar los comandos intermedios
	for (i = 2; i < argc - 1; i++) {
		// Dividir el comando en tokens
		positions = count_char(argv[i]);
		ex = malloc(sizeof(char *) * (positions + 1));
		divide_terms(positions, argv[i], ex);
		ex[0] = find_command_path(ex[0]);
		if (ex[0] == NULL) {
			fprintf(stderr,
				"error: command not found in /bin, /usr/bin\n");
			free(ex);
			exit(EXIT_FAILURE);
		}
		// Ejecutar el comando
		pipes(prev_fd, fd, ex);
		free(ex[0]);
		free(ex);
		memcpy(prev_fd, fd, sizeof(fd));
	}

	// Dividir el último comando en tokens
	positions = count_char(argv[argc - 1]);
	ex = malloc(sizeof(char *) * (positions + 1));
	divide_terms(positions, argv[argc - 1], ex);
	ex[0] = find_command_path(ex[0]);
	if (ex[0] == NULL) {
		fprintf(stderr, "error: command not found in /bin, /usr/bin\n");
		free(ex);
		exit(EXIT_FAILURE);
	}
	// Ejecutar el último comando
	lastpipe(prev_fd, ex);
	free(ex[0]);
	free(ex);

	exit(EXIT_SUCCESS);
}
