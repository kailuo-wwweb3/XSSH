#include "xssh.h"

/* 
 Team 18: Kai Luo, Andrew Moore and Ziyang Huang.
*/

/* default file descriptors*/
int defout, defin;

/* redirection file descriptors*/
int outfd, infd;

/* child process PID array */
pid_t *child_pids, child_count;
int child_cap;

/* This is where the program begins. */
int
main(int argc, char *argv[])
{	
	/* All variables that we need */
	char *input, **words;
	int i = 0, j = 0, running = 1;
	FILE *inFile = NULL;
	pipeFlag = 0;
	defin = dup(STDIN_FILENO);
	defout = dup(STDOUT_FILENO);
	initializeEnvironment();
	
    if (argc > 1) {
        for (i = 1; i < argc; i ++) {
            if (strcmp(argv[i], "-x") == 0) {
				displayCommand = 1;
            } else if (strcmp(argv[i], "-d") == 0) {
				i++;
				if (i >= argc) { 
					printf("./xssh [-x] [-d level] [file [arg] …]\n");
					running = 0;
                    exit_status = -1;
				} else {
					debug_level = atoi(argv[i]);
				}
            } else {
                if (inFile == NULL) {
                    if ((inFile = fopen(argv[i], "r")) == NULL) {
                        printf("The file name is invalid.\n");
						running = 0;
                        exit_status = -1;
                    } else {
                        /* grab file arguments */	
						exit_status = run_script(inFile);
						fclose(inFile);
                    }
                }
            }
        }
    }
	
	/* Initialize child capacity, child count, and array of child pids */
	child_cap = 1;
	child_count = 0;
	child_pids = malloc(child_cap * sizeof(pid_t));
		
    while (running) {
		i = 0;
		redirect_in = 0;
		pipeFlag = 0;
	
		dup2(defin, STDIN_FILENO);
		dup2(defout, STDOUT_FILENO);
		
        /* get the input from user. */
        input = readline(">> ");
        
        if (strcmp(input, "") == 0) {
        	continue;
        }
        
        add_history(input);
        
		words = parse_cmd(input, &i);
		if (pipeFlag) {
			running = run_pipe_cmd(i, words);
			pipeFlag = 0;
		} else {
			running = run_cmd(i, words);
		}
    }

	for (j = 0; j < i; j++) {
		free(words[i]);
	}
	
	free(words);
	free(input);

	return exit_status;		
}

void initializeEnvironment()
{
	/*
	 *	$$: the PID of this instance of xssh
	 *	$?: the exit status of the last completed foreground command
	 *	$!: the PID of the last dispatched background command	
	 */
	
	char instance_pid[50], last_exit_status[50], last_bg_pid[50];
	sprintf(instance_pid, "%d", getpid());
	setenv("$", instance_pid, 1);
	
	sprintf(last_exit_status, "%d", cmd_exit_status);
	setenv("?", last_exit_status, 1);
	
	sprintf(last_bg_pid, "%d", background_pid);
	setenv("!", last_bg_pid, 1);	
}

int run_script(FILE *script)
{
	int scripting = 1;
	int size;
	char **args, cmd[100];
	
	while (scripting && !feof(script)) {
		size = 0;
		redirect_in = 0;
	
		dup2(defin, STDIN_FILENO);
		dup2(defout, STDOUT_FILENO);

        /* get the input from user. */
        fgets(cmd, 100, script);
		
		args = parse_cmd(cmd, &size);
		
		if (pipeFlag) {
			scripting = run_pipe_cmd(size, args);
			pipeFlag = 0;
		} else {
			scripting = run_cmd(size, args);
		}
    }
	return cmd_exit_status;
}

char **parse_cmd(char *cmd, int *size)
{
	int argc = 0;
	char **args = malloc(100 * sizeof(char *));
	char *decommented, *pos;
	int i;
	
	if (cmd[0] == '#') {
		args[0] = NULL;
		argc++;
	} else {
		decommented = strtok(cmd, "#");
		
		pos = strchr(decommented, '|');
		pipeFlag = (pos == NULL) ? 0 : 1;
		
		if (pipeFlag) {
			args[argc] = strtok(decommented, "|");
			while (args[argc] != NULL) {
				args[++argc] = strtok(NULL, "|");
			}
		} else {
			pos = strchr(decommented, '&');
			background = (pos == NULL) ? 0 : 1;
			decommented = strtok(decommented, "&");
		
			/* check redirection */
			inFileName = NULL;
			outFileName = NULL;
		
			pos = strchr(decommented, '>');
			if (pos != NULL) {
				outFileName = strtok(pos + 1, "");
				outfd = open(outFileName, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
				dup2(outfd, STDOUT_FILENO);
				decommented = strtok(decommented, ">");
			} 
		
			pos = strchr(decommented, '<');
			if (pos != NULL) {
				redirect_in = 1;
				inFileName = strtok(pos + 1, " ");
				infd = open(inFileName, O_RDWR | O_CREAT, S_IRWXU);
				dup2(infd, STDIN_FILENO);	
				decommented = strtok(decommented, "<");	
			} 
		
			/* split input into words. */		
			args[argc] = strtok(decommented, " ");
			while (args[argc] != NULL) {
				argc++;
				args[argc] = strtok(NULL, " ");
			}
		}
	}

	*size = argc;
	args = realloc(args, (argc + 1) * sizeof(char *));
	args[argc] = NULL;
	
	if (args[0] == NULL) {
		return args;
	}
	
	args = varSubsitute(args, argc);
	
	if (displayCommand) {
		printf("Command: ");
		for (i = 0; i < argc; i ++) {
			printf("%s ", args[i]);
		}
		printf("\n");
	}

	return args;
}

char** varSubsitute(char** argv, int argc) 
{
	int i;
	char* ptr;
	for (i = 0; i < argc; i ++) {
		if (argv[i][0] == '$') {
			ptr = argv[i];
			ptr ++;				
			if ((argv[i] = getenv(ptr)) == NULL) {
				printf("There is no environment value for the variable %s.\n", ptr);
			}
		}
	}
	
	return argv;
}


int run_cmd(int argc, char *argv[]) 
{	
	char last_bg_pid[50], msg[50];
	
	if (argv[0] == NULL) {
		/* ignore the blank line. */
		return 1;
    } else if (strcmp(argv[0], "quit") == 0) { 
		/* quit the shell. */
        if (argv[1] != NULL) {
			if (!isalpha(*argv[1])){ 
				cmd_exit_status = atoi(argv[1]);
			} 
		}
		
		exit_status = cmd_exit_status;
		return 0;
    } else if (strcmp(argv[0], "echo") == 0) { 
		/* echo: print all the words after the echo command. */
        cmd_exit_status = echo(argc, argv);
       
    } else if ((strcmp(argv[0], "cd") == 0) || (strcmp(argv[0], "chdir") == 0)) {
	 	/* change the working directory.*/
		
		cmd_exit_status = changeDirectory(argc, argv);

	} else if (strcmp(argv[0], "set") == 0) {
        
        /* Set variable by the given value.*/
        cmd_exit_status = setVariable(argc, argv);
    } else if (strcmp(argv[0], "wait") == 0) { 
		if (argv[1] == NULL) {
			printf("wait <PID>\n");
			cmd_exit_status = 1;
		} else {
        	cmd_exit_status = wait_cmd((pid_t) atoi(argv[1]));
		}
	} else if (strcmp(argv[0], "unset") == 0) { 
		/* do unset */
		cmd_exit_status = unset_cmd(argc, argv);
	} else {
		/* Command not found. Look for external command to run. */
		cmd_exit_status = run_external_cmd(argv);
	}
	
	sprintf(msg, "Command exited with exit status %d", cmd_exit_status);
	debug(msg);
	
	sprintf(last_bg_pid, "%d", background_pid);
	setenv("!", last_bg_pid, 1); 
	
	return 1;
}

int run_pipe_cmd(int argc, char* argv[]) {
	int i, size;
	pid_t pid;
	char **command;
	int **pipes;
	pipes = (int**)malloc(argc*sizeof(int*));
	/* initialize pipes */
	for(i=0;i<argc;i++)
	{
	    pipes[i] = (int*)malloc(2*sizeof(int));
	}
	
	/* Create argc child processes. */
	for (i = 0; i < argc; i ++) {
		if (i < argc - 1) {
			if ((pipe(pipes[i])) < 0) {
				perror("Fail to set up the pipe.");
			}
		}
		
		if((pid = fork()) < 0) {
			perror("Fork failed.");
		}
		
		if (pid == 0) {			
			command = parse_cmd(argv[i], &size);
			
			if (i == 0) {
				close(pipes[i][0]);
				/*dup2(defin, STDIN_FILENO);*/
				dup2(pipes[i][1], STDOUT_FILENO);
				close(pipes[i][1]);
			} else if (i == argc - 1) {
				close(pipes[i - 1][1]);
				/*dup2(defout, STDOUT_FILENO);*/
				dup2(pipes[i - 1][0], STDIN_FILENO);
				close(pipes[i - 1][0]);
			} else {
				close(pipes[i - 1][1]);
				close(pipes[i][0]);
				dup2(pipes[i - 1][0], STDIN_FILENO);
				close(pipes[i - 1][0]);
				dup2(pipes[i][1], STDOUT_FILENO);
				close(pipes[i][1]);
			}
			pipeFlag = 1;
			run_cmd(size, command);
		} else {
			/* parent process*/
				waitpid(pid, NULL, WNOHANG);
			}
		}
	return 1;
}
            
int run_external_cmd(char *argv[])
{
	pid_t child_pid;
	int child_status;
	
	if (pipeFlag) {
		debug("Running pipelined command");
		execvp(argv[0], argv);
		printf("Unknown command\n");
		exit(0);
	} else {
		child_pid = fork();
	
		if(child_pid == 0) {
			execvp(argv[0], argv);
			printf("Unknown command\n");
			exit(0);
		} else {
			if (background) {
				debug("Running command in background");
				waitpid(child_pid, &child_status, WNOHANG);
				background_pid = child_pid;
				child_pids[child_count] = child_pid;
				child_count++;
				if (child_count == child_cap) {
					child_cap *= 2;
					child_pids = realloc(child_pids, child_cap * sizeof(pid_t));
					if (child_pids == NULL) {
						printf("Error reallocating child_pids\n");
						exit(-1);
					}
				}
				printf("[%d]\t%d\n", child_count, child_pid);
			} else {
				debug("Running command in foreground");
				wait(&child_status);
			}
		
			return 0;
		}
	}
	
	return 0;
}

int echo(int argc, char* argv[]) {

	int i;
	for (i = 1; i < argc; i++) {
		if (argv[i] != NULL) {
			printf("%s ", argv[i]);
		}
	}
	printf("\n");

	return 0;
}

int wait_cmd(pid_t pid)
{
	int status, error, i, is_valid_pid;
	if (pid < -1 ){
		
	} else {
		for (i = 0; i < child_count; i++) {
			if (child_pids[i] == pid) {
				is_valid_pid = 1;
				break;
			}
		}
		
		if (is_valid_pid) {
			debug("PID is a child of xssh. xssh will wait on it.");
			error = waitpid(pid, &status, 0);
		
			if (error == -1) {
				perror("waitpid");
			}
			
			is_valid_pid = 0;
			
			for (i = 0; i < child_count; i++) {
				if (child_pids[i] == pid) {
					is_valid_pid = 1;
				}
				if (is_valid_pid && i < (child_count - 1)) {
					child_pids[i] = child_pids[i + 1];
				}
			}
			
			child_count--;
			child_cap = child_count == 0 ? 1 : child_count;
			
			child_pids = realloc(child_pids, child_cap * sizeof(pid_t));
			if (child_pids == NULL) {
				debug("Error reallocating child_pids");
				exit(-1);
			}
		}
		return 1;
	}
	
	return 1;
}

int changeDirectory(int argc, char* argv[]) {
	char* homeDirectory = NULL;
	homeDirectory = getenv("HOME");
	if (argc == 2) {
		/* a path is provided, set the working directory. */
		debug("Changing directory to given path");
		setenv("PWD", argv[1], 1);
	}else if ((homeDirectory != NULL) && (argc == 1)) {
		/* a path is not provided, set the working directory to be home directory.*/
		debug("Changing directory to home directory");
		setenv("PWD", homeDirectory, 1);
	}else if ((homeDirectory == NULL) && (argc == 1)) {
		/* the home directory is not set, set the working directory to be home directory.*/
		debug("Changing directory to root since home directory is undefined"); 
		setenv("PWD", "/", 1);
	}else {
		printf("Incorrect format, should be: %s [path].\n", argv[0]);
		return -1;
	}
	
	chdir(getenv("PWD"));
	return 0;
}


int setVariable(int argc, char* argv[])
{
	 int i = 0;
    /* Set without arguments.*/
    if (argc == 1) {
        while (environ[i]) {
            printf("%s\n", environ[i++]);
        }
    } else if (argc == 2) {
        /*set variable to a empty string*/
        if (isValidVariable(argv[1])) {
            setenv(argv[1], "", 1);
        }else {
            debug("Set failed because of an invalid variable name.");
            return -1;
        }
        debug("Set variable to empty string.");
    } else if (argc == 3) {
        /* insert the variable into environment with the value or reset the variable with the value if it is already in the environment.*/
        if (isValidVariable(argv[1])) {
            setenv(argv[1], argv[2], 1);        
        } else {
        	debug("Set failed because of an invalid variable name.");
            return -1;
        }
        debug("Set variable to the value passed in");
    } else if (argc > 3) {
    	printf("Too many arguments: set [variable [value]]\n");
    	return -1;
    }
    
    return 0;
}
            
int isValidVariable(char *name)
{
	int i;
    
    if ((isdigit(name[1]) || (name[1] == '_'))) {
        printf("The variable name should not begin with a number or the underscore character ([0-9_]).\n");
        return 0;
    }

    for (i = 0; i < strlen(name); i ++) {
        if (!((isupper(name[i])) || (isdigit(name[i])) || (name[i] == '_'))) {
			printf("The variable name should contain only uppercase letters, numbers, or the underscore character ([A-Z0-9_]+).\n");
	        return 0;
	    }
    }
    
    return 1;
}

int unset_cmd(int argc, char* argv[])
{
	int i, error;
	
	if (argc <= 1) {
		printf("Not enough arguments: unset [variable ...]\n");
	}
	
	for (i = 1; i < argc; i++) {
		error = unsetenv(argv[i]);
		if (error == -1) {
			debug("Unset failed because of an error with unsetenv()");
			perror("Unset");
			return 1;
		}
	}
	
	debug("Unset removed definitions for all variables passed in.");	
	
	return 0;
}

void debug(char *msg)
{
	if (debug_level > 0) {
		fprintf(stderr, "%s\n", msg);
	}
}
