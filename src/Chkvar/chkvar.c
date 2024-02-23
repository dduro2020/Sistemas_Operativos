#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void
imprimir_usage_error()
{
	printf("usage: ch varname varcontent [varname varcontent] ... ");
}

void
imprimir_errores(int num_error, char *error1, char *error2)
{

	if (num_error == 0) {
		printf("error: ");
	} else {
		printf(", ");
	}
	printf("%s != %s", error1, error2);
}

int
buscar_errores(int num_variables, char **lista)
{
	int error = 0;		//variable para guardar numero de errores

	if ((num_variables < 3) || ((num_variables - 1) % 2 != 0)) {	//si introducimos un numero de argumentos impar o < 2 imprimimos error
		imprimir_usage_error();
		error++;
	} else {
		int i;

		for (i = 1; i < num_variables; i = i + 2) {
			char *entorno = getenv(lista[i]);

			if ((entorno == NULL)
			    || (strcmp(entorno, lista[i + 1]) != 0)) {
				imprimir_errores(error, lista[i], lista[i + 1]);
				error++;
			}
		}
	}

	return error;
}

int
forma_de_salida(int errores)
{
	if (errores != 0) {
		printf("\n");
		return EXIT_FAILURE;
	} else {
		return EXIT_SUCCESS;
	}
}

int
main(int argc, char *argv[])
{
	int salida, errores;

	errores = buscar_errores(argc, argv);
	salida = forma_de_salida(errores);
	exit(salida);

}
