#include <stdio.h>
#include <dirent.h>

int main(int argc,char** argv){
	DIR *streamp;
	struct dirent *dep;
	streamp=opendir(argv[1]);

	while((dep=readdir(streamp))!=NULL){
		printf("%s\n",dep->d_name);
	}
	closedir(streamp);
	return 0;
	
}
