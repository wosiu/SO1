/*
	Michal Wos mw336071
	Zadanie zaliczeniowe 1
	Zarzadca
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"
#define SYSERR(msg) syserr("%s, line %d: %s:", __FILE__, __LINE__, msg);
#define BUF_SIZE 16384

char buf[ BUF_SIZE ];
const char EXIT[] = "#exit\n";
char path[256];
const char data_dir[]="DATA/";


int is_ready( char* onp )
{
	return 1;
}

int main( int argc, const char* argv[] )
{
	// PARSOWANIE ARGUMENTOW, OTWIERANIE PLIKU Z DANYMI

	int n = 10, size;
	FILE *data_input;
	FILE *data_output;

	if ( argc == 4 ) {
		n = atoi( argv[1] );

		strcpy( path, data_dir );
		if ( ( data_input = fopen( strcat( path, argv[2] ), "r") ) == NULL )
			SYSERR("Cannot open file with input");

		strcpy( path, data_dir );
		if ( ( data_output = fopen( strcat( path, argv[3] ), "wb+") ) == NULL )
			SYSERR("Cannot open or create file with input");
	} else {
		SYSERR("Incorrect number of arguments");
		return 0;
	}

	// TWORZENIE PIERSCIENIA

	int prince_in_pipe_dsc, prince_out_pipe_dsc;
	int pipe_dsc[2][2];
	if (pipe ( pipe_dsc[1] ) == -1) SYSERR("Error in pipe");
	// zapamietujemy deskryptor zapisywania do pipe, ktory bedzie polaczony
	// z wejsciem pierwszego wezla
	prince_in_pipe_dsc = pipe_dsc[1][1];

	int i;
	for ( i = 0; i < n; i++ ) {
		// pipe wychodzacy z poprzedniego wezla staje sie wchodzacym do obecnego
		pipe_dsc[0][0] = pipe_dsc[1][0];
		pipe_dsc[0][1] = pipe_dsc[1][1];
		// towrzymy nowy pipe wychodzacy z obecnego wezla
		if (pipe ( pipe_dsc[1] ) == -1)
			SYSERR("Cannot create pipe");

		switch( fork() ) {
			case -1:
				SYSERR("fork");
			// w potomku
			case 0:
				// zamykam nie uzywane deskryptory:
				// ten na ktory pisal poprzedni wezel
				if ( close( pipe_dsc[0][1] ) == -1 )
					SYSERR("Cannot close pipe descriptor");
				// ten z ktorego bedzie czytal nastepny wezel
				if ( close( pipe_dsc[1][0] ) == -1 )
					SYSERR("Cannot close pipe descriptor");
				// dane zamiast z stdin, przyjda z read z pipe
				if (dup2(pipe_dsc[0][0], STDIN_FILENO) == -1)
					SYSERR("Cannot duplicate pipe descriptor");
				// dane zamiast isc na stdout, bedza szly na write do pipe
				if (dup2(pipe_dsc[1][1], STDOUT_FILENO) == -1)
					SYSERR("Cannot duplicate pipe descriptor");

				execl("./executor", "executor", (char *) 0);
				SYSERR("execl");
			// w macierzystym
			default:
			{
				// w procesie macierzystym zamykamy lacza stworzone do komunikacji
				// miedzy wezlami w pierscieniu, poniewaz ich kopie sa w odpowiednich wezlach
				if ( close( pipe_dsc[0][0] ) == -1 )
					SYSERR("Cannot close pipe descriptor");
				// ..oprocz pisania do pierwszego pipe - tam bedzie pisac manager
				if ( i != 0 ) {
					if ( close( pipe_dsc[0][1] ) == -1 )
						SYSERR("Cannot close pipe descriptor");
				}
			}
		}
	}
	// musimy jescze zamknac pipe stworzony w ostanim obrocie petli
	if ( close( pipe_dsc[1][1] ) == -1 )
		SYSERR("Cannot close pipe descriptor");
	// czytania z pipe nie zamykamy, poniewaz stamtad bedzie czytac manager
	prince_out_pipe_dsc = pipe_dsc[1][0];


	// WCZYTYWANIE I OBSLUGIWANIE DANYCH

	//wczytywanie liczby testow z pliku
	int tests_no, tests_it = 0;
	if ( fscanf(data_input, "%d\n", &tests_no) == 0 )
		SYSERR("Cannot read tests number from file");

	int onp_in_ring = 0;
	FILE *prince_out_stream = fdopen( prince_out_pipe_dsc, "r" );
	FILE *prince_in_stream = fdopen( prince_in_pipe_dsc, "w" );

	while (1) {
		// pierscien pelen lub koniec pliku z danymi a w pierscieniu jakies dane
		if ( onp_in_ring == n || ( onp_in_ring > 0 && tests_it == tests_no ) ) {
			onp_in_ring--;

			printf("1 if: start\n");
			fgets ( buf, BUF_SIZE, prince_out_stream );
			printf("1 if: wczytal: %s", buf);

			// jesli onp jest wyliczone, zapisujemy do pliku
			if ( is_ready( buf ) ) {
				fprintf(data_output,"%s",buf);
				fflush(data_output);
			// w przeciwnym razie wkladamy spowrotem do pierscienia
			} else {
				if ( write (prince_in_pipe_dsc, buf, strlen(buf) - 1) == -1 )
					SYSERR("Cannot rewrite to ring");
				onp_in_ring++;
			}

			printf("1 if: end \n");
		// w pierscieniu wolne miejsce na dane a w pliku sa dane
		} else if ( tests_it < tests_no ) {
			printf("2 if: start\n");
			tests_it++;
			// zapisujemy do bufora numer testu
			snprintf(buf, BUF_SIZE,"%d: ",tests_it);
			int len = strlen(buf);
			// czytamy z pliku kolejny test i dodajemy go do bufora
			if ( fgets (buf + len, BUF_SIZE - len, data_input) == NULL )
				SYSERR("Cannot read data from file");
			// wkladamy do pierscienia
			printf("2 if: %s", buf);
			printf("Wysylam wyrazenie do pierscienia: %s\n", buf);
			if ( write (prince_in_pipe_dsc, buf, strlen(buf) ) == -1 )
				SYSERR("Cannot write to ring data from file");
			onp_in_ring++;
		// wszystkie dane obliczone
			printf("2 if: koniec \n");
		} else {
			printf("THE END\n");
			if ( write (prince_in_pipe_dsc, EXIT, strlen(EXIT)) == -1 )
				SYSERR("Cannot write exit command to ring");

			printf("wyslano exit\n");
			break;
		}
	}

	for ( i = 0; i < n; i++ ) {
		wait(0);
	}

	return 0;
}
