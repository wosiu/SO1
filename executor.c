#include <stdio.h>

#define BUF_SIZE 1024

int main(){
	//char out[BUF_SIZE] = ;
	char buf[BUF_SIZE];
	int size;

	if( ( size = read( 0, buf, BUF_SIZE - 1 ) ) == -1 )
		perror("read");

	buf [size < BUF_SIZE - 1 ? size : BUF_SIZE - 1] = '\0';

	strcat( buf, " executor" );
	//write( 1, buf, sizeof(buf)-1 );
	printf("%s",buf);

	return 0;
}
