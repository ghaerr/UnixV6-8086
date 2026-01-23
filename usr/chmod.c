#include "unix.h"

int main(int argc, char **argv)
{
	register i, m;
	register char *c;
	int count = 0;

	if(argc < 3) {
		printf("arg count\n");
		exit1(1);
	}
	c = argv[1];
	m = 0;
	for(m=0; *c; c++) {
		if(*c < '0' || *c > '7') {
			printf("bad mode\n");
			exit1(1);
		}
		m = (m<<3) | *c - '0';
	}
	for(i=2; i<argc; i++)
		if(chmod(argv[i], m) < 0) {
			count++;
			perror(argv[i]);
		}
	exit1(count);
}
