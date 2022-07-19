#include <dirent.h>
#include <stdio.h>
#include <string.h>

int main(int argc, void **argv){
	DIR *d;
	char path[80];

	if (argc < 2){
		strcpy(path, ".");
	} else {
		strcpy(path, argv[1]);
	}
	d = opendir(path);
	if(d){
		printf("Success! %s\n", path);
	} else {
		printf("NULL\n");
	}
	closedir(d);
	return(0);
}
