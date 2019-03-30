#include "headerfile.h"


int builtin_pinfo(char **args)
{
	char * path ="/proc/";
	char * newp = malloc(sizeof(char)*ARGMAX);
	if(args[1]==NULL)
		sprintf(newp,"%sself",path);
	else
		sprintf(newp,"%s%s",path,args[1]);
	char *stats = malloc(sizeof(char) * ARGMAX);
	sprintf(stats,"%s/stat",newp);
	FILE* f = fopen(stats ,"r");
	if(f==NULL)
	{
		perror("ERROR");
		return (EXIT_FAILURE);
	}
	int pid;
	char stat,residue[30];
	fscanf(f,"%d",&pid);
	fscanf(f,"%s",residue);
	fscanf(f," %c",&stat);
	printf("Process Id --> %d\n",pid);
	printf("Process Status --> %c\n",stat);
	fclose(f);
	char *mem_stats = malloc(sizeof(char)*ARGMAX);
	sprintf(mem_stats,"%s/statm",newp);
	f = fopen(mem_stats,"r");
	if(f==NULL)
	{
		perror("ERROR");
		return (EXIT_FAILURE);
	}
	int size;
	fscanf(f,"%d",&size);
	printf("-- %d{Virtual Memory}\n",size);
	fclose(f);
	char *exe_path = malloc(sizeof(char)*ARGMAX);
	sprintf(exe_path,"%s/exe",newp);
	char buf[ARGMAX]={0};
	int flag=0;
	if(readlink(exe_path,buf,sizeof(buf))==-1)
	{
		perror("readlink");
		return (EXIT_FAILURE);
	}
	if(strlen(buf)<strlen(root))
	{
		printf("Executable Path -- %s\n",buf);
		return 1;
	}
	for(int i=0;i<strlen(root);i++)
	{
		if(root[i]!=buf[i])
		{
			flag=1;
			break;
		}
	}
	if(flag)
		printf("Executable Path -- %s\n",buf);
	else
	{
		printf("Executable Path -- ~");
		printf("%s\n",buf+strlen(root));
	}
	return 1;
}
