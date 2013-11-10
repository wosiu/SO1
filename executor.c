#include <stdio.h>

const int BUF_SIZE = 1024;
const char EXIT[] = "#exit";

int main(){
	//char out[BUF_SIZE] = ;
	char buf[BUF_SIZE];
	int size;
	int c = 0;
	char com[] = "executor";

	do {
		//strcat( com, " I" );
		//perror( com );

		if( ( size = read( 0, buf, BUF_SIZE - 1 ) ) == -1 )
			perror("read");

		//strcat( com, "-" );
		//perror( com );

		buf [size < BUF_SIZE - 1 ? size : BUF_SIZE - 1] = '\0';
		//perror ( buf );
		//strcat( buf, " executor" );
		//write( 1, buf, sizeof(buf)-1 );
		printf("%s",buf);
		fflush(stdout);
		//strcat( com, "-" );
		//perror( com );

	} while ( strcmp(buf,EXIT) != 0 );
	//} while ( 0 );


	//while (1) {}

	return 0;
}
