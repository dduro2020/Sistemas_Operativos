#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* Imprimir un mensaje de error y salir */
void
error(char *message)
{
	fprintf(stderr, "%s\n", message);
	exit(EXIT_FAILURE);
}

/* Ejecución de ping para una máquina dada */
void
ping(char *machine, char *timeout)
{
	char *args[] =
	    { "/bin/ping", "-q", "-c", "2", "-W", timeout, machine, NULL };
	if (execv(args[0], args) == -1) {
		fprintf(stderr, "ping failed for %s\n", machine);
		exit(EXIT_FAILURE);
	}
}

int
main(int argc, char *argv[])
{
	/* Comprobar args */
	if (argc < 3) {
		fprintf(stderr, "usage: %s timeout <host_id> [host_id2...]\n",
			argv[0]);
		return EXIT_FAILURE;
	}

	int i;
	pid_t pid, wpid;

	/* Array para almacenar los PIDs de los procesos hijos */
	pid_t child_pids[argc - 2];
	int status;
	int timeout = atoi(argv[1]);

	if (timeout <= 0) {
		error("timeout must be a positive integer");
	}

	/* Crear procesos hijos para cada host */
	for (i = 2; i < argc; i++) {
		pid = fork();
		if (pid == 0) {
			/* Proceso hijo */
			ping(argv[i], argv[1]);
		} else if (pid < 0) {
			/* Error al crear el proceso hijo */
			error("Error al crear proceso hijo");
		} else {
			/* Almacenar el PID del proceso hijo */
			child_pids[i - 2] = pid;
		}
	}

	/* Esperar a que todos los procesos hijos terminen */
	while ((wpid = wait(&status)) > 0) {
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
			/* Proceso hijo terminado con error */
			for (i = 0; i < argc - 2; i++) {
				if (child_pids[i] == wpid) {
					/* Imprimir el mensaje de error indicando el host que falló */
					fprintf(stderr, "\nno existe: %s\n",
						argv[i + 2]);
				}
				kill(child_pids[i], SIGTERM);
			} 
			exit(EXIT_FAILURE);
		}
	}

	/* Si se ha llegado aqui, el programa sale con exito */
	exit(EXIT_SUCCESS);
}
