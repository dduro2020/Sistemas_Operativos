#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

enum {
	Bufsize = 8 * 1024,
};

/* Imprimir un mensaje de error y salir */
void
error(char *message)
{
	fprintf(stderr, "%s\n", message);
	exit(EXIT_FAILURE);
}

/* Bytes totales del archivo */
int
extract_file_bytes(char *file)
{
	int file_bytes;
	struct stat sb;

	if (stat(file, &sb) != 0) {
		error
		    ("error: cannot extract file bytes or file does not exist");
	}

	file_bytes = sb.st_size;

	return file_bytes;
}

/* Funcion lectura / escritura */
void
copy_bytes(char *file1, char *file2, int bytes)
{
	int f1, f2, nr, count;
	int aux = 0;
	char buf[Bufsize];

	/* Si se nos ha pasado - como fichero de lectura, leemos de stdin */
	/* Comprobamos si se ha pasado exactamente '-' */
	if (file1[0] == '-' && file1[1] == '\0') {
		f1 = STDIN_FILENO;
		if (bytes == 0) {
			bytes = Bufsize;
		} 
	} else {
		f1 = open(file1, O_RDONLY);
		if (f1 < 0) {
			error("error: cannot open input file");
		}
		if (bytes == 0) {
			bytes = extract_file_bytes(file1);
			if (bytes <= 0) {
				error
				    ("error: cannot extract file bytes or file is empty");
			}
		} else {
			/* Si nos piden leer mas bytes que los hay disponibles; error */
			aux = extract_file_bytes(file1);
			if (aux <= 0) {
				error
				    ("error: cannot extract file bytes or file is empty");
			}
			if (bytes > aux) {
				error
				    ("error: bytes passed are higher than file bytes");
			}
		}
	}
	/* Si se nos ha pasado - como fichero de escritura, escribimos en stdout */
	/* Comprobamos si se ha pasado exactamente '-' */
	if (file2[0] == '-' && file2[1] == '\0') {
		f2 = STDOUT_FILENO;
	} else {
		f2 = open(file2, O_CREAT | O_WRONLY | O_TRUNC,
			  S_IWUSR | S_IRUSR);
		if (f2 < 0) {
			error("error: cannot create or open output file");
		}
	}

	count = 0;
	/* Leemos del descriptor de entrada */
	while ((nr = read(f1, buf, Bufsize)) > 0) {
		if (count + nr > bytes && bytes > 0) {
			nr = bytes - count;
		}
		/* Escribimos en el descriptor de salida */
		if (write(f2, buf, nr) != nr) {
			error("error: cannot write to output file");
		}
		count += nr;
		/* Terminamos cuando hayamos leido todos los bytes del archivo o los pedidos */
		if (count == bytes) {
			break;
		}
	}
	/* Cerramos descriptores */
	if (f1 != STDIN_FILENO) {
		close(f1);
	}
	if (f2 != STDOUT_FILENO) {
		close(f2);
	}
}

int
main(int argc, char *argv[])
{
	if (argc < 3 || argc > 4) {
		fprintf(stderr,
			"usage: %s <input file> <output file> [<bytes>]\n",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	char *file1 = argv[1];
	char *file2 = argv[2];
	int bytes = 0;

	/* Comprobamos si nos han pasado bytes como argumento */
	if (argc == 4) {
		bytes = atoi(argv[3]);
		if (bytes <= 0) {
			error("number of bytes must be a positive integer");
		}
	}
	/* Comprobamos si se puede acceder al fichero a leer */
	if (file1[0] != '-' && access(file1, F_OK) != 0) {
		fprintf(stderr, "error: cannot access input file %s\n", file1);
		exit(EXIT_FAILURE);
		/* Es posible que el archivo comience por '-', se comprueba */
	} else if (file1[0] == '-' && file1[1] != '\0'
		   && access(file1, F_OK) != 0) {
		fprintf(stderr, "error: cannot access input file %s\n", file1);
		exit(EXIT_FAILURE);
	}
	copy_bytes(file1, file2, bytes);

	/* Si se ha llegado aqui, el programa sale con exito */
	exit(EXIT_SUCCESS);
}
