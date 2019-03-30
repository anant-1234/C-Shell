#include "headerfile.h"

char *builtin_functions[]= {"cd","pwd","echo","exit","ls","pinfo","remindme","clock","setenv","unsetenv","jobs","kjob","fg","overkill","quit","bg"};
int (*function_pointers[])(char **) = {&builtin_cd, &builtin_pwd, &builtin_echo,&builtin_exit,&builtin_ls,&builtin_pinfo,&builtin_remindme,&builtin_clock,&builtin_setenv,&builtin_unsetenv,&builtin_jobs,&builtin_kjob,&builtin_fg,&builtin_overkill,&builtin_quit,&builtin_bg};
char *bg_processes[1000000];
int proc_status[1000000];
int fg_pid,global_child,to_kill;

int file_exist (char *filename)
{
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}

int min(int a,int b)
{
	return (a<b)?a:b;
}

int if_builtin(char **args)
{
	int size = (int)sizeof(builtin_functions)/(int)sizeof(char*);
	for(int i=0;i<size;i++)
	{
		if (!(strcmp(builtin_functions[i],args[0])))
		{
			(*function_pointers[i])(args);
			return 1;
		}
	}
	return 0;
}
int redirect(char **args,int redirect_flag,int end)
{
	int pid = fork();
	char infile[ARGMAX],outfile[ARGMAX];
	int in_flag=0,out_flag=0,append_flag=0;
	if(pid<0)
	{
		perror("error");
		return 0;
	}
	else if(pid==0)
	{
		int y,limit;
		for(y=0;args[y]!=NULL;y++);
		limit=y;
		for(int i=1;args[i]!=NULL;i++)
		{
			if(args[i-1][0]=='<')
			{
				limit=min(limit,i-1);
				sprintf(infile,"%s",args[i]);
				in_flag=1;
			}
			else if(strcmp(args[i-1],">>") == 0)
			{
				limit=min(limit,i-1);
				sprintf(outfile,"%s",args[i]);
				append_flag=1;
				out_flag=1;
			}
			else if(args[i-1][0]=='>')
			{
				limit=min(limit,i-1);
				sprintf(outfile,"%s",args[i]);
				out_flag=1;
			}
		}
		args[limit]=NULL;
		if(in_flag)
		{
			if(file_exist(infile))
			{
				int fd = open(infile,O_RDONLY);
				if(fd == -1)
				{
					printf("Error in opening the input file\n");
					exit(EXIT_FAILURE);	
				}
				if(dup2(fd,0)==-1)
				{
					perror("dup2 :");
					exit(EXIT_FAILURE);
				}
			}
			else
			{
				printf("Input file doesn't exist\n");
				exit(EXIT_FAILURE);
			}
		}
		if(out_flag)
		{
			if(!file_exist(outfile))
			{
				int fd = open(outfile,O_WRONLY | O_CREAT ,0644);
				if(fd == -1)
				{
					printf("Error in creating the file\n");
					exit(EXIT_FAILURE);
				}
			}
			int fd;
			if(append_flag)
				fd = open(outfile,O_WRONLY | O_APPEND);
			else
				fd = open(outfile,O_WRONLY);
			
			dup2(fd,1);
		}
		if(redirect_flag == 1)
		{
			dup2(end,0);
		}	
		else if(redirect_flag == 2)
		{
			dup2(end,1);
		}
		if(if_builtin(args))
			exit(EXIT_SUCCESS);
		if(execvp(args[0],args)==-1)
		{
			perror("error :");
			exit(EXIT_FAILURE);
		}
		exit(EXIT_SUCCESS);
	}
	else
	{
		wait(NULL);
		return 1;
	}	
}
int create_process(int input,int output,char **args)
{
	int pid = fork();
	if(!pid)
	{
		if(input!=0){
			dup2(input,0);
			close(input);
		}
		if(output!=1)
		{
			dup2(output,1);
			close(output);
		}

		if(execvp(args[0],args)==-1)
		{
			printf("Command not found \n");
			exit(EXIT_FAILURE);
		}
		exit(EXIT_SUCCESS);
	}
	else
	{
		wait(NULL);
	}
	return 1;
}

int pipe_creator(char ** args)
{
	int in,fd[2],start;
	in = 0;
	int idx=0;
	char **cmd_args = malloc(sizeof(char *) * ARGMAX);
	for(int i=0;args[i]!=NULL;i++)
	{
		if(strcmp(args[i],"|")==0)
		{
			start = i;
			break;
		}
		cmd_args[idx++]=args[i];
	}
	cmd_args[idx]=NULL;
	pipe(fd);
	redirect(cmd_args,2,fd[1]);
	close(fd[1]);
	in = fd[0];

	for(int i=0;i<idx;i++)
		cmd_args[i]=NULL;
	
	idx = 0 ;
	for(int i = start+1;args[i]!=NULL;i++)
	{
		if(strcmp(args[i],"|")==0)
		{
			cmd_args[idx]=NULL;
			if(pipe(fd)==-1)
			{
				perror("pipe :");
				return 0;
			}
			create_process(in,fd[1],cmd_args);
			close(fd[1]);
			in = fd[0];
			for(int j=0;j<idx;j++)
				cmd_args[j]=NULL;
			idx=0;
		}
		else
			cmd_args[idx++]=args[i];
	}
	cmd_args[idx]=NULL;
	redirect(cmd_args,1,in);
		
	return 1;
}

void sigzzhandler(int signum)
{
	proc_status[fg_pid]=1;
	// printf("%s\n",bg_processes[fg_pid]);
	printf("\nStopped : [%d] \n",fg_pid);
	kill(fg_pid,SIGTSTP);
	fflush(stdout);
	return 1;
}

int launch(char **args)
{
	int bg_flag = 0,redirect_flag=0,piped_flag=0;
	if(args[0]==NULL)
	{
		perror("bash");
		return (EXIT_FAILURE);
	}
	for(int i=0;args[i]!=NULL;i++)
	{
		if(args[i][0]=='<' || args[i][0]=='>' || strcmp(args[i],">>")==0)
		{
			redirect_flag = 1;
		}
		if(strcmp(args[i],"|")==0)
			piped_flag=1;
	}
	if(piped_flag)
	{
		pipe_creator(args);
		return 1;
	}
	else if(redirect_flag)
	{
		redirect(args,0,2);
		return 1;
	}
	for(int i=0;args[i]!=NULL;i++)
	{
		if(strcmp(args[i],"&")==0)
		{
			bg_flag=1;
			args[i]=NULL;
			break;
		}
	}
	int status;
	int pid = fork();
	if(pid < 0)
	{
		perror("bash");
		return EXIT_FAILURE;
	}
	else if(!pid)
	{
		if(bg_flag==1)
		{
			if (strcmp(args[0],"vim")==0 || strcmp(args[0],"vi")==0)
			{	
				exit(EXIT_SUCCESS);	
			}
			else
			{
				setpgid(0,0);
			}
		}
		if(execvp(args[0],args) == -1)
		{
			printf("Command not found\n");
			exit(EXIT_FAILURE);
		}
		
		return 0;
	}
	else if(bg_flag == 0 && pid)
	{
		fg_pid = pid;
		bg_processes[fg_pid]=args[0];
		// printf("%s\n",bg_processes[fg_pid]);
		chiild = pid;
		signal(SIGINT,sighandler);
		signal(SIGTSTP,sigzzhandler);
		waitpid(pid, &status, WUNTRACED);
	}
	else if(pid)
	{
		if(strcmp(args[0],"vi") != 0 && strcmp(args[0],"vim")!=0)
		{
			printf("%s process created with pid = %d\n",args[0],pid);
			bg_processes[pid]=args[0];
			proc_status[pid]=2;
		}
		else
		{
			bg_processes[pid]=args[0];
			proc_status[pid]=1;
		}
	}
	return 0;
}

void check_if_builtin(char **args)
{
	int flag = 0;
	if(args[0] == NULL)
		return (EXIT_FAILURE);
	for(int i=0;args[i]!=NULL;i++)
	{
		if(strcmp(args[i],">")==0 || strcmp(args[i],"<")==0 || strcmp(args[i],">>")==0 || strcmp(args[i],"|")==0)
			flag=1;
	}
	int size = (int)sizeof(builtin_functions)/(int)sizeof(char*);
	if(!flag)
	{
		for(int i=0;i<size;i++)
		{
			if (!(strcmp(builtin_functions[i],args[0])))
			{
				(*function_pointers[i])(args);
				return ;
			}
		}
	// printf("e/ntered\n");
	}
	launch(args);
	return ;

}

void check_status(void)
{
	int status,check = waitpid(-1, &status, WNOHANG);
	if(check > 0)
	{
		if(WIFEXITED(status)!=0)
		{
			printf("Process %s with pid %d exited normally\n",bg_processes[check],check);
			proc_status[check] = 0;
			bg_processes[check] = NULL;
		}
		else
		{
			int exit_status = WEXITSTATUS(status);
			printf("Process with pid %d exited with exit status : %d\n",check,exit_status);
		}
	}
	return ;
}

int builtin_jobs(char **args)
{
	if(args[1]!=NULL)
	{
		printf("Insuffecient no of arguments\n");
	}
	int counter = 1;
	for(int i=0;i<1000000;i++)
	{
		if(bg_processes[i]!=NULL)
		{
			// printf("%d %s\n",i,bg_processes[i]);
			char * path ="/proc/";
			char * newp = malloc(sizeof(char)*ARGMAX);
			sprintf(newp,"%s%d",path,i);
			char * stats = malloc(sizeof(char)*ARGMAX);
			sprintf(stats,"%s/stat",newp);
			FILE *f =fopen(stats,"r");

			if(f==NULL)
			{
				return 0;
			}
			int pid;
			char stat,residue[30];
			fscanf(f,"%d",&pid);
			fscanf(f,"%s",residue);
			fscanf(f," %c",&stat);
			if(stat=='R' || stat=='S')
				proc_status[i]=2;
			else
				proc_status[i]=1;
			printf("[%d]\t",counter);
			if(proc_status[i]==1)
				printf("Stopped\t\t");
			else
				printf("Running\t\t");
			printf("%s[%d]\n",bg_processes[i],i);
			counter++;
		}
	} 
}
int builtin_kjob(char **args)
{
	int pid,proc_flag=0;
	if(args[1]==NULL || args[2]==NULL)
	{
		printf("Insuffecient arguments\n");
		return 0;
	}
	int counter = 1;
	for(int i = 0;i<1000000;i++)
	{
		if(proc_status[i]!=0)
		{
			if(counter == atoi(args[1]))
			{
				pid = i;
				proc_flag = 1;
				break;
			}
			counter++;
		}
	}
	if(proc_flag == 0)
	{
		printf("No process exists with this process ID\n");
		return 0;
	}
	if(kill(pid,atoi(args[2]))==-1)
	{
		perror("signal : ");
		return 0;
	}
	if(atoi(args[2]) == 9)
	{
		proc_status[pid] = 0;
		bg_processes[pid] = NULL;
	}
	return 1;
}
int builtin_fg(char **args)
{
	int done = 0;
	if(args[1]==NULL)
	{
		printf("Insuffecient arguments\n");
		return 0;
	}
	int counter = 1;
	for(int i=0;i<1000000;i++)
	{
		
		if(proc_status[i]!=0 && counter == atoi(args[1]))
		{
			int status;
			kill(i,SIGCONT);
			fg_pid = i;
			chiild = i;
			signal(SIGTSTP,sigzzhandler);
			waitpid(i,&status,WUNTRACED);
			if(WIFEXITED(status) || WIFSIGNALED(status))
			{
				bg_processes[i] = NULL;
				proc_status[i] = 0;
			}
			done = 1;
			break;
		}
		else if(proc_status[i]!=0)
			counter++;
	}
	if(!done)
	{
		printf("No process exists with this bg_id\n");
		return 0;
	}
	return 1;
}
int builtin_bg(char **args)
{
	int done = 0;
	if(args[1]==NULL)
	{
		printf("Insuffecient arguments\n");
		return 0;
	}
	int counter = 1;
	for(int i=0;i<1000000;i++)
	{
		
		if(proc_status[i]!=0 && counter == atoi(args[1]))
		{
			int status;
			kill(i,SIGCONT);
			proc_status[i] = 2;
			done = 1;
			break;
		}
		else if(proc_status[i]!=0)
			counter++;
	}
	if(!done)
	{
		printf("No process exists with this bg_id\n");
		return 0;
	}
	return 1;
}
int builtin_overkill(char **args)
{
	for(int i=0;i<1000000;i++)
	{
		if(proc_status[i]!=0)
		{
			proc_status[i]=0;
			bg_processes[i]=NULL;
			kill(i,9);
		}
	}
	return 1;
}

