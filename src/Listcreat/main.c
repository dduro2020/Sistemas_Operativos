#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "listcreat.h"

/* Definimos nuestra variable global de forma cte */
Creat all_animals[] = {
	{0, "cat", "grey/white", 600, 9000},
	{1, "dog", "grey", 750, 12000},
	{2, "cat", "black", 620, 10000},
	{3, "dog", "white/black/grey", 1100, 30000},
	{4, "rat", "white", 300, 2000},
	{5, "dog", "black/brown", 1400, 35000},
	{6, "cat", "orange/grey", 800, 7500},
	{7, "horse", "brown", 2000, 300000},
	{8, "snake", "black", 1000, 5000},
	{9, "dog", "light_brown", 1100, 20000}
};

int
main(int argc, char *argv[])
{
	if (argc != 1) {
		fprintf(stderr, "%s usage: [no arguments requiered] \n",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	struct list *lista;
	struct Queue *cola;
	int i;

	/* Creamos una lista vacía */
	lista = create_list();
	/* Añadimos todos los animales a nuestra lista */
	for (i = 0; i < MAX_ANIMALS; i++) {
		list_add_last(lista, all_animals[i]);
	}

	/* Imprimimos la lista */
	print_list(lista);

	/* He planteado el problema como: términos pares: (0, 2...) */
	/* Eliminamos los términos pares */
	/* Esta forma elimina los elementos con un bucle for */
	/*for(i = 0; i < MAX_ANIMALS; i = i + 2) {
	   delete_list(lista, all_animals[i].id);
	   } */

	/* Esta funcion elimina los terminos pares/impares */
	delete_odd_even_list(lista, EVEN);

	/* Imprimimos la lista */
	print_list(lista);

	/* Destruimos la lista */
	destroy_list(lista);

	/* Creamos la cola */
	cola = createQueue();

	/* Guardamos los 10 animaless */
	for (i = 0; i < MAX_ANIMALS; i++) {
		enqueue(cola, all_animals[i]);
	}
	/* Imprimimos la cola */
	printQueue(cola);

	/* Sacamos e imprimimos los 5 primeros terminos */
	printf("Imprimiendo los 5 primeros elementos de la cola...\n\n");
	for (i = 0; i < MAX_ANIMALS / 2; i++) {
		print_creat(dequeue(cola));
	}
	/* Destruimos la cola */
	destroyQueue(cola);
}
