/*
	Michal Wos mw336071
	Zadanie zaliczeniowe 1
	Zarzadca
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "err.h"

const int BUF_SIZE = 16384;
const char EXIT[] = "#exit\n";
const char data_dir[]="DATA/";


int is_ready( char* onp )
{
	return 1;
}

int main( int argc, const char* argv[] )
{
	// ====== PARSOWANIE ARGUMENTOW, OTWIERANIE PLIKU Z DANYMI =================

	int n;
	char path[256];
	FILE *data_input_stream, *data_output_stream, *ring_out_stream,
		*ring_in_stream;

	if ( argc == 4 ) {
		n = atoi( argv[1] );

		strcpy( path, data_dir );
		strcat( path, argv[2] );
		if ( ( data_input_stream = fopen( path, "r") ) == NULL )
			SYSERR("Cannot open file with input");

		strcpy( path, data_dir );
		strcat( path, argv[3] );
		if ( ( data_output_stream = fopen( path, "wb+") ) == NULL )
			SYSERR( "Cannot open or create file with input" );
	} else {
		SYSERR( "Incorrect number of arguments" );
		return 0;
	}

	// ====== TWORZENIE PIERSCIENIA ============================================

	int ring_in_pipe_dsc, ring_out_pipe_dsc;
	int pipe_dsc[2][2];
	if ( pipe ( pipe_dsc[1] ) == -1 ) SYSERR( "Error in pipe" );
	// zapamietujemy deskryptor zapisywania do pipe, ktory bedzie polaczony
	// z wejsciem pierwszego wezla
	ring_in_pipe_dsc = pipe_dsc[1][1];

	int i;
	for ( i = 0; i < n; i++ ) {
		// pipe wychodzacy z poprzedniego wezla staje sie wchodzacym do obecnego
		pipe_dsc[0][0] = pipe_dsc[1][0];
		pipe_dsc[0][1] = pipe_dsc[1][1];
		// towrzymy nowy pipe wychodzacy z obecnego wezla
		if ( pipe ( pipe_dsc[1] ) == -1 )
			SYSERR( "Cannot create pipe" );

		switch( fork() ) {
			case -1:
				SYSERR( "fork" );
			// w potomku
			case 0:
				// zamykam nie uzywane deskryptory:
				// ten na ktory pisal poprzedni wezel
				if ( close( pipe_dsc[0][1] ) == -1 )
					SYSERR( "Cannot close pipe descriptor" );
				// ten z ktorego bedzie czytal nastepny wezel
				if ( close( pipe_dsc[1][0] ) == -1 )
					SYSERR( "Cannot close pipe descriptor" );
				// dane zamiast z stdin, przyjda z read z pipe
				if ( dup2( pipe_dsc[0][0], STDIN_FILENO ) == -1 )
					SYSERR( "Cannot duplicate pipe descriptor" );
				// dane zamiast isc na stdout, bedza szly na write do pipe
				if ( dup2( pipe_dsc[1][1], STDOUT_FILENO ) == -1 )
					SYSERR( "Cannot duplicate pipe descriptor" );

				execl( "./executor", "executor", (char *) 0 );
				SYSERR( "execl" );
			// w macierzystym
			default:
			{
				// w procesie macierzystym zamykamy lacza stworzone do
				// komunikacji miedzy wezlami w pierscieniu, poniewaz ich kopie
				// sa w odpowiednich wezlach
				if ( close( pipe_dsc[0][0] ) == -1 )
					SYSERR( "Cannot close pipe descriptor" );
				// ..oprocz pisania do pierwszego pipe - tam bedzie pisac manager
				if ( i != 0 ) {
					if ( close( pipe_dsc[0][1] ) == -1 )
						SYSERR( "Cannot close pipe descriptor" );
				}
			}
		}
	}
	// musimy jescze zamknac pipe stworzony w ostanim obrocie petli
	if ( close( pipe_dsc[1][1] ) == -1 )
		SYSERR( "Cannot close pipe descriptor" );
	// czytania z pipe nie zamykamy, poniewaz stamtad bedzie czytac manager
	ring_out_pipe_dsc = pipe_dsc[1][0];

	// otworzenie strumieni w celu przyjemniejszej komunikacji z pierscieniem
	if ( ( ring_out_stream = fdopen( ring_out_pipe_dsc, "r" ) ) == NULL )
		SYSERR( "Cannot open stream from ring out descriptor" );
	if ( ( ring_in_stream = fdopen( ring_in_pipe_dsc, "w" ) )  == NULL )
		SYSERR( "Cannot open stream from ring in descriptor" );
	setbuf ( ring_in_stream , NULL );


	// ====== WCZYTYWANIE I OBSLUGIWANIE DANYCH ================================

	int tests_no, tests_it = 0;
	int onp_in_ring = 0;
	char buf[ BUF_SIZE ];

	//wczytywanie liczby testow z pliku
	if ( fscanf( data_input_stream, "%d\n", &tests_no ) == 0 )
		SYSERR( "Cannot read tests number from file" );

	while ( 1 ) {
		// pierscien pelen lub koniec pliku z danymi a w pierscieniu jakies dane
		if ( onp_in_ring == n || ( onp_in_ring > 0 && tests_it == tests_no ) ) {
			onp_in_ring--;
			// pobieramy wyrazenie onp z pierscienia
			if ( fgets ( buf, BUF_SIZE, ring_out_stream ) == NULL )
				SYSERR( "Cannot read data from ring" );
			// jesli wyrazenie te jest wyliczone, zapisujemy wynik do pliku
			if ( is_ready( buf ) ) {
				fputs( buf, data_output_stream );
			// w przeciwnym razie wkladamy spowrotem do pierscienia
			} else {
				if ( fputs( buf, ring_in_stream ) < 0 )
					SYSERR( "Cannot rewrite to ring" );
				onp_in_ring++;
			}
		// w pierscieniu wolne miejsce na dane a w pliku sa dane
		} else if ( tests_it < tests_no ) {
			tests_it++;
			// dodajemy prefiks z numerem wiersza
			snprintf( buf, BUF_SIZE, "%d: ", tests_it );
			int len = strlen( buf );
			// czytamy z pliku kolejny test
			if ( fgets ( buf + len, BUF_SIZE - len, data_input_stream ) == NULL )
				SYSERR( "Cannot read data from file" );
			// wkladamy go do pierscienia
			if ( fputs( buf, ring_in_stream ) < 0 )
				SYSERR( "Cannot write to ring data from file" );
			onp_in_ring++;
		// wszystkie dane obliczone
		} else {
			break;
		}
	}

	// wysylamy do wykonawcow informacje o zakonczeniu
	if ( fputs( EXIT, ring_in_stream ) < 0 )
		SYSERR( "Cannot write exit command to ring" );

	for ( i = 0; i < n; i++ ) {
		wait( 0 );
	}
	fclose( ring_in_stream );
	fclose( ring_out_stream );
	return 0;
}
