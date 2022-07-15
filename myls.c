/*
 * This is a test task.
 * Analog of ls utility, realized functionality of 'ls -l'
 *
 * TODO:
 * Fix bug: works only in current and parent directories
 * LS_COLORS to define colors of files and folders.
 *
 *
 */

#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[]){
        //DIR *d;
        struct dirent *dir;
        struct stat st;
	struct tm *timeinfo;
	char buffer[80];
	char pathname[80];

	if(argc < 2){
		strcpy(pathname, ".");
	} else {
		strcpy(pathname, argv[1]);
	}
	//printf("%s\n", pathname);
	DIR *d = opendir(pathname);
	assert(d != NULL);
        //d = opendir("/home/medalex/test_code/myls/test_folder");
	/*if((d = opendir("./test_folder")) == NULL){
		printf("Error");
		exit(1);
	}*/
	
        if(d){
                while((dir = readdir(d)) != NULL){
                        stat(dir->d_name, &st);
			//ls output:
			//- permissions
			//- link count
			//- owner name
			//- owner group
			//- size
			//- time of last modification
			//- name
			
			if(dir->d_name[0] == '.'){
				continue; //no hidden files in 'ls -l' mode.
			}			

			//permissions	
			mode_t perm = st.st_mode;
                        printf(S_ISREG(perm) ? "-" : "d");
			printf((perm & S_IRUSR) ? "r": "-");
			printf((perm & S_IWUSR) ? "w": "-");
			printf((perm & S_IXUSR) ? "x": "-");

			printf((perm & S_IRGRP) ? "r": "-");
			printf((perm & S_IWGRP) ? "w": "-");
			printf((perm & S_IXGRP) ? "x": "-");

			printf((perm & S_IROTH) ? "r": "-");
			printf((perm & S_IWOTH) ? "w": "-");
			printf((perm & S_IXOTH) ? "x": "-");

			//link count
			printf(" %ld", st.st_nlink);

			//owner name
			struct passwd *pwd;
			pwd = getpwuid(st.st_uid);
			printf(" %s", pwd->pw_name);

			//owner group
			//printf("\t%s", st.st_gid);
			struct group *g;
			g = getgrgid(st.st_gid);
			printf(" %s", g->gr_name);

			//size
			int size = st.st_size;
 			printf(" %d\t", size);

			//time of last modification
			timeinfo = localtime(&st.st_mtime);
			strftime(buffer, 80, "%b %d %R", timeinfo);
			printf("%s", buffer);

			//name
                        printf("\t%s\n", dir->d_name);
                }
                closedir(d);
        }
        return(0);
 
}
