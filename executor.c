/*
	Michal Wos mw336071
	Zadanie zaliczeniowe 1
	Wykonawca
*/

#include <stdio.h>

const int BUF_SIZE = 16384;
const char EXIT[] = "#exit\n";

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
	char buf[BUF_SIZE];
	int size;
	int c = 0;
	char com[] = "executor";

	while ( 1 ) {

		// while not end of line - czyta tak dlugo az zostanie przeslana cala linia
		// bo caly test moze sie nie zmiescic w pipe na jeden raz
		// zalozyc ze noe linie beda zaczynac sie od # x onp #, gdzie x to numer testu
		// i moge to wysylac jako np 4 paczki (['#'],x,onp,'#')
		if ( fgets (buf, BUF_SIZE, stdin) == NULL )
			perror("Cannot read data from file");

		if( buf[0] == '#' ) {
			printf("%s",buf);
			return 0;
		}
		// robienie ONP i wypisanie

		printf("%s", buf );
		fflush(stdout);
	};
	return 0;
}
