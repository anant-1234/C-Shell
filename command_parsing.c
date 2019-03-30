#include "headerfile.h"


char** truncate_path(char **args)
{
	int idx = 0;
	char **args2 = malloc(sizeof(char*) *ARGMAX);
	for(int i=0;args[i]!=NULL;i++)
	{
		if(args[i][0]=='~')
		{
			char *path = malloc(sizeof(char)*ARGMAX);
			strcpy(path,root);
			strcat(path,args[i]+1);
			args2[idx++]=path;
		}
		else
		{
			args2[idx++]=args[i];
		}
	}
	args2[idx]=NULL;
	return args2;
}
char **separate_arguments(char *command)
{
	char delim[4]={' ','\t','\n','\a'};
	int buf_size = ARGMAX,idx=0;
	char **args = malloc(sizeof(char*) *buf_size);
	char *token;
	token = strtok(command,delim);
	while(token!=NULL)
	{
		args[idx++]=token;
		if(idx>=buf_size)
		{
			buf_size+=buf_size;
			args = realloc(args,sizeof(char*) * buf_size);
			if(!args)
			{
				write(2,"Allocation Error\n",sizeof("Allocation Error\n"));
				exit (EXIT_FAILURE);
			}
		}
		token = strtok(NULL,delim);
	} 
	args[idx]=NULL;
	args = truncate_path(args);
	return args;
}
char **separate_commands(char *line)
{
	char delim[2]={';'};
	int buf_size = ARGMAX,idx=0;
	char ** commands = malloc(sizeof(char*) * buf_size);
	char * token;
	token = strtok(line,delim);
	while(token!=NULL)
	{
		commands[idx++]=token;
		if(idx>=buf_size)
		{
			buf_size+=buf_size;
			commands = realloc(commands,sizeof(char*) * buf_size);
			if(!commands)
			{
				write(2,"Allocation Error\n",sizeof("Allocation Error\n"));
				exit (EXIT_FAILURE);
			}
		}
		token = strtok(NULL,delim);
	}
	commands[idx]=NULL;
	return commands;
}
char *readLine(void)
{
	ssize_t line_size = 0;
	char * line =NULL;
	getline(&line,&line_size,stdin);
	return line;
}