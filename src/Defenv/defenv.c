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

/* Comprobar si se ha pasado un path como argumento */
void
check_4_path(int n, char **args)
{
	int i;

	for (i = 1; i < n; i++) {
		if (args[i][0] == '/' || args[i][0] == '.') {
			return;
		}
		if (strchr(args[i], '=') == NULL) {
			error("error: incorrect enviroment variable");
		}
	}
	error("error: path needed");
}

/* Ejecución de programa */
void
run_program(char **args, char **env_vars, int env_count)
{
	pid_t pid = fork();
	int i;

	if (pid == -1) {
		error("error: cannot create child");
	} else if (pid == 0) {	/* Proceso hijo */
		/* Variables de entorno */
		for (i = 0; i < env_count; i++) {
			char *env_var = env_vars[i];

			/* Buscamos la posición del '=' en cada variable */
			char *eq_pos = strchr(env_var, '=');

			if (eq_pos == NULL) {
				error("error: wrong enviroment variable");
			}
			/* Dividimos la cadena en dos en la posición del '=' */
			*eq_pos = '\0';
			char *name = env_var;

			/* Situamos el puntero una pos más del igual (valor de la variable) */
			char *value = eq_pos + 1;

			/* Definimos la variable de entorno */
			setenv(name, value, 1);
		}

		/* Ejecutamos el programa */
		execv(args[0], args);

		/* Si execv tiene un error */
		fprintf(stderr, "error: cannot execute: %s\n", args[0]);

		/* Liberar memoria antes de salir */
		for (i = 0; i < env_count; i++) {
			free(env_vars[i]);
		}
		free(env_vars);
		free(args);

		exit(EXIT_FAILURE);
	} else {		/* Proceso padre */
		int status;

		/* Esperamos a que termine el proceso hijo */
		if (waitpid(pid, &status, 0) == -1) {
			error("error: waitpid failled");
		}
		/* Si el proceso hijo terminó normalmente */
		if (WIFEXITED(status)) {
			/* Liberar memoria antes de salir */
			for (i = 0; i < env_count; i++) {
				free(env_vars[i]);
			}
			free(env_vars);
			free(args);

			/* Salimos con el mismo status que el hijo */
			exit(WEXITSTATUS(status));
			/* Si el proceso hijo terminó con un error */
		} else {
			fprintf(stderr, "defenv: program failed: %s\n",
				args[0]);
			/* Liberar memoria antes de salir */
			for (i = 0; i < env_count; i++) {
				free(env_vars[i]);
			}
			free(env_vars);
			free(args);

			exit(EXIT_FAILURE);
		}
	}
}

int
main(int argc, char *argv[])
{
	/* Comprobar args */
	if (argc < 2) {
		fprintf(stderr,
			"usage: %s [optional args: <example=example>...] <executable_path> [executable_args...]\n",
			argv[0]);
		return EXIT_FAILURE;
	}
	int i, j;

	check_4_path(argc, argv);
	char **env_vars = malloc(sizeof(char *) * (argc - 2));

	if (env_vars == NULL) {
		error("error: could not allocate memory");
	}

	int env_count = 0;

	/* Recorremos los argumentos para rellenar los datos */
	for (i = 1; i < argc; i++) {
		char *arg = argv[i];

		/* Buscar la posición del primer '=' en el argumento */
		char *eq_pos = strchr(arg, '=');

		/* Si se encontró un '=', es una variable de entorno, la guardamos */
		if (eq_pos != NULL) {
			env_vars[env_count] = malloc(strlen(arg) + 1);

			if (env_vars[env_count] == NULL) {
				error("error: could not allocate memory");
			}

			strcpy(env_vars[env_count], arg);
			env_count++;
			/* Si no se encontró un '=', es el path */
		} else {
			char **args = malloc(sizeof(char *) * (argc - i + 2));

			if (args == NULL) {
				error("error: could not allocate memory");
			}
			/* Guardamos los argumentos para utilizar execv */
			for (j = 0; j < argc - i + 1; j++) {
				args[j] = argv[i + j];
			}

			args[argc - i + 1] = NULL;
			run_program(args, env_vars, env_count);
			free(args);
			break;
		}
	}

	/* Liberar la memoria del array de variables de entorno */
	for (i = 0; i < env_count; i++) {
		free(env_vars[i]);
	}

	free(env_vars);
	/* Si se ha llegado aqui, el programa sale con exito */
	exit(EXIT_SUCCESS);
}
