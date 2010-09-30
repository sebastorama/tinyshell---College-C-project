/* 
 *
 * Alex Palmeira Fragoso              - 5377692
 * Lucas Pierre Figueiredo            - 6426847
 * Sebastião Giacheto Ferreira Júnior - 5634611
 *
 *  * Grupo B14 - SO2
 */

#include "parser.h"

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
	int bg;						/* the cmd line instruct the cmd to run in bg mode? */
	
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
	
	/* should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0) {
		argv[--argc] = NULL;
    }	
	*argc_out = argc;
    return bg;
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
			if (*(current_path-2) != '/') strcat(which, "/");
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
