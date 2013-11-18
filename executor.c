/*
	Michal Wos mw336071
	Zadanie zaliczeniowe 1
	Wykonawca
*/

#include <stdio.h>
#include <string.h>
#include "err.h"

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
	return input;
}

int main(){
	char buf[BUF_SIZE];

	while ( 1 ) {
		if ( fgets ( buf, BUF_SIZE, stdin ) == NULL )
			syserr( "Cannot read data from stdin" );

		if( strcmp( buf, EXIT ) == 0 ) {
			printf( "%s", buf );
			return 0;
		}
		// robienie ONP i wypisanie

		printf( "%s", buf );
		fflush( stdout );
	};
	return 0;
}
