#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include  "listcreat.h"

/* Imprimir la estructura creat */
void
print_creat(Creat animal)
{
	printf("Specie:\t%s\n", animal.specie);
	printf("ID:\t%lld\n", animal.id);
	printf("Colour:\t%s\n", animal.colour);
	printf("Height:\t%d\n", animal.height);
	printf("Weight:\t%d\n\n", animal.weight);
}

/* Crea una nueva lista vacía */
struct list *
create_list()
{
	struct list *new_list = (struct list *)malloc(sizeof(struct list));

	new_list->head = NULL;
	new_list->tail = NULL;
	new_list->count = 0;
	return new_list;
}

/* Devuelve 1 si la lista está vacía, 0 en caso contrario */
int
list_is_empty(struct list *l)
{
	return l->count == 0;
}

/* Imprime los elementos de la lista */
void
print_list(struct list *l)
{
	struct node *cur = l->head;

	printf("Imprimiendo lista con %d elementos...\n\n", l->count);
	while (cur != NULL) {
		print_creat(cur->animal);
		cur = cur->next;
	}
}

/* Imprime los elementos pares o impares de la lista */
void
delete_odd_even_list(struct list *l, num_type type)
{
	int n = 2;
	struct node *cur = l->head;

	while (cur != NULL) {
		struct node *next = cur->next;

		if ((n % 2) == type) {
			if (cur->prev != NULL) {
				cur->prev->next = cur->next;
			}
			if (cur->next != NULL) {
				cur->next->prev = cur->prev;
			}
			if (cur == l->head) {
				l->head = cur->next;
			}
			if (cur == l->tail) {
				l->tail = cur->prev;
			}
			free(cur);
			l->count--;
		}
		n++;
		cur = next;
	}
}

/* Agrega un elemento al principio de la lista */
void
list_add_first(struct list *l, Creat new_animal)
{
	struct node *new_node = (struct node *)malloc(sizeof(struct node));

	memset(&new_node->animal, '\0', sizeof(new_node->animal));

	strncpy(new_node->animal.colour, new_animal.colour,
		strlen(new_animal.colour));
	strncpy(new_node->animal.specie, new_animal.specie,
		strlen(new_animal.specie));
	new_node->animal.id = new_animal.id;
	new_node->animal.height = new_animal.height;
	new_node->animal.weight = new_animal.weight;

	new_node->prev = NULL;
	new_node->next = l->head;
	if (l->head != NULL) {
		l->head->prev = new_node;
	}
	l->head = new_node;
	if (l->tail == NULL) {
		l->tail = new_node;
	}
	l->count++;
}

/* Agrega un elemento al final de la lista */
void
list_add_last(struct list *l, Creat new_animal)
{
	struct node *new_node = (struct node *)malloc(sizeof(struct node));

	memset(&new_node->animal, '\0', sizeof(new_node->animal));

	strncpy(new_node->animal.colour, new_animal.colour,
		strlen(new_animal.colour));
	strncpy(new_node->animal.specie, new_animal.specie,
		strlen(new_animal.specie));
	new_node->animal.id = new_animal.id;
	new_node->animal.height = new_animal.height;
	new_node->animal.weight = new_animal.weight;

	new_node->next = NULL;
	new_node->prev = l->tail;
	if (l->tail != NULL) {
		l->tail->next = new_node;
	}
	l->tail = new_node;
	if (l->head == NULL) {
		l->head = new_node;
	}
	l->count++;
}

/* Recorremos la lista para comprobar la existencia de ids duplicados */
/* Funcion privada */
void
check_dup(struct list *l, int id)
{
	struct node *cur = l->head;
	int n = 0;

	while (cur != NULL) {
		if (cur->animal.id == id) {
			n = n + 1;
		}
		cur = cur->next;
	}
	if (n > 1) {
		destroy_list(l);
		fprintf(stderr, "error: existen varios creat con mismo id\n");
		exit(EXIT_FAILURE);
	}
}

/* Busca un elemento con el identificador especificado y devuelve su puntero */
struct node *
find_in_list(struct list *l, int id)
{
	check_dup(l, id);
	struct node *cur = l->head;

	while (cur != NULL) {
		if (cur->animal.id == id) {
			return cur;
		}
		cur = cur->next;
	}
	return NULL;
}

/* Borra un elemento con el identificador especificado */
void
delete_list(struct list *l, int id)
{
	check_dup(l, id);
	struct node *cur = l->head;
	struct node *to_delete = NULL;

	while (cur != NULL) {
		if (cur->animal.id == id) {
			to_delete = cur;
			break;
		}
		cur = cur->next;
	}

	if (to_delete == NULL) {
		printf("Sin resultados para id: %d\n", id);
		return;
	}
	if (to_delete->prev != NULL) {
		to_delete->prev->next = to_delete->next;
	}
	if (to_delete->next != NULL) {
		to_delete->next->prev = to_delete->prev;
	}
	if (to_delete == l->head) {
		l->head = to_delete->next;
	}
	if (to_delete == l->tail) {
		l->tail = to_delete->prev;
	}
	l->count--;
	free(to_delete);
}

/* Devuelve el primer elemento de la lista */
struct node *
first_in_list(struct list *l)
{
	return l->head;
}

/* Devuelve el último elemento de la lista */
struct node *
last_in_list(struct list *l)
{
	return l->tail;
}

/* Devuelve el número de elementos en la lista */
int
list_size(struct list *l)
{
	return l->count;
}

/* Elimina la lista */
void
destroy_list(struct list *l)
{
	struct node *cur = l->head;

	while (cur != NULL) {
		struct node *next = cur->next;

		free(cur);
		cur = next;
	}
	free(l);
}

/* Añade un elemento al final de la cola */
void
enqueue(struct Queue *q, Creat new_animal)
{
	struct Node *temp = (struct Node *)malloc(sizeof(struct Node));

	memset(&temp->animal, '\0', sizeof(temp->animal));

	strncpy(temp->animal.colour, new_animal.colour,
		strlen(new_animal.colour));
	strncpy(temp->animal.specie, new_animal.specie,
		strlen(new_animal.specie));
	temp->animal.id = new_animal.id;
	temp->animal.height = new_animal.height;
	temp->animal.weight = new_animal.weight;

	temp->next = NULL;
	if (q->rear == NULL) {
		q->front = q->rear = temp;
		return;
	}
	q->rear->next = temp;
	q->rear = temp;
}

/* Saca el primer elemento de la cola */
Creat
dequeue(struct Queue *q)
{
	Creat ret_animal;

	memset(&ret_animal, '\0', sizeof(ret_animal));
	if (q->front == NULL) {
		printf("Cola vacia\n");
		ret_animal.id = -1;
		return ret_animal;
	}
	struct Node *temp = q->front;

	strncpy(ret_animal.colour, temp->animal.colour,
		strlen(temp->animal.colour));
	strncpy(ret_animal.specie, temp->animal.specie,
		strlen(temp->animal.specie));
	ret_animal.id = temp->animal.id;
	ret_animal.height = temp->animal.height;
	ret_animal.weight = temp->animal.weight;

	q->front = q->front->next;
	if (q->front == NULL)
		q->rear = NULL;
	free(temp);
	return ret_animal;
}

/* Imprime la cola */
void
printQueue(struct Queue *q)
{
	if (q->front == NULL) {
		printf("Cola vacia\n");
		return;
	}
	struct Node *temp = q->front;

	printf("Imprimiendo cola...\n\n");
	while (temp != NULL) {
		print_creat(temp->animal);
		temp = temp->next;
	}
}

/* Elimina la cola */
void
destroyQueue(struct Queue *q)
{
	while (q->front != NULL) {
		struct Node *temp = q->front;

		q->front = q->front->next;
		free(temp);
	}
	q->rear = NULL;
	free(q);
}

/* Crea una nueva cola */
struct Queue *
createQueue()
{
	struct Queue *q = (struct Queue *)malloc(sizeof(struct Queue));

	q->front = q->rear = NULL;
	return q;
}
