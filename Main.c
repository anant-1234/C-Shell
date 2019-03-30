#include "headerfile.h"

void sighandler(int signum)
{
	
	if(chiild>0)
	{
		kill(chiild,signum);
	}
	chiild = 0;
	return ;
}
void sigshandler(int signum)
{
	return ;
}
int main(int argc,char **argv[])
{
	int i;
	root = getenv("PWD");
	while(1)
	{
		check_status();
		prompt();
		char cmd[ARGMAX];
		signal(SIGINT,sighandler);
		signal(SIGTSTP,sigshandler);
		char *line = readLine();
		char **commands = separate_commands(line);
		for(i=0;commands[i]!='\0' && commands[i][0]!='\n';i++)
		{
			char **args = separate_arguments(commands[i]);
			check_if_builtin(args);
		}		
		
	}

}