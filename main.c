/* 
 * MiniShell - Parte 1
 *
 * Alex Palmeira Fragoso              - 5377692
 * Lucas Pierre Figueiredo            - 6426847
 * Sebastião Giacheto Ferreira Júnior - 5634611
 *
 *  * Grupo B14 - SO2
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
#define MAXJOBS      16   /* max jobs at any point in time */

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */


typedef struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE];  /* command line */
} job;
job jobs[MAXJOBS]; /* The job list */


/* Global variables */
char prompt[] = "B14> ";    /* command line prompt */
char sbuf[MAXLINE];         /* for composing sprintf messages */
/* End global variables */


/* Function prototypes */
void eval(char *cmdline);
int builtin_cmd(char **argv);
int parseline(const char *cmdline, char **argv, int *argc_out);
char * which(char * argv0);
int file_exists(const char * filename);

/* Dir operations */
void print_dir();
int change_dir();

/* Job operations */
void list_jobs(job *jobs);
void initialize_jobs(job *jobs);
int first_free_job(job *jobs);
int add_job(job *jobs, pid_t pid, char *cmdline);
int remove_job(job *jobs, int jid);

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
	if(builtin_cmd(argv)) return; /* check for builtin cmd */
	if (!argc) return;            /* parseline got a blank line */
	
	
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

/* 
 * parseline - Parse the command line and build the argv array.
 * 
 * Characters enclosed in single quotes are treated as a single
 * argument.
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
 for it's ocurrence on the directorys list of $PATH, and returns the first valid 
 ocurrence, with the program name appended to the directory on a
 fresh new allocated str. If nothing was found, returns NULL. 
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
		
		while (path) {
			current_path = strsep(&path, ":");
			which = calloc(strlen(argv0) + strlen(current_path) + 1, sizeof(char));
			
			/* Concat argv0 + '/' + current_path for file testing */
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


/* Dir operations */
void print_dir() {
	
}

int change_dir() {
	
	return 0;
}



/***********************
 * Jobs operations 
 ***********************/
void list_jobs(job *jobs) {
	int i;
	for (i=0; i< MAXJOBS; i++) {
		if (jobs[i].pid != 0) {
			printf("[%d] (%d) ", i, jobs[i].pid);
			switch (jobs[i].state) {
				case BG: 
					printf("Running ");
					break;
				case FG: 
					printf("Foreground ");
					break;
				case ST: 
					printf("Stopped ");
					break;
				default:
					printf("listjobs: Internal error: job[%d].state=%d ", 
						   i, jobs[i].state);
			}
			printf("%s", jobs[i].cmdline);
		}
	}
}

void initialize_jobs(job *jobs) {
	int i;
	for (i=0; i<MAXJOBS; i++) {
		jobs[i].pid = 0;
		jobs[i].state = UNDEF;
		memset(jobs[i].cmdline, 0, sizeof(char)*MAXLINE);
	}
}


int first_free_job(job *jobs) {
	
}


int add_job(job *jobs, pid_t pid, char *cmdline) {
	if (pid > 0) {
		
	} else {
		return -1;
	}

}

int remove_job(job *jobs, int jid) {
	
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
