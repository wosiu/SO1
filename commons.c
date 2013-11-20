#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "commons.h"

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
