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
	Pathsize = 1024,
};

struct files_count {
	int c;
	int h;
	long int bytes;
};

typedef struct files_count Files_count;

void
extractpath(char *path, char *actualpath, char *name)
{

	strcpy(path, actualpath);
	strcat(path, "/");
	strcat(path, name);
}

int
count_bytes(char *file)
{
	int file_bytes;
	struct stat sb;

	stat(file, &sb);
	file_bytes = sb.st_size;

	return file_bytes;
}

Files_count
terms_sum(Files_count files, Files_count files_sum)
{

	files_sum.c += files.c;
	files_sum.h += files.h;
	files_sum.bytes += files.bytes;

	return files_sum;
}

Files_count
readfiles(char *path)
{
	Files_count files;

	files.h = 0;
	files.c = 0;
	files.bytes = 0;

	char *p = strrchr(path, '.');

	if (p[1] == 'c' && p[2] == '\0') {
		files.c = 1;
		files.bytes = count_bytes(path);
	}
	if (p[1] == 'h' && p[2] == '\0') {
		files.h = 1;
		files.bytes = count_bytes(path);
	}
	return files;
}

Files_count
readdirectory(char *path)
{
	DIR *d;
	struct dirent *ent;
	Files_count files;

	files.h = 0;
	files.c = 0;
	files.bytes = 0;

	char realpath[Pathsize];

	d = opendir(path);
	if (d == NULL) {
		err(EXIT_FAILURE, "error: open dir failed: %s", path);
	}

	while ((ent = readdir(d)) != NULL) {
		extractpath(realpath, path, ent->d_name);
		if (ent->d_name[0] != '.') {
			if (ent->d_type == DT_REG) {
				if (strrchr(ent->d_name, '.')) {
					files =
					    terms_sum(readfiles(realpath),
						      files);
				}
			} else if (ent->d_type == DT_DIR) {
				files =
				    terms_sum(readdirectory(realpath), files);
			}
		}
	}
	closedir(d);
	return files;
}

void
checkbadcharacter(char *path)
{
	int i;

	for (i = 0; i < strlen(path); i++) {
		if (path[i] == '\n') {
			path[i] = '\0';
		}
	}
}

void
printcount(Files_count files, char *path)
{
	printf("%s\t%d\t%d\t%ld\n", path, files.c, files.h, files.bytes);
}

int
main(int argc, char *argv[])
{
	if (argc != 1) {
		errx(EXIT_FAILURE, "usage: %s paths directories in stdin",
		     argv[0]);
	}

	char path[Pathsize];
	char pwd[Pathsize];
	char realpath[Pathsize];

	while (fgets(path, Pathsize, stdin) != NULL) {
		if (path[0] != '/') {
			getcwd(pwd, Pathsize);
			checkbadcharacter(path);
			extractpath(realpath, pwd, path);
			printcount(readdirectory(realpath), path);
		} else {
			checkbadcharacter(path);
			printcount(readdirectory(path), path);
		}
	}
	exit(EXIT_SUCCESS);
}
