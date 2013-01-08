#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>


#ifndef XSSH_H
#define XSSH_H


/* exit status of shell (quit arg) */
int exit_status;    

/* exit status of last foregound command ($?) */
int cmd_exit_status = 0;

/* PID of last background command ($!) */
pid_t background_pid = 0;

/* Flag to make the shell die */
char exit_shell;

/* Display command lines after variable substitution */
int show_cmds;

/* Stores debug level. Affects verbosity of shell. */
int debug_level = 0;

/* Current output stream. Defaults to stdout. Used in redirection. */
char* outFileName;
char* inFileName;

/* Flag denoting whether a process should be executed in the background. */
int background = 0;

/* Flag denoting whether input redirection is active. */
int redirect_in = 0;

/* Flag for -x. */
int displayCommand = 0;

/* Flag for pipeline*/
int pipeFlag = 0;

/* The environment */
extern char **environ;
#endif

int echo(int argc, char* argv[]);
int wait_cmd(pid_t pid);
int setVariable(int argc, char *argv[]);
int unset_cmd(int argc, char* argv[]);
void initializeEnvironment();
int run_script(FILE *script);
char **parse_cmd(char *cmd, int *size); 
int run_cmd(int argc, char *argv[]);
int run_external_cmd(char *argv[]);
pid_t run_bg_cmd(int argc, char *argv[]);
int isValidVariable(char* argv);
char** varSubsitute(char** argv, int argc);
int changeDirectory(int argc, char* argv[]);
int run_pipe_cmd(int argc, char *argv[]);

void debug(char *msg);
