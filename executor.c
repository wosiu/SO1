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

const int BUF_SIZE = 4096;
const char EXIT[] = "#exit\n";

char res[ 4096 ];

int is_operator( char* onp, int op )
{
	int onp_l = strlen( onp );
	// sprawdzamy czy to operator uwzgledniajac potencjalny minus przy liczbie
	if ( isdigit( onp[op] ) ) {
		return 0;
	}
	if ( isspace( onp[op] ) ) {
		return 0;
	}
	if ( op + 1 < onp_l && onp[op]=='-' && isdigit( onp[op + 1] ) ) {
		return 0;
	}
	return 1;
}

int is_ready( char *onp )
{
	int i = 0;
	for ( ; i < strlen( onp ); i++ ) {
		switch ( onp[i] ) {
			case '+': return 0;
			case '-': if ( is_operator( onp, i ) ) return 0; break;
			case '*': return 0;
			case '/': return 0;
		}
	}
	return 1;
}

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

	//printf("len %d\n", onp_l);

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
	//printf("onp_it: %d\n",onp_it);
	//printf("przepisany numer lini: %s|\n", res );

	if ( onp_it == onp_l ) {
		SYSERR( "Incorrect format of onp line - no line number")
	}

	res[ ++onp_it] = ' ';

	//printf("dodana spacja: %s|\n", res );


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
		//TODO: sprawdzic czy sie nie wychrzania na "\n"
		} else if ( is_operator( onp, op ) ) {
			break;
		}
		op++;
	}

	//printf("sp1:%d, sp2:%d, sp3:%d. op:%d\n",space[0],space[1],space[2],op);
	//printf( "onp size: %d\n", onp_l );

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

	//printf("operator: %c\n", onp[op] );
	assert( onp_it == op - 1 );

	// wykonujemy dzialanie zgodne z operatorem
	switch ( onp[op] ) {
		case '+': w = a + b; break;
		case '-': w = a - b; break;
		case '*': w = a * b; break;
		case '/': w = a / b; break;
		default: SYSERR( "Unrecognised operator" );
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
