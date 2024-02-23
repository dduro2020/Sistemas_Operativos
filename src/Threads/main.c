#include "stack.h"

int
main(int argc, char *argv[])
{
	pthreadt thread;

	if (pthread_create(&thread, NULL, fn(), NULL) != 0) {
		warnx("failed creating threat");
		return 1;
	}
	fprintf(stderr, " Hi! Im fn\n");
	if (pthread_join(thread, NULL) != 0) {
		warnx("failed joining threat");
		return 1;
	}
	return 0;
}
