#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
//#include "err.h"


#define BUF_SIZE 1024

void syserr( char* msg ) {
	perror(msg);
}

int main( int argc, const char* argv[] )
{
	int n = 2;
	char* input_path, output_path;
	if ( argc > 3 ) {
		n = atoi( argv[1] );
		input_path = argv[2];
		output_path = argv[3];
	}

	//char message[] = "2 7 + 3 / 14 3 - 4 * + 2 /";
	char* message = "  jakis napis z macierzystego.. ";
	//char* message = "#exit";

	// tworzymy lacza dla executerow
	n++; //bo manager
	int pipe_dsc[n][2];
	int i, size;
	for ( i = 0; i < n; i++ ) {
		if (pipe ( pipe_dsc[i] ) == -1) syserr("Error in pipe\n");
	}
  	// manager - indeks 0, executerzy [1, n-1]

	//przygotowanie łaczy dla executorow
	for ( i = 1; i < n; i++ ) {
		switch( fork() ) {
			case -1:
				syserr("Error in fork\n");
			// w potomku
			case 0:
				// zamiast na in, pojdzie na read do pipe
				if (dup2(pipe_dsc[ (i-1+n) % n ][0], STDIN_FILENO) == -1) syserr("Error in dup! [0]");
				// cos zamiast isc na out, bedzie szlo na write do pipe
				if (dup2(pipe_dsc[i][1], STDOUT_FILENO) == -1) syserr("Error in dup! [1]");
				execl("./executor", "executor", (char *) 0);
				syserr("Error in execl\n");
			// w macierzystym
			default:
				{syserr("macierzysty");}
		}
	}


	int tests_no = 2;

	// TODO for -> while
	// wczytujemy kolejne linie testu i puszczamy je w pierscien
	//
	for( i = 0; i < tests_no; i++ ) {
		if ( i == tests_no - 1 ) message = "#exit";
		// w managerze nie zastepujemy jego stdin/out. Zna jawnie deskryptory pipe
		// sluzace do komunikacji z executerami.
		// manager pisze do pierwszego pipe
		if ( write (pipe_dsc[0][1], message, sizeof(message) - 1) == -1 )
			syserr("write");
		// manager czyta z ostatniego pipe
		char buf[BUF_SIZE];

		printf("macierzysty napisal\n");


		if ( ( size = read (pipe_dsc[n-1][0], buf, sizeof(buf) - 1) ) == -1 )
			syserr("read");

		printf("macierzysty wczytal\n");

		buf [size < BUF_SIZE - 1 ? size : BUF_SIZE - 1] = '\0';

		printf("%s [koniec]\n",buf);
	}



	for ( i = 0; i < n; i++ ) {
		wait(0);
	}

	return 0;
}
