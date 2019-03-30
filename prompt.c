#include "headerfile.h"

char *getpath(char *curr_dir)
{
	int id,f=0;
	if(strlen(curr_dir)<strlen(root))
	{
		return curr_dir;
	}
	else
	{
		for (int i=0;i<strlen(root);i++)
		{
			if(curr_dir[i]!=root[i])
			{
				f=1;
				id=i;
				break;
			}
		}
		if(f)
			return curr_dir;

		else
		{
			relative_path = malloc(sizeof(char) * ARGMAX);
			int size=1;
			relative_path[0]='~';
			for(int j=strlen(root);j<strlen(curr_dir);j++)
				relative_path[size++]=curr_dir[j];
			relative_path[size]='\0';
			return relative_path;
		}
	}
}

void prompt(void)
{
	char *user = getenv("USER");

	char hostname[ARGMAX];
	hostname[ARGMAX-1]='\0';
    gethostname(hostname,ARGMAX-2);
    
	char curr_dir[ARGMAX];
	curr_dir[ARGMAX-1]='\0';
	getcwd(curr_dir,ARGMAX-2);
	char *path = getpath(curr_dir);
	printf("%s@%s:%s>",user,hostname,path);
	free(relative_path);

}