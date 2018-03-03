#include "cerinta3.h"

void init_root1 ( node** root, int board[3][3] )
{
	node* new = ( node* ) malloc ( sizeof ( node ) );
	for ( int i = 0; i < 3; i++ )
		for ( int j = 0; j < 3; j++ )
			new->board[i][j] = board[i][j];
	
	new->successors = NULL;
	*root = new;
}

void add_successor1 ( node** parent, int board[3][3] )
{
	/*
	  Intrari:
		-> parent: nodul parinte caruia i se va adauga un succesor
		-> board: tabla de joc asociata noului nod
	*/

	// Creez noul nod ce va fi adaugat
	// Aloc memorie
	struct successors* new = ( struct successors* ) malloc ( sizeof ( struct successors ) );

	// Copiez tabla
	init_root1 ( &new->nod, board );
	new->next = NULL;

	// Daca nodul parinte nu are niciun succesor in lista de succesori,
	// se adauga primul in lista de succesori
	if ( (*parent)->successors == NULL )
		(*parent)->successors = new;
	// Daca lista de succesori nu este goala,
	// se merge pana la ultimul nod din lista si se adauga acolo
	else
	{
		// Creez o copie al primului succesor
		struct successors* aux = (*parent)->successors;
		// Parcurg lista de succesori pana la ultimul
		while ( aux->next != NULL )
			aux = aux->next;
		// Adaug nodul
		aux->next = new;
	}
}

int score ( int board[3][3] )
{
	// Intrari: -> board: tabla de joc
	// Iesire: -> scorul
	int i;

	for ( i = 0; i < 3; i++ )
	{
		// Verific daca exista vreo linie castigatoare
		if ( board[i][0] !=0 &&
			 board[i][0] == board[i][1] && board[i][1] == board[i][2] )
			return board[i][0];
		// Verific daca exisra vreo coloana castigatoare
		if ( board[0][i] != 0 &&
			 board[0][i] == board[1][i] && board[1][i] == board[2][i] )
			return board[0][i];
	}
	// Verific daca diagonala principala este castigatoare
	if ( board[0][0] != 0 &&
		 board[0][0] == board[1][1] && board[1][1] == board[2][2] )
		return board[0][0];
	// Verific daca diagonala secundara este castigatoare
	if ( board[0][2] != 0 &&
		 board[0][2] == board[1][1] && board[1][1] == board[2][0] )
		return board[0][2];

	return 0;
}

int minimax1 ( node** root, int board[3][3], int player )
{
	/*
		Intrari:
			-> root: nodul curent din arbore
			-> board: tabla de joc
			-> player: jucatorul care trebuie sa faca miscarea
		Iesire:
			-> scorul obtinut prin efectuarea mutarii
	*/

	int i, j, scor;
	int winner = score ( board );
	// Verific daca a castigat cineva
	if ( winner )
		// Daca da, atunci castigatorul este jucatorul anterior
		return -1 * player;

	// Initializez scorul cu o valoarea care sa nu afecteze
	// calculul minimului sau a maximului, in functie de situatie
	scor = -2 * player;
	for ( i = 0; i < 3; i++ )
		for ( j = 0; j < 3; j++ )\
		{
			if ( board[i][j] == 0 )
			{
				// Se incearca mutarea
				board[i][j] = player;
				// Se adauga nodul in arbore
				add_successor1 ( root, board );
				struct successors* aux = (*root)->successors;
				while ( aux->next )
					aux = aux->next;
				int temp_scor = minimax1 ( &aux->nod, board, -1 * player );
				// aux = aux->next;

				if ( player == 1 )
				{
					if ( temp_scor > scor )
						scor = temp_scor;
				}
				else
					if ( temp_scor < scor )
						scor = temp_scor;

				// Refac tabla la loc
				board[i][j] = 0;
			}
		}

	if ( scor == -2 * player )
		return 0;
	return scor;
}

void DFS1 ( node* root, int tabs, char c1, char c2, FILE* output, int cerinta )
{
	/*
		Intrari:
			-> root: radacina arborelui
			-> tabs: numarul de taburi ce trebuiesc afisate inaintea nodului
			-> c1: Simbolul AI-ului
			-> c2: Simbolul adversarului
			-> output: Fisierul de iesire
			-> cerinta: Va aface anumite afisari in functie de cerinta
	*/
	int i, j, k;
	// Afisez tabla asociata nodului curent sau arborele SI/SAU, depinde de cerinta

	if ( cerinta == 1 )
	{
		// Afisez arborele de joc
		for ( i = 0; i < 3; i++ )
		{
			for ( k = 1; k <= tabs; k++ )
				fprintf ( output, "\t" );
			for ( j = 0; j < 3; j++ )
			{
				if ( root->board[i][j] == 1 )
					fprintf ( output, "%c", c1 );
				else
					if ( root->board[i][j] == -1 )
						fprintf ( output, "%c", c2 );
					else
						fprintf ( output, "-" );
				if ( j != 2 )
					fprintf ( output, " " );
			}
			fprintf ( output, "\n" );
		}
		fprintf ( output, "\n" );
	}
	else
	{
		// Afisez arborele SI/SAU
		for ( i = 0; i < tabs; i++ )
			fprintf ( output, "\t" );
		fprintf ( output, "%c\n", root->c );
	}

	// Cat timp nodul curent are succesori, ii parcurg pe fiecare si repet algoritmul
	struct successors* aux = root->successors;
	while ( aux )
	{
		DFS1 ( aux->nod, tabs + 1, c1, c2, output, cerinta );
		// Trec la urmatorul succesor
		aux = aux->next;
	}	
}

void build_AndOr_tree ( node** root, int player )
{
	if ( !(*root)->successors )
	{
		int scor;
		scor = score ( (*root)->board );
		if ( scor == 1 )
			(*root)->c = 'T';
		else
			(*root)->c = 'F';
	}
	struct successors* aux = (*root)->successors;
	while ( aux )
	{
		build_AndOr_tree ( &aux->nod, -1 * player );
		// Trec la urmatorul succesor
		aux = aux->next;
	}	


	aux = (*root)->successors;
	if ( aux )
	{
		if  ( player == -1 )
		{
			(*root)->c = 'T';
			while ( aux )
			{
				if ( aux->nod->c == 'F' )
				{
					(*root)->c = 'F';
					break;
				}
				aux = aux->next;
			}
		}
		else
		{
			(*root)->c = 'F';
			while ( aux )
			{
				if ( aux->nod->c == 'T' )
				{
					(*root)->c = 'T';
					break;
				}
				aux = aux->next;
			}
		}
	}
}

int main ( int argc, char** argv )
{
	// Deschid fisierele
	FILE *input, *output;
	input = fopen ( argv[2], "r" );
	if ( !input )
		return 1;
	output = fopen ( argv[3], "w" );

	switch ( argv[1][ strlen ( argv[1]) - 1 ] )
	{
		case '1':
		case '2':
		{
			char c1, c2;
			fscanf ( input, "%c\n", &c1 );

			if ( c1 == 'X' )
				c2 = 'O';
			else
				c2 = 'X';

			int board[3][3];
			int i, j, k;
			for ( i = 0; i < 3; i++ )
			{
				k = 0;
				char line[11];
				fgets ( line, 10, input );
				// board[i] = ( int* ) malloc ( 3 * sizeof ( int ) );
				for ( j = 0; j < 3; j++ )
				{
					if ( line[k] == c1 )
						board[i][j] = 1;
					else
						if ( line[k] == c2 )
							board[i][j] = -1;
						else
							board[i][j] = 0;
					k += 2;
				}
			}

			node* root;
			init_root1 ( &root, board );
			minimax1 ( &root, board, 1 );

			if ( argv[1][ strlen ( argv[1]) - 1 ] == '1' )
				DFS1 ( root, 0, c1, c2, output, 1 );
			else
			{
				build_AndOr_tree ( &root, 1 );
				DFS1 ( root, 0, c1, c2, output, 2 );
			}

			// Eliberarea memoriei
			free_root ( &root );
			free ( root );
			break;
		}

		case '3':
		{
			node* root;
			queue* q;
			q = NULL;
			// Citesc inaltimea arborelui
			int h;
			fscanf ( input, "%d", &h );
			int val;

			// Aloc memorie unui buffer si citesc
			char* buffer = ( char* ) malloc ( 12 * sizeof ( char ) );
			fscanf ( input, "%s", buffer );
			val = extract ( input, buffer );

			init_root ( &root, val, 0 );
			root->nr_succesori = val;
			enqueue ( &q, &root );
			
			init_tree ( &q, input );
			
			minimax ( &(q->nod), 1 );
			
			DFS ( q->nod, 0, output );

			free_root ( &(q->nod) );
			free_queue ( &q );
			free ( root );
			free ( buffer );
			break;
		}

		case 'b':

			break;
	}

	fclose ( input );
	fclose ( output );
	return 0;
}