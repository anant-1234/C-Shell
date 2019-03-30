#include "headerfile.h"


void print_permissions(struct stat buf)
{
	if(S_ISDIR(buf.st_mode))printf("d");
	else if(S_ISLNK(buf.st_mode))printf("l");
	else printf("-");
	printf( (buf.st_mode & S_IRUSR) ? "r" : "-");
	printf( (buf.st_mode & S_IWUSR) ? "w" : "-");
	printf( (buf.st_mode & S_IXUSR) ? "x" : "-");
	printf( (buf.st_mode & S_IRGRP) ? "r" : "-");
	printf( (buf.st_mode & S_IWGRP) ? "w" : "-");
	printf( (buf.st_mode & S_IXGRP) ? "x" : "-");
	printf( (buf.st_mode & S_IROTH) ? "r" : "-");
	printf( (buf.st_mode & S_IWOTH) ? "w" : "-");
	printf( (buf.st_mode & S_IXOTH) ? "x" : "-");
	return ;
}
int builtin_ls(char **args)
{
	DIR *directory;
	struct dirent *dir;
	int flag_l = 0,flag_a = 0;
	char **directories = malloc(sizeof(char*) * ARGMAX);
	int idx = 0;
	for (int i=1;args[i]!=NULL;i++)
	{
		if(args[i][0]=='-')
		{
			if(args[i][1]=='\0')
			{
				perror("bash");
				return (EXIT_FAILURE);
			}
			else if(args[i][1]=='l')
			{
				flag_l = 1;
				if(args[i][2]=='a')
					flag_a = 1;
			}
			else if(args[i][1]=='a')
			{
				flag_a = 1;
				if(args[i][2]=='l')
					flag_l = 1;
			}
		}
		else 
			directories[idx++]=args[i];
	}

	if(idx == 0)
	{
		char * s=".";
		directories[idx++]=s;
	}
	directories[idx]=NULL;
	for(int i=0;directories[i]!=NULL;i++)
	{
		directory = opendir(directories[i]);
		printf("%s\n",directories[i]);

		if(!directory)
		{
			printf("No such directory exists\n");
		}
		else 
		{
			dir = readdir(directory);
			struct stat buff;
			while(dir!=NULL)
			{
				struct stat buff;
				if(!flag_a && dir->d_name[0]=='.')
				{
					dir = readdir(directory);
					continue;
				}
				if(flag_l)
				{
					char* path = malloc(sizeof(char)*ARGMAX);
					sprintf(path,"%s%s%s",directories[i],"/",dir->d_name);
					stat(path,&buff);
					
					char *mod_time = ctime(&buff.st_mtime);
					mod_time[strlen(mod_time)-1]='\0';
					char* user_id = getpwuid(buff.st_uid)->pw_name;
					char* group_id = getgrgid(buff.st_gid)->gr_name;
					long long int size = (long long int)buff.st_size;
					
					print_permissions(buff);
					printf("\t%d ",(int)buff.st_nlink);
					printf("\t%s ",user_id);
					printf("\t%s ",group_id);
					printf("\t%lld ",size);
					printf("\t%s ",mod_time);
					printf("\t");
				}
				printf("%s ",dir->d_name);
				printf("\n");
				dir = readdir(directory);
			}
			closedir(directory);
		}
		printf("\n");
	}
	return 0;
}