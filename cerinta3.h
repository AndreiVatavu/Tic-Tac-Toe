#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Structura unui nod din arbore
typedef struct node
{
	// Tabla de joc
	// Pentru primele 2 cerinte
	int board[3][3];
	char c;
	// Lista tuturor succesorilor
	// Pentru cerinta 3
	int val;
	int nr_succesori;
	// Lista tuturor succesorilor
	struct successors
	{
		struct node* nod;
		struct successors* next;
		struct successors* last;
	}*successors;
}node;

typedef struct queue
{
	node* nod;
	struct queue* next;
	struct queue* last;
}queue;

void init_root ( node** root, int val, int add )
{
	node* new = ( node* ) malloc ( sizeof ( node ) );
	if ( add )
		new->val = val;
	new->successors = NULL;
	*root = new;
}

void enqueue ( queue** q, node** nod )
{
	queue* new = ( queue* ) malloc ( sizeof ( queue ) );
	new->nod = *nod;
	new->next = NULL;
	if ( *q == NULL )
	{
		*q = new;
		(*q)->last = new;
	}
	else
	{
		(*q)->last->next = new;
		(*q)->last = new;
	}
}

void dequeue ( queue** q )
{
	*q = (*q)->next;
}

void add_successor ( node** parent, int val, int add )
{
	/*
	  Intrari:
		-> parent: nodul parinte caruia i se va adauga un succesor
		-> val: valoarea asociata noului nod
		-> add: specifica daca se va adauga o valoare sau daca doar se va aloca memorie
	*/

	// Creez noul nod ce va fi adaugat
	// Aloc memorie
	struct successors* new = ( struct successors* ) malloc ( sizeof ( struct successors ) );

	// Adaug valoarea
	init_root ( &new->nod, val, add );
	if ( add == 0 )
		new->nod->nr_succesori = val;
	new->next = NULL;

	// Daca nodul parinte nu are niciun succesor in lista de succesori,
	// se adauga primul in lista de succesori
	if ( (*parent)->successors == NULL )
	{
		(*parent)->successors = new;
		(*parent)->successors->last = new;
	}
	// Daca lista de succesori nu este goala,
	// se merge pana la ultimul nod din lista si se adauga acolo
	else
	{
		(*parent)->successors->last->next = new;
		(*parent)->successors->last = new;
	}
}

void DFS ( node* root, int tabs, FILE* output )
{
	/*
		Intrari:
			-> root: radacina arborelui
			-> tabs: numarul de taburi ce trebuiesc afisate inaintea nodului
			-> output: Fisierul de iesire
	*/
	int i;
	// Afisez tabla asociata nodului curent sau arborele SI/SAU, depinde de cerinta

	for ( i = 0; i < tabs; i++ )
		fprintf ( output, "\t" );
	fprintf ( output, "%d\n", root->val );

	// Cat timp nodul curent are succesori, ii parcurg pe fiecare si repet algoritmul
	struct successors* aux = root->successors;
	while ( aux )
	{
		DFS ( aux->nod, tabs + 1, output );
		// Trec la urmatorul succesor
		aux = aux->next;
	}	
}



// Functia extrage valoarea din paranteze
int extract ( FILE* input, char* buffer )
{
	// Elimin parantezele din sir
	buffer = buffer + 1;
	buffer[ strlen ( buffer ) - 1 ] = '\0';

	// Extrag numarul intreg din sir
	int nr;
	sscanf ( buffer, "%d", &nr );

	return nr;
}

void init_tree ( queue** q, FILE* input )
{
	int val, i;

	// Aloc memorie unui buffer si citesc
	char* buffer = ( char* ) malloc ( 12 * sizeof ( char ) );
	queue* temp = *q; 

	while ( temp != NULL )
	{	
		for ( i = 0; i < temp->nod->nr_succesori; i++ )
		{
			fscanf ( input, "%s", buffer );
			int sw = 1;
			if ( buffer[0] == '(' )
				sw = 0;

			val = extract ( input, buffer );
			add_successor ( &temp->nod, val, sw );
			// struct successors* aux = temp->nod->successors;
			if ( sw == 0 )
			{
				// while ( aux->next != NULL )
				// 	aux = aux->next;
				enqueue ( q, &( temp->nod->successors->last->nod ) );
			}
		}
		dequeue ( &temp );
	}
	free ( buffer );
}

int minimax ( node** root, int player )
{
	if ( (*root)->successors == NULL )
		return (*root)->val;
	struct successors* aux = (*root)->successors;
	int scor = -9999999 * player;
	while ( aux )
	{
		int temp_scor = minimax ( &( aux->nod ), -1 * player );
		if ( player == 1 )
		{
			if ( temp_scor > scor )
				scor = temp_scor;
		}
		else
			if ( temp_scor < scor )
				scor = temp_scor;
		aux = aux->next;
	}
	if ( scor == -9999999 * player )
		scor = 0;
	(*root)->val = scor;
	return scor;
}

void free_successors ( struct successors** successor )
{
	if ( (*successor) )
	{
		free_successors ( &(*successor)->next );
		free ( (*successor)->nod );
		free ( *successor );
	}
}	

void free_root ( node** root )
{
	struct successors* aux = (*root)->successors;
	while ( aux )
	{
		free_root ( &aux->nod );
		aux = aux->next;
	}
	if ( (*root)->successors )
		free_successors ( &(*root)->successors );
}

void free_queue ( queue** q )
{
	if ( (*q)->next )
		free_queue ( &(*q)->next );
	free ( *q );
}