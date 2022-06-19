#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

struct Node
{ // declaracion de un nodo de lista enlazada
	int data;
	struct Node *next;
};

void sortedInsert(struct Node **head_ref, struct Node *new_node)
{
	struct Node *current;
	if (*head_ref == NULL // caso especial de que sea cabecera
		|| (*head_ref)->data >= new_node->data)
	{
		new_node->next = *head_ref;
		*head_ref = new_node;
	}
	else
	{
		current = *head_ref;
		while (current->next != NULL && current->next->data < new_node->data)
		{
			current = current->next;
		}
		new_node->next = current->next;
		current->next = new_node;
	}
}

struct Node *newNode(int new_data)
{
	struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
	new_node->data = new_data; // inserta la info en el data del nodo
	new_node->next = NULL;

	return new_node;
}

void printList(struct Node *head)
{
	struct Node *ptr = head;
	while (ptr)
	{
		printf("%d —> ", ptr->data);
		ptr = ptr->next;
	}
	printf("Fin\n");
}

// Insertar los nodos al incio de la lista
void push(struct Node **head, int data)
{
	struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
	newNode->data = data;
	newNode->next = *head;
	*head = newNode;
}

// Toma dos listas ordenadas en orden creciente y fusiona sus nodos
// para hacer una gran lista ordenada, que se devuelve
struct Node *sortedMerge(struct Node *a, struct Node *b)
{
	if (a == NULL)
	{
		return b;
	}
	else if (b == NULL)
	{
		return a;
	}
	struct Node *result = NULL;

	// escoge entre `a` o `b`, y repitesi (a->datos <= b->datos)
	if (a->data <= b->data)
	{
		result = a;
		result->next = sortedMerge(a->next, b);
	}
	else
	{
		result = b;
		result->next = sortedMerge(a, b->next);
	}
	return result;
}

/*
	Divide los nodos de la lista dada en mitades anteriores y posteriores
	y devuelve las dos listas utilizando los parámetros de referencia.
	Si la longitud es impar, el nodo extra debe ir en la lista frontal.
	Utiliza la estrategia de punteros rápidos/lentos
*/
void middleSplit(struct Node *source, struct Node **frontRef,
				 struct Node **backRef)
{
	if (source == NULL || source->next == NULL)
	{ // si la longitud es inferior a 2
		*frontRef = source;
		*backRef = NULL;
		return;
	}

	struct Node *slow = source;
	struct Node *fast = source->next;
	while (fast != NULL)
	{
		fast = fast->next;
		if (fast != NULL)
		{
			slow = slow->next;
			fast = fast->next;
		}
	}
	*frontRef = source;
	*backRef = slow->next;
	slow->next = NULL;
}

// Ordenar una lista enlazada dada utilizando el algoritmo de ordenación merge
void mergeSort(struct Node **head)
{
	if (*head == NULL || (*head)->next == NULL)
	{ // longitud 0 o 1
		return;
	}

	struct Node *a;
	struct Node *b;

	//// divide `cabeza` en las sublistas `a` y `b`
	middleSplit(*head, &a, &b);

	// ordenar recursivamente las sublistas
	mergeSort(&a);
	mergeSort(&b);

	// fusiona las dos listas previamente ordenadas
	*head = sortedMerge(a, b);
}

//---------------------------------FIN DE LISTAS------------------------------------------------------------

int main(void)
{

	/*FILE *arch;
	arch = fopen("prueba.txt","rb");
	if (arch==NULL){
		printf ("%s \n", "No se ha podido leer el archivo, proceso cancelado.");
		exit(1);
	}
	else{
		printf ("%s \n", "Archivo abierto correctamente");
	}*/

	const char *filename = "prueba.txt";
	FILE *in_file = fopen(filename, "rb");
	if (!in_file)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	struct stat sb;
	if (stat(filename, &sb) == -1)
	{
		perror("stat");
		exit(EXIT_FAILURE);
	}

	char *file_contents = malloc(sb.st_size/8);
	int part =  sb.st_size / 8; //divide el tamaño del documento en 8 partes (para 8 hilos)
	
	//fread(void *datos, int longitud, int longitud_unidad, FILE * handle)
	fread(file_contents, part, 1, in_file);
	printf("Fread 1: %s\n\n\n", file_contents);

	fread(file_contents, part+=1, 1, in_file);
	printf("Fread 2: %s\n\n\n", file_contents);

	fread(file_contents, part+=1, 1, in_file);
	printf("Fread 3: %s\n\n\n", file_contents);

	fread(file_contents, part+=1, 1, in_file);
	printf("Fread 4: %s\n\n\n", file_contents);

	fread(file_contents, part+=1, 1, in_file);
	printf("Fread 5: %s\n\n\n", file_contents);

	fread(file_contents, part+=1, 1, in_file);
	printf("Fread 6: %s\n\n\n", file_contents);

	fread(file_contents, part+=1, 1, in_file);
	printf("Fread 7: %s\n\n\n", file_contents);

	fread(file_contents, part+=1, 1, in_file);
	printf("Fread 8: %s\n\n\n", file_contents);

	fclose(in_file);

	free(file_contents);


	/*int keys[] = {86, 28, 4, 13, 186, 9}; //valores de emjemplo
	int n = sizeof(keys) / sizeof(keys[0]);

	struct Node *head = NULL;
	for (int i = 0; i < n; i++){ // los agrega
		push(&head, keys[i]);
	}

	// ordena la lista
	mergeSort(&head);

	printList(head);

	struct Node *new_node = newNode(5); // nuevo valor
	sortedInsert(&head, new_node); //lo inserta ordenado

	printList(head);*/

	return 0;
}