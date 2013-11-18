/*
	Michal Wos mw336071
	Zadanie zaliczeniowe 1
	Wykonawca
*/

#include <stdio.h>

const int BUF_SIZE = 16384;
const char EXIT[] = "#exit";

char* calculate( char *input )
{
	char res[ BUF_SIZE ];

	// do debugu
	if ( input[0] == '#' ) {
		return input;
	}

	// tutaj onp
	strcpy( res, input );
	return res;
}

int main(){
	//char out[BUF_SIZE] = ;
	char buf[BUF_SIZE];
	int size;
	int c = 0;
	char com[] = "executor";

	while ( 1 ) {

		// while not end of line - czyta tak dlugo az zostanie przeslana cala linia
		// bo caly test moze sie nie zmiescic w pipe na jeden raz
		// zalozyc ze noe linie beda zaczynac sie od # x onp #, gdzie x to numer testu
		// i moge to wysylac jako np 4 paczki (['#'],x,onp,'#')
		//if( ( size = read( 0, buf, BUF_SIZE - 1 ) ) == -1 )
		//	perror("read");
		// buf [size < BUF_SIZE - 1 ? size : BUF_SIZE - 1] = '\0';

		fprintf( stderr, "- executor wszedl do executera:\n" );

		if ( fgets (buf, BUF_SIZE, stdin) == NULL )
			perror("Cannot read data from file");

		//buf [strlen(buf) < BUF_SIZE - 1 ? strlen(buf) : BUF_SIZE - 1] = '\0';

		//if( strcmp(buf,EXIT) == 0 ) {
		fprintf( stderr, "- executor rozmiar przyjetego stringa: %d\n", strlen(buf) );

		if ( strlen(buf) == 0 ) {
			fprintf( stderr, "- executor pominiecie pustego stringa\n" );
			continue;
		}

		if( buf[0] == '#' ) {
			fprintf( stderr, "- executor przyjal exit: %s", buf );
			printf("%s",buf);
			return 0;
		} else {
			fprintf( stderr, "- executor przyjal onp: %s", buf );
		}

		// robienie ONP i wypisanie
		//write( 1, buf, sizeof(buf)-1 );

		//printf("%s", calculate( buf ) );
		fprintf( stderr, "- executor wysyla onp: %s", buf );
		printf("%s", buf );
		fflush(stdout);
	};
	return 0;
}
