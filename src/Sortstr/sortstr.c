#include <stdio.h>
#include <string.h>

int
resize(int argc, char *argv[])
{
	char VOCALES[10] = { 'a', 'e', 'i', 'o', 'u', 'A', 'E', 'I', 'O', 'U' };
	char *p;
	int contador = 0;
	int sobrantes = 0;

	for (int elem = 1; elem < argc; elem++) {
		contador = 0;
		for (int i = 0; i < sizeof(VOCALES); i++) {
			p = argv[elem];
			while (*p != '\0') {
				if (VOCALES[i] == *p) {
					contador++;
				}
				p++;
			}
		}
		if (contador == 0) {
			sobrantes++;
			argv[elem] = "";
		}
	}
	return sobrantes;
}

void
sort(int argc, char *argv[])
{
	char copia;

	for (int i = 1; i < argc - 1; i++) {
		for (int j = i + 1; j < argc; j++) {
			if (strcmp(argv[i], argv[j]) > 0) {
				copia = argv[i];
				argv[i] = argv[j];
				argv[j] = copia;
			}
		}
	}
}

int
main(int argc, char *argv[])
{
	int sin_vocales = 0;

	sin_vocales = resize(argc, argv);
	sort(argc, argv);

	for (int num = 1 + sin_vocales; num < argc; num++) {
		printf("%s \n", argv[num]);
	}
	return 0;

}
