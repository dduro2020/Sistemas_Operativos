#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <dirent.h>

enum {
	Bufsize = 8 * 1024,
};

int
countzeros(char *path, char *name)
{
	int nr, fd;
	char c[Bufsize];
	int count;
	int zeros = 0;
	char file[100];

	strcpy(file, path);
	strcat(file, name);

	fd = open(file, O_RDONLY);

	if (fd < 0) {
		printf("error: cannot open file\n");
	}
	while ((nr = read(fd, &c, Bufsize)) != 0) {
		count = 0;
		if (nr > 0) {
			while (count < nr) {
				if (c[count] == '\0') {
					zeros++;
				}
				count++;
			}
		} else {
			break;
		}
	}
	close(fd);

	return zeros;
}

void
readdirectory(char *path)
{
	int zeros = 0;
	DIR *d;
	struct dirent *ent;

	d = opendir(path);
	if (d == NULL) {
		err(EXIT_FAILURE, "error: open dir failed: %s", path);
	}
	strcat(path, "/");
	while ((ent = readdir(d)) != NULL) {
		if (ent->d_name[0] != '.') {
			zeros = countzeros(path, ent->d_name);
			printf("%d %s\n", zeros, ent->d_name);
		}
	}
	closedir(d);
}

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		errx(EXIT_FAILURE, "usage: %s directory", argv[0]);
	}
	readdirectory(argv[1]);

	exit(EXIT_SUCCESS);
}
