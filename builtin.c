#include "headerfile.h"

int flag = 0;
int builtin_exit(char **args)
{
	exit(EXIT_SUCCESS);
}

int builtin_quit(char **args)
{
	exit(EXIT_SUCCESS);
}
int builtin_pwd(char **args)
{
	char *cwd = malloc(sizeof(char)*ARGMAX);
	cwd[ARGMAX-1]='\0';
	getcwd(cwd,ARGMAX-2);
	printf("%s\n",cwd);
	return 0;
}

int builtin_remindme(char **args)
{
	if(args[1]==NULL || args[2]==NULL)
	{
		printf("Insuffecient arguments\n");
		return 0;
	}
	int pid = fork();
	if(pid==0)
	{
		sleep(atoi(args[1]));
		printf("\n");
		printf("Reminder : ");
		for(int i=2;args[i]!=NULL;i++)
			printf("%s ",args[i]);
		printf("\n");
		exit(EXIT_SUCCESS);
	}
	else
		return 1;
}

int builtin_cd(char **args)
{
	if(args[1]==NULL)
	{
		chdir(root);
		return 0;
	}
	if(chdir(args[1])!=-1)
		return 0;
	else
	{
		perror("bash");
		return -1;
	}
	return 0;
}

int builtin_echo(char **args)
{
	int now = 1;
	char * word = args[now++];
	while(word!=NULL)
	{
		printf("%s ",word);
		word = args[now++];
	}
	printf("\n");
	return 0;
}

int sigzhandler(int signum)
{
	printf("\nExited\n");
	fflush(stdout);
	flag = 1;
	return 1;
}

int builtin_clock(char **args)
{
	if(args[1]==NULL || args[2]==NULL || strcmp(args[1],"-t"))
	{
		perror("bash :");
		return EXIT_FAILURE;
	}
	flag=0;
	while(1)
	{
		char *idate =malloc(sizeof(char)*ARGMAX);
		char *itime = malloc(sizeof(char)*ARGMAX);
		signal(SIGTSTP,sigzhandler);
		signal(SIGINT,sigzhandler);
		if(flag)
			return 1;
		sprintf(idate,"/sys/class/rtc/rtc0/date");
		sprintf(itime,"/sys/class/rtc/rtc0/time");
		FILE *f = fopen(idate,"r");
		FILE *f2 = fopen(itime,"r");
		if(f==NULL || f2==NULL)
		{
			perror("bash: ");
			return EXIT_FAILURE;
		}
		char my_date[40],my_time[40];
		fscanf(f,"%s",my_date);
		fscanf(f2,"%s",my_time);
		printf("%s, %s\n",my_date,my_time);
		fclose(f);
		fclose(f2);
		sleep(atoi(args[2]));
	}
	return 1;
}

int builtin_setenv(char **args)
{
	if(args[1]==NULL || (args[2]!=NULL && args[3]!=NULL))
	{
		printf("Wrong number of arguments\n");
		return 0;
	}
	if(args[2]==NULL)
		args[2]="";
	if(setenv(args[1],args[2],1)==-1)
	{
		perror("setenv :");
		return 0;
	}
	return 1;
}

int builtin_unsetenv(char **args)
{
	if(args[1]==NULL)
	{
		printf("Insuffecient arguments\n");
		return 0;
	}
	if(unsetenv(args[1])==-1)
	{
		perror("unsetenv :");
		return 0;
	}
	return 1;
}
