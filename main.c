/* 
 * MiniShell - Parte 1
 *
 * Alex Palmeira Fragoso              - 5377692
 * Lucas Pierre Figueiredo            - 6426847
 * Sebastião Giacheto Ferreira Júnior - 5634611
 *
 *  * Grupo B14 - SO2
 */

#include "constants.h"
#include "jobs.h"
#include "parser.h"

/* Misc manifest constants */
#define _ANSI_SOURCE

/* Global variables */
char prompt[] = "B14> ";    /* command line prompt */
char sbuf[MAXLINE];         /* for composing sprintf messages */
job jobs[MAXJOBS];

	/* Terminal/session/process group */
	int shell_terminal;
	pid_t shell_pgid;
	struct termios shell_tmodes;
/* End global variables */


/* Function prototypes */
void sigchld_handler(int sig);

void default_signals();
void ignore_signals();
void initialize_shell();
void eval(char *cmdline);



/* Dir operations */
void print_dir();
int change_dir();

/* Misc function prototypes */
void usage(void);
void unix_error(char *msg);
void app_error(char *msg);

/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv) 
{
    char cmdline[MAXLINE];
	initialize_shell();
    int emit_prompt = 1; /* emit prompt (default) */
	initialize_jobs(jobs);
	
    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);
	
    /* Execute the shell's read/eval loop */
    while (1) {
		/* Read command line */
		if (emit_prompt) {
			printf("%s", prompt);
			fflush(stdout);
		}
		if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
			app_error("fgets error");
		if (feof(stdin)) { /* End of file (ctrl-d) */
			fflush(stdout);
			exit(0);
		}
		
		/* Evaluate the command line */
		eval(cmdline);
		fflush(stdout);
    } 
	
    exit(0); /* control never reaches here */
}


/* default_signals() - Set back the signals ignored by ignore_signals to the
 default behavior. */
void default_signals() {
	signal(SIGINT , SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGTSTP, SIG_DFL);
	signal(SIGTTIN, SIG_DFL);
	signal(SIGTTOU, SIG_DFL);
	signal(SIGCHLD, SIG_DFL);	
}

/* ignore_signals() - We are the shell, we don't want the shell process to close
 itself accidentaly if we receive a signal. */
void shell_signals() {
	signal(SIGINT , SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGCHLD, sigchld_handler);
}


/*
 * initialize_shell - Initialize the shell :D 
 *
 */
void initialize_shell() {
	shell_terminal = STDIN_FILENO;
	
	/* Loop until we are in the foreground.  */
	while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
		kill(- shell_pgid, SIGTTIN);
	
	shell_signals();
	
	/* Put ourselves in our own process group.  */
	shell_pgid = getpid();
	if (setpgid (shell_pgid, shell_pgid) < 0) {
		unix_error("Couldn't put the shell in its own process group");
		exit (1);
	}
	
	/* Grab control of the terminal.  */
	tcsetpgrp(shell_terminal, shell_pgid);
	
	/* Save default terminal attributes for shell.  */
	tcgetattr(shell_terminal, &shell_tmodes);
}

/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 */
void eval(char *cmdline) 
{
	char *argv[MAXARGS];
	memset(argv, 0x0, sizeof(char *)*MAXARGS); /* Reset the argv vector */
	int argc, bg;
	pid_t pid;	
	
	bg = parseline(cmdline, argv, &argc);
	if (!argc | builtin_cmd(argv)) return;
	
	
	/* get the argv0 (executable name), with absolute path */
	char * argv0_with_path = which(argv[0]);
	if(!argv0_with_path) return;
	
	pid = fork();
	if (pid > 0)         /* father code */
	{
		setpgid (pid, pid);
		if(bg) {
			add_job(jobs, pid, BG, argv0_with_path);
		}
		else {
			tcsetpgrp(shell_terminal, pid);
			waitpid(pid, 0, 0);
			tcsetpgrp(shell_terminal, getpid());
			remove_job_by_pid(jobs, pid);
		}
	} 
	else if (pid==0)   /* child code */
	{	
		pid = getpid();
		setpgid(pid, pid);
		default_signals();
		if(!bg) tcsetpgrp(shell_terminal, pid);
		execvp(argv0_with_path, argv);
		exit(1); /* if something goes wrong */
	}
	
	memset(argv, 0x0, sizeof(char *)*MAXARGS); /* Reset the argv vector */
	return;
}


/*
 * builtin_cmd - Check if a given command is builtin.
 * If the answer is positive, execute the command and return 1.
 * If not, do nothing, and return 0.
 */
int builtin_cmd(char **argv) {
	
    if (strcmp(argv[0],"quit") == 0) {
		printf("Quitting ...\n");
		exit(0);
	}
	else if (strcmp(argv[0], "jobs") == 0) {
		list_jobs(jobs);
		return 1;
	}
	else if (strcmp(argv[0], "cd") == 0) {
		chdir(argv[1]);
		return 1;
	}
	else if (strcmp(argv[0], "pwd") == 0) {
		print_dir();
		return 1;
	}
	return 0;     /* not a builtin command */
}


/* 
 * sigchld_handler - When any of the child process receive
 * a signal, update all the job status.
 */
void sigchld_handler(int sig) 
{
	update_job_status(jobs);
	return;
}

/* Dir operations */
void print_dir() {
	char temp[1024];
	getcwd(temp,1024);
	printf("%s\n", temp);
}

/***********************
 * Other helper routines
 ***********************/
/*
 * unix_error - unix-style error routine
 */
void unix_error(char *msg)
{
	fprintf(stdout, "%s: %s\n", msg, strerror(errno));
	exit(1);
}

/*
 * app_error - application-style error routine
 */
void app_error(char *msg)
{
	fprintf(stdout, "%s\n", msg);
	exit(1);
}
