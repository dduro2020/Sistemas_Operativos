
typedef enum {
	MAX_LEN = 100
} max_lenght;

typedef enum {
	MAX_ANIMALS = 10
} max_animals;

typedef enum {
	EVEN = 0,
	ODD
} num_type;

typedef struct {
	long long id;
	char specie[MAX_LEN];
	char colour[MAX_LEN];
	int height;		/*mm */
	int weight;		/*g */
} Creat;

/* Estructuras para lista */
struct node {
	Creat animal;
	struct node *next;
	struct node *prev;
};

struct list {
	struct node *head;
	struct node *tail;
	int count;
};

/* Estructuras para cola */
struct Node {
	Creat animal;
	struct Node *next;
};

struct Queue {
	struct Node *front;
	struct Node *rear;
};

/* Definimos array global */
extern Creat all_animals[];

/* Funcion para imprimir cada struct */
void print_creat();

/*List: */
struct list *create_list();
int list_is_empty(struct list *l);
void print_list(struct list *l);
void list_add_first(struct list *l, Creat new_animal);
void list_add_last(struct list *l, Creat new_animal);
struct node *find_in_list(struct list *l, int id);
void delete_list(struct list *l, int id);
struct node *first_in_list(struct list *l);
struct node *last_in_list(struct list *l);
int list_size(struct list *l);
void destroy_list(struct list *l);

/* Funcion no requerida por enunciado, utilizada para eliminar miembros en posiciones pares/impares */
void delete_odd_even_list(struct list *l, num_type type);

/*Queue: */
void enqueue(struct Queue *q, Creat new_animal);
Creat dequeue(struct Queue *q);
void printQueue(struct Queue *q);
void destroyQueue(struct Queue *q);
struct Queue *createQueue();
