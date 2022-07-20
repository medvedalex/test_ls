/*
 * This is a test task.
 * Analog of ls utility, realized functionality of 'ls -l'
 *
 * TODO:
 * Fix bug: works only in current and parent directories
 * LS_COLORS to define colors of files and folders.
 * check MAX_PATH
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

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define DPRINT 0

typedef struct Properties {
	char *permissions;
	long link_count;
	char *owner;
	char *group;
	long size;
	char *tinfo;
	char *name;
} properties;

int getFileInfo(char *pathname, char *name, struct Properties *prop){
	if(DPRINT) printf("getFileInfo begin\n");
	if(DPRINT) printf("pathname: %s\n", pathname);
	if(DPRINT) printf("name: %s\n", name);
	struct tm *timeinfo;
	char buffer[80] = {0};
	struct stat st;

	if(strlen(name) > 0){
		strcat(buffer, pathname);
		if(pathname[strlen(pathname)] != '/'){
			strcat(buffer, "/");
		}
		strcat(buffer, name);
		//name
		prop->name = name;
	} else {
		prop->name = pathname;
		strcat(buffer, pathname);
	} 
	stat(buffer, &st);

	
	if(DPRINT) printf("NAME: %s\n", buffer);

	//permissions	
	mode_t perm = st.st_mode;
	prop->permissions = malloc(sizeof(char)*11);
	prop->permissions[0] = (S_ISREG(perm) ? '-' : 'd');
	
	prop->permissions[1] = ((perm & S_IRUSR) ? 'r': '-');
	prop->permissions[2] = ((perm & S_IWUSR) ? 'w': '-');
	prop->permissions[3] = ((perm & S_IXUSR) ? 'x': '-');
                                                           
	prop->permissions[4] = ((perm & S_IRGRP) ? 'r': '-');
	prop->permissions[5] = ((perm & S_IWGRP) ? 'w': '-');
	prop->permissions[6] = ((perm & S_IXGRP) ? 'x': '-');
                                                           
	prop->permissions[7] = ((perm & S_IROTH) ? 'r': '-');
	prop->permissions[8] = ((perm & S_IWOTH) ? 'w': '-');
	prop->permissions[9] = ((perm & S_IXOTH) ? 'x': '-');
	
	prop->permissions[10] = '\0';

	//link count
	prop->link_count = st.st_nlink;

	//owner name
	struct passwd *pwd;
	pwd = getpwuid(st.st_uid);
	prop->owner = malloc(sizeof(pwd->pw_name));
	strcpy(prop->owner, pwd->pw_name);

	//owner group
	struct group *g;
	if(DPRINT) printf("group id %d, name is %s\n", st.st_gid, name);
	g = getgrgid(st.st_gid);
	if(!g){
		prop->group = malloc(sizeof(char));
		strcpy(prop->group, "-"); //temporary patch
	} else {
		prop->group = malloc(sizeof(g->gr_name));
		strcpy(prop->group, g->gr_name);
	}
	if(DPRINT) printf("group\n");

	//size
	prop->size = st.st_size;

	//time of last modification
	timeinfo = localtime(&st.st_mtime);
	strftime(buffer, 80, "%b %d %R", timeinfo);
	prop->tinfo = malloc(sizeof(char)*sizeof(buffer));
	strcpy(prop->tinfo, buffer);	


	if(DPRINT) printf("getFileInfo end\n");
}

int len(long a){
	int i = 0;
	for(;a>=1;i++){
		a/=10;
	} 
	return i;
}

//print with alignment
void printFilesProrepties(struct Properties *prop_array, int counter){
	if(DPRINT) printf("printFilesProrepties begin\n");
	//get maximum lenght of each parameter
	int max_link_count = 0,  max_owner_name = 0, max_group_name = 0, max_size = 0;
	for(int i = 0; i < counter; i++){
		struct Properties prop = prop_array[i];	
		max_link_count = MAX(max_link_count, len(prop.link_count));
		max_owner_name = MAX(max_owner_name, strlen(prop.owner));
		max_group_name = MAX(max_group_name, strlen(prop.group));
		max_size = MAX(max_size, len(prop.size));
			
	}
	for(int i = 0; i < counter; i++){
		struct Properties prop = prop_array[i];	
		printf("%s %*ld %*s %*s %*ld %s %s\n", 	prop.permissions,\
												max_link_count, prop.link_count, \
												max_owner_name, prop.owner,\
												max_group_name, prop.group,\
												max_size, prop.size, \
												prop.tinfo, \
												prop.name);
	}
	if(DPRINT) printf("printFilesProrepties end\n");
	return;
}

int main(int argc, char *argv[]){
    DIR *d;
	FILE *f;
    struct dirent *dir;
    struct stat st;
	struct tm *timeinfo;
	char pathname[80];

	struct Properties *files = malloc(sizeof(properties));
	int counter = 0;
	
	if(DPRINT) printf("main\n");

	if(argc < 2){
		strcpy(pathname, ".");
	} else {
		strcpy(pathname, argv[1]);
	}

	if(DPRINT) printf("main 1\n");
	if(NULL == (d = opendir(pathname))){
		//can't open directory
		//it may be regular file
		if(DPRINT) printf("opendir is NULL\n");
		if(0 > (stat(pathname, &st))){
			printf("Can't open a file or a directory\n");
			return(0);
		} else {
			//it's a regular file
			if(DPRINT) printf("regular file\n");
			struct Properties prop;
			getFileInfo(pathname, "", &prop);
			counter = counter + 1;
			files[counter-1] = prop;	
			printFilesProrepties(files, counter);
			return(0);
		}
	}

	if(DPRINT) printf("main 2\n");

    while((dir = readdir(d)) != NULL){
		if(dir->d_name[0] == '.'){
			continue; //no hidden files in 'ls -l' mode.
		}	
		struct Properties prop;
		getFileInfo(pathname, dir->d_name, &prop);
		counter = counter + 1;
		files = realloc(files, sizeof(properties)*counter);
		files[counter-1] = prop;	
    }
	if(DPRINT) printf("main 3\n");
    closedir(d);
	if(DPRINT) printf("main 4\n");
	printFilesProrepties(files, counter);
	if(DPRINT) printf("main 5\n");
    return(0);
 
}
