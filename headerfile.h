
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>

#define ARGMAX 1024

int builtin_cd(char **args);
int builtin_pwd(char **args);
int builtin_echo(char **args);
int builtin_exit(char **args);
int builtin_quit(char **args);
int builtin_pinfo(char **args);
int builtin_ls(char **args);
int builtin_remindme(char **args);
int builtin_clock(char **args);
int builtin_setenv(char **args);
int builtin_unsetenv(char **args);
int builtin_jobs(char **args);
int builtin_kjob(char **args);
int builtin_fg(char **args);
int builtin_bg(char **args);
int builtin_overkill(char **args);
char **truncate_path(char **args);
char **separate_arguments(char *command);
char **separate_commands(char *line);
char *readLine(void);
char *getpath(char *curr_dir);
void prompt(void);
int launch(char **args);
void check_if_builtin(char **args);
void check_status(void);
void sighandler(int signum);

char *root;
char *relative_path;
int chiild;

