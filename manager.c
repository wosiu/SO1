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
#define SYSERR(msg) syserr("%s, line %d: %s:",__FILE__,__LINE__,msg);


const int BUF_SIZE = 1024;
const char EXIT[] = "#exit";
char path[256];
const char data_dir[]="DATA/";

int main( int argc, const char* argv[] )
{
	// PARSOWANIE ARGUMENTOW, OTWIERANIE PLIKU Z DANYMI
	int n = 10, size;
	FILE *data_input, *data_output;
	if ( argc > 3 ) {
		n = atoi( argv[1] );

		strcpy( path, data_dir );
		if ( data_input = fopen( strcat( path, argv[2] ), "r") == NULL )
			SYSERR("Cannot open file with input");

		strcpy( path, data_dir );
		if ( data_output = fopen( strcat( path, argv[3] ), "wb+") == NULL )
			SYSERR("Cannot open or create file with input");
	}

	//char message[] = "2 7 + 3 / 14 3 - 4 * + 2 /";
	char message[] = "#jakis napis z macierzystego.. ";

	// TWORZENIE PIERSCIENIA

	int prince_in_pipe_dsc, prince_out_pipe_dsc;
	int pipe_dsc[2][2];
	if (pipe ( pipe_dsc[1] ) == -1) SYSERR("Error in pipe");
	// zapamietujemy deskryptor zapisywania do pipe, ktory bedzie polaczony
	// z wejsciem pierwszego wezla
	prince_in_pipe_dsc = pipe_dsc[1][1];
	//tworzymy lacza w pierscieniu i uruchamiamy wykonawcow
	int i;
	for ( i = 0; i < n; i++ ) {

		// pipe wychodzacy z poprzedniego wezla staje sie wchodzacym do obecnego
		pipe_dsc[0][0] = pipe_dsc[1][0];
		pipe_dsc[0][1] = pipe_dsc[1][1];
		// towrzymy nowy wychodzacy z obecnego wezla pipe
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

	// musimy jescze zamknac pipe stworzony w ostanim obroie petli
	if ( close( pipe_dsc[1][1] ) == -1 )
		SYSERR("Cannot close pipe descriptor");
	// czytania z pipe nie zamykamy, poniewaz stamtad bedzie czytac manager
	prince_out_pipe_dsc = pipe_dsc[1][0];

	int tests_no = 10;

	// TODO for -> while
	// wczytujemy kolejne linie testu i puszczamy je w pierscien
	//
	for( i = 0; i < tests_no; i++ ) {
		if ( i == tests_no - 1 ) strcpy( message, EXIT );
		// w managerze nie zastepujemy jego stdin/out. Zna jawnie deskryptory pipe
		// sluzace do komunikacji z executerami.
		// manager pisze do pierwszego pipe

		if ( write (prince_in_pipe_dsc, message, sizeof(message) - 1) == -1 )
			SYSERR("write");

		// manager czyta z ostatniego pipe
		char buf[BUF_SIZE];

		if ( ( size = read (prince_out_pipe_dsc, buf, sizeof(buf) - 1) ) == -1 )
			SYSERR("read");

		buf [size < BUF_SIZE - 1 ? size : BUF_SIZE - 1] = '\0';

		printf("%s [koniec]\n",buf);
	}

	for ( i = 0; i < n; i++ ) {
		wait(0);
	}

	return 0;
}
