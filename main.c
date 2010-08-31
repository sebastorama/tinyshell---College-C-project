/* 
 * MiniShell - Parte 1
 *
 * Alex Palmeira Fragoso              - 5377692
 * Lucas Pierre Figueiredo            - 6426847
 * Sebastião Giacheto Ferreira Júnior - 5634611
 *
 *  * Turma B - SO2
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/* Misc manifest constants */
#define _ANSI_SOURCE
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */
#define MAXPATHS     16   /* 

/* Global variables */
char prompt[] = "B14> ";    /* command line prompt */
int verbose = 0;            /* if true, print additional output */
char sbuf[MAXLINE];         /* for composing sprintf messages */
/* End global variables */


/* Function prototypes */
void eval(char *cmdline);
int parseline(const char *cmdline, char **argv, int *argc_out);
char * which(char * argv0);
int file_exists(const char * filename);

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

	if (!argc) return; /* parseline got a blank line */
	
	
	/* get the argv0 (executable name), with absolute path */
	char * argv0_with_path = which(argv[0]);
	pid = fork();
	if (pid > 0)         /* father code */
	{   
		waitpid(pid, 0, 0);
		memset(argv, 0x0, sizeof(char *)*MAXARGS); /* Reset the argv vector */
	} 
	else if (pid==0)   /* child code */
	{
		setpgid(0, 0);
		execve(argv0_with_path, argv, 0);
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

/* Similar to the unix program 'which'. Get the program name in argv0, search
 for it's ocurrence on the directorys list of $PATH, and returns the first 
 ocurrence, with the program name appended to the directory str 
 fresh new allocated str. If nothing found, returns NULL. 
 Also, if argv0 is already an absolute path (with a '/' on the beginning), 
 return argv0 unchanged */

char * which(char * argv0) {
	if (argv0) {
		if (argv0[0] == '/') return argv0;
		char *env_path = getenv("PATH");
		char *path = calloc(strlen(env_path), sizeof(char));
		strcpy(path, env_path);
		char *current_path = NULL; /* Holds the current path that's being tested */
		char *which = NULL;
		
		int i;
		for (i=0; i<MAXPATHS && path; i++) {
			current_path = strsep(&path, ":");
			which = calloc(strlen(argv0) + strlen(current_path) + 1, sizeof(char));
			
			/* Concatenate argv0 + '/' + current_path for file testing */
			strcat(which, current_path);
			if (*(path-2) != '/') strcat(which, "/");
			strcat(which, argv0);
			
			if (file_exists(which)) return which;
			free(which);
			which = NULL;
		}
		
		return NULL;	
	} else {
		return NULL;
	}

	
}

/* Check file existence on path "filename" */
int file_exists(const char * filename) {
	FILE * file;
    if (file = fopen(filename, "r")) {
        fclose(file);
        return 1;
    }
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
