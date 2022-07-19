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
#include <assert.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

typedef struct Properties {
	char *permissions;
	long link_count;
	char *owner;
	char *group;
	long size;
	char *tinfo;
	char *name;
} properties;

int getFileInfo(char *name, struct Properties *prop){
	struct tm *timeinfo;
	char buffer[80];
	struct stat st;
	stat(name, &st);
	
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
	g = getgrgid(st.st_gid);
	prop->group = malloc(sizeof(g->gr_name));
	strcpy(prop->group, g->gr_name);

	//size
	prop->size = st.st_size;

	//time of last modification
	timeinfo = localtime(&st.st_mtime);
	strftime(buffer, 80, "%b %d %R", timeinfo);
	prop->tinfo = malloc(sizeof(char)*sizeof(buffer));
	strcpy(prop->tinfo, buffer);	

	//name
	prop->name = name;

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
	return;
}

int main(int argc, char *argv[]){
    DIR *d;
	FILE *f;
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

	if(NULL == (d = opendir(pathname))){
		//can't open directory
		//it may be regular file
		if(0 > (stat(pathname, &st))){
			printf("Can't open a file or a directory\n");
			return(0);
		} else {
			printf("it's a file!\n");
			
 			printf("%ld\n", st.st_size);
		}
	}
        //d = opendir("/home/medalex/test_code/myls/test_folder");
	/*if((d = opendir("./test_folder")) == NULL){
		printf("Error");
		exit(1);
	}*/
	
	
	struct Properties *files = malloc(sizeof(properties));
	int counter = 0;
    while((dir = readdir(d)) != NULL){
		if(dir->d_name[0] == '.'){
			continue; //no hidden files in 'ls -l' mode.
		}	
		struct Properties prop;
		getFileInfo(dir->d_name, &prop);
		counter = counter + 1;
		files = realloc(files, sizeof(properties)*counter);
		files[counter-1] = prop;	
    }
    closedir(d);
	printFilesProrepties(files, counter);
    return(0);
 
}
