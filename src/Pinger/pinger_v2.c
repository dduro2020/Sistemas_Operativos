#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* Imprimir un mensaje de error y salir */
void error(char *message)
{
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

/* Ejecución de ping para una máquina dada */
void ping(char *machine, char *timeout)
{
    char *args[] = {"/bin/ping", "-q", "-c", "2", "-W", timeout, machine, NULL};
    if (execv(args[0], args) == -1) {
        fprintf(stderr, "ping failed for %s\n", machine);
        exit(EXIT_FAILURE);
    }
}

/* Comprueba si una máquina está en línea */
int check_machine(char *machine, char *timeout)
{
    int status;
    pid_t pid = fork();
    if (pid == -1) {
        error("fork failed");
    } else if (pid == 0) {
        ping(machine, timeout);
    } else {
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            return 1; /* La máquina está en línea */
        }
    }
    return 0; /* La máquina no está en línea */
}

int main(int argc, char *argv[])
{
    /* Comprobar args */
    if (argc < 3) {
        fprintf(stderr, "usage: %s <timeout> <host_id> [host_id2...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int timeout = atoi(argv[1]);
    if (timeout <= 0) {
        error("timeout must be a positive integer");
    }

    int i;
    for (i = 2; i < argc; i++) {
        if (!check_machine(argv[i], argv[1])) {
            fprintf(stderr, "no response from %s\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    /* Si se ha llegado aqui, el programa sale con exito */
    return EXIT_SUCCESS;
}
