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
/* End global variables */


/* Function prototypes */
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
		fflush(stdout);
    } 
	
    exit(0); /* control never reaches here */
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
	pid = fork();
	if (pid > 0)         /* father code */
	{
		if(bg) {
			add_job(jobs, pid, BG, argv0_with_path);
			printf("Process Job ID: %d\n"
				   "PID: %d\n"
				   "Name: %s\n"
				   "in background...\n", pid_to_jid(jobs, pid), pid, argv[0]);
			
		}
		else {
			add_job(jobs, pid, FG, argv0_with_path);
			waitpid(pid, 0, 0);
		}
	} 
	else if (pid==0)   /* child code */
	{
		setpgid(0, 0);
		execve(argv0_with_path, argv, 0);
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
	char *args[MAXARGS];
	
    *args = *argv;
    if (strcmp(args[0],"quit") == 0) {
		printf("Quitting ...\n");
		exit(0);
	}
	else if (strcmp(args[0], "jobs") == 0) {
		list_jobs(jobs);
		return 1;
	}
	else if (strcmp(args[0], "cd") == 0) {
		change_dir();
		return 1;
	}
	else if (strcmp(args[0], "pwd") == 0) {
		print_dir();
		return 1;
	}
	return 0;     /* not a builtin command */
}



/* Dir operations */
void print_dir() {
	char temp[1024];
	getcwd(temp,1024);
	printf("%s\n", temp);

}

int change_dir() {
	
	return 0;
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
