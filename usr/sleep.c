#include "unix.h"

void catch()
{
	printf("^C\n");
}

main(argc, argv)
char **argv;
{
	int c, n;
	char *s;

	signal(SIGINT, catch);
	n = 0;
	if(argc < 2) {
		printf("arg count\n");
		exit();
	}
	s = argv[1];
	while(c = *s++) {
		if(c<'0' || c>'9') {
			printf("bad character\n");
			exit();
		}
		n = n*10 + c - '0';
	}
	sleep(n);
	printf("Done\n");
}
