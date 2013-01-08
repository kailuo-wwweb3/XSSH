This is the readme file to show the instructions for the XSSH shell

Team 18
by Kai Luo, Andrew Moore and Ziyang Huang 

Instructions for Building and Running the Shell:

Building:
Type "make"; the shell should compile.
Running: 
Type ./xssh and then the options you desire:
	-d [integer] will tell the shell to output debugging 
	information if integer = 1. if integer = 0, no information
	will be printed.
	-x will tell the shell to display the command line after 
	variable substitution but before it is evaluated
	file <args> will tell the shell to read commands from the
	inputted file; the args will be used as inputs to the script.
The shell will run.

Input Instructions:

1. When typing the instruction to run a background process, the ampersand (&) must come 
immediately after the command.
2. When using redirection, the input nd/or output file must be typed immediately after the 
command and in the order <input file> <output file>. Other arguments to the command may be typed 
after these.


**********************MILESTONE 1*****************

a. Instructions for building the shell
1.Type "make" + Enter to compile the xssh.
2.Type "./xssh" + Enter to go to the shell.
3.Type "./xssh" + Filename + Enter to read or test the commands from a specific file.(For test cases, see the instruction c)
	
b. Instructions for testing and running the shell
1. Type "./xssh" + options + enter to run the shell
	Option List:
	-x
	Display the command line after variable substitution but before it is evaluated.

c.Current test cases (run by typing "./xssh <filename>":
	echo_and_quit.txt			test the echo and quit commands
	echoes.txt 				test the echo command
	exteranl_commands.txt			test the external command and the echo command

Top three design/implementation feartures:
	1. Implementation of external commands
	2. Implementation of the echo 
	3. Implementation of reading the commands from the file

Top three questions remain to be answered:
	1. What should we do about the debug level?
	2. Linux & Mac diffenrence when dealing with the shell?
	3. What is variable substitution?

**********************MILESTONE 2*****************
a. Instructions for building the shell
1.Type "make" + Enter to compile the xssh.
2.Type "./xssh" + Enter to go to the shell.
3.Type "./xssh" + Filename + Enter to read or test the commands from a specific file.(For test cases, see the instruction c)
	
b. Instructions for testing and running the shell
1. Type "./xssh" + options + enter to run the shell
	Option List:

	Redirection 
	The standard input and standard output of a command can be redirected to and from arbitrary files. Redirection is indicated using the less-than (<) character for input redirection and the greater-than (>) character for output redirection. xssh must support input and output redirection for all shell builtins as well as all executed external commands. The syntax for redirection is:
	command [<input_file] [>output_file]
	The < or > characters are required to be the first character of their respective arguments and cannot be separated from the filename by a space. 
	
	wait pid
	Wait for the child process pid to complete. If pid is -1, wait for any child process to complete. If pid is 0, wait for all child processes to complete. If xssh has not spawned a background process with PID pid or that process has already completed, return immediately.

    	Note that xssh is only responsible for waiting on its own children; it does not have to implement wait for arbitrary PIDs.


**********************MILESTONE 3*****************
a. Instructions for building the shell
1.Type "make" + Enter to compile the xssh.
2.Type "./xssh" + Enter to go to the shell.
3.Type "./xssh" + Filename + Enter to read or test the commands from a specific file.(For test cases, see the instruction c)
	
b. Instructions for testing and running the shell
1. Type "./xssh" + options + enter to run the shell
	Option List:
	
	Variable substitution $
	With the exception of $$, $?, and $!, all variables in xssh are environment variables (i.e. the shell does not maintain its own set of variables).
	xssh performs variable substitution on each command line before it is executed. This means that an argument, command name or path may be placed into a variable, which must be evaluated in order to determine what to execute.
	e.g., if $E evaluates to echo, and $HELLO evaluates to hello world, then $E $HELLO should result in the command echo hello world being run.

	set [variable [value]]
	When run with no arguments, display a list of all environment variables and their values. Otherwise set variable to value or the empty string if value is not given.
    	xssh should follow standard variable naming conventions for syntax-checking variable (see VARIABLES for more details). Note that some environment implementations may be more lax than this standard; if xssh conforms to these specifications, it must also allow variable names to exist that do not follow this specification (i.e. xssh may not alter or unset nonconformant variables).
	
	unset [variable …]
	Remove the given variables from the environment. The value of a unset variable is undefined.


**********************MILESTONE 4*****************
a. Instructions for building the shell
1.Type "make" + Enter to compile the xssh.
2.Type "./xssh" + Enter to go to the shell.
3.Type "./xssh" + Filename + Enter to read or test the commands from a specific file.(For test cases, see the instruction c)

b. Instructions for testing and running the shell
1. Type "./xssh" + options + enter to run the shell
	Option List:
	-d level
    	Set the debugging level to level. A debug level of 0 will not output any debug messages while 		1 will output enough information to verify the correctness of the operation. The extent of 		that information is left to the implementation. 
	
	cd [path]
    	Update the present working directory. xssh should attempt the following, in order:
        If path is given, set the working directory to path.
        If path is not given, but the HOME environment variable is set, set the working directory to the value given in the HOME environment variable.
        If path is not given and the HOME environment variable is not set, set the working directory to the drive root (/ in Linux). 

    	Note: the current directory is available from the PWD environment variable. cd must update this environment variable on a successful working directory change.
	
	chdir [path]
    	see cd [path].

	PWD
	show the current directory

	
Top three design/implementation feartures:
	1. Implementation of history function. The user may use the up and down to find previous commands.
	2. Implementation of the readline function. When cd into a directory, user is able to use [Tab] to do the contents assistant 
	3. Implementation of editting the command, user is able to edit the command line by left and right button.
	4. Debugging. Our shell can use the debug level to debug the project.	





























