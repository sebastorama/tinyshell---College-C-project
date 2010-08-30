/* 
 * tsh - A tiny shell program with job control
 * 
 * Sebastião Giacheto Ferreira Júnior - 5634611
 * Francisco Ito Anuatti			  - 4586996
 * Turma A - SO2
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

/* Misc manifest constants */
#define _ANSI_SOURCE
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */


/* 
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

/* Global variables */
extern char **environ;      /* defined in libc */
char prompt[] = "B14> ";    /* command line prompt (DO NOT CHANGE) */
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char sbuf[MAXLINE];         /* for composing sprintf messages */
/* End global variables */


/* Function prototypes */

/* Here are the functions that you will implement */
void eval(char *cmdline);


/* Here are helper routines that we've provided for you */
int parseline(const char *cmdline, char **argv, int *argc_out); 

void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
typedef void handler_t(int);

/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv) 
{
    char c;
    char cmdline[MAXLINE];
    int emit_prompt = 1; /* emit prompt (default) */
	
    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);
	
    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
			case 'h':             /* print help message */
				usage();
				break;
			case 'v':             /* emit additional diagnostic info */
				verbose = 1;
				break;
			case 'p':             /* don't print a prompt */
				emit_prompt = 0;  /* handy for automatic testing */
				break;
			default:
				usage();
		}
    }
	
	
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
	int argc, bg;
	pid_t pid;
	
	bg = parseline(cmdline, argv, &argc);
	if (!argc) return; /* parseline got a blank line */
	
	
	pid = fork();
	if (pid > 0)         /* father code */
	{   
		waitpid(pid, 0, 0);
	} 
	else if (pid==0)   /* child code */
	{
		setpgid(0, 0);
		execve(argv[0], argv, 0);
		exit(1); /* if something goes wrong */
	}
	
	return;
}

/* 
 * parseline - Parse the command line and build the argv array.
 * 
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.
 *
 *
 * We modified this a little bit from the original source by adding a 
 * the extern parameter "argc_out". This parameter should inform the
 * caller, the number of arguments parsed on "argv" array.
 */
int parseline(const char *cmdline, char **argv, int *argc_out) 
{
    static char array[MAXLINE]; /* holds local copy of command line */
    char *buf = array;          /* ptr that traverses command line */
    char *delim;                /* points to first space delimiter */
    int argc;                   /* number of args */
	
    strcpy(buf, cmdline);
    buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space */ 
    while (*buf && (*buf == ' ')) /* ignore leading spaces */
		buf++;
	
	
	
    /* Build the argv list */
    argc = 0;
    if (*buf == '\'') {
		buf++;
		delim = strchr(buf, '\'');
    }
    else {
		delim = strchr(buf, ' ');
    }
	
    while (delim) {
		argv[argc++] = buf;
		*delim = '\0';
		buf = delim + 1;
		while (*buf && (*buf == ' ')) /* ignore spaces */
			buf++;
		
		if (*buf == '\'') {
			buf++;
			delim = strchr(buf, '\'');
		}
		else {
			delim = strchr(buf, ' ');
		}
    }
    argv[argc] = NULL;
    
    if (argc == 0) {  /* ignore blank line */
		*argc_out = 0;
		return 1;
	}
	
	*argc_out = argc;
    return 0;
}

/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void) 
{
	printf("Usage: shell [-hvp]\n");
	printf("   -h   print this message\n");
	printf("   -v   print additional diagnostic information\n");
	printf("   -p   do not emit a command prompt\n");
	exit(1);
}

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


