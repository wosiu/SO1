/*
	Michal Wos mw336071
	Zadanie zaliczeniowe 1
	Wykonawca
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include "err.h"
#include "commons.h"

char res[ BUF_SIZE ];
void calculate( char* onp );

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
		calculate( buf );
		printf( "%s", res );
		fflush( stdout );
	};
	return 0;
}


void calculate( char* onp )
{
	int onp_l = strlen(onp);
	int onp_it = 0;
	int i;

	for ( i = 0; i < BUF_SIZE; i++ )
		res[i] = 0;

	// przepisujemy numer lini
	for ( onp_it = 0; onp_it < onp_l; onp_it++ ) {
		res[onp_it] = onp[onp_it];
		if ( onp[onp_it] == ':' ) {
			break;
		}
	}

	res[ ++onp_it] = ' ';

	// szukamy pierwszego operatora arytmetycznego,
	// jego polozenie pamietamy w zmiennej op
	// zapamietujemy takze polozenie 3 ostatnich spacji przed nim
	int space[3], op;
	space[0] = onp_l - 1;
	space[1] = space[2] = onp_it++;
	op = onp_it;

	while ( op < onp_l ) {
		if ( onp[op] == ' ' ) {
			space[0] = space[1];
			space[1] = space[2];
			space[2] = op;
		} else if ( is_operator( onp, op ) ) {
			break;
		}
		op++;
	}

	// przepisujemy prefix wyrazenia, ktory nie ulegnie zmianie
	for	( ; onp_it <= space[0]; onp_it++ ) {
		res[onp_it] = onp[onp_it];
	}

	// jesli nie znaleziono operatora, to wyrazenie jest obliczone
	if ( op  == onp_l ) {
		return;
	}

	int res_it = onp_it;

	// zamieniamy dwie liczby przed operatorem na wartosci liczbowe
	int a, an, bn, b, w;
	an = bn = 1;
	a = b = 0;

	// sprawdzamy czy ujemna liczba
	if ( onp[ onp_it ] == '-' ) {
		an = -1;
		onp_it++;
	}

	for ( ; onp_it < space[1]; onp_it++ ) {
		a = a * 10 + (int)onp[onp_it] - (int)'0';
	}
	a *= an;
	// omijamy spacje
	onp_it++;
	if ( onp[ onp_it ] == '-' ) {
		bn = -1;
		onp_it++;
	}
	for ( ; onp_it < space[2]; onp_it++ ) {
		b = b * 10 + (int)onp[onp_it] - (int)'0';
	}
	b *= bn;

	// wykonujemy dzialanie zgodne z operatorem
	switch ( onp[op] ) {
		case '+': w = a + b; break;
		case '-': w = a - b; break;
		case '*': w = a * b; break;
		case '/': w = a / b; break;
	}

	// dopisujemy obliczona liczbe do ciagu wynikowego
	char tmp[33];
	snprintf( tmp, 33, "%d", w );

	for ( i = 0; i < strlen(tmp); i++ ) {
		res[ res_it++ ] = tmp[i];
	}

	// dopisujemy do ciagu wynikowego sufix
	onp_it = op + 1;
	for ( ; onp_it < onp_l; onp_it++ ) {
		res[ res_it++ ] = onp[ onp_it ];
	}
}
