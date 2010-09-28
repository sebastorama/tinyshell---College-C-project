/*
 *  jobs.h
 *  tsh
 *
 *  Created by Sebastião Ferreira on 9/28/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "constants.h"

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

void list_jobs(job *jobs);
void initialize_jobs(job *jobs);
int first_free_job(job *jobs);
int add_job(job *jobs, pid_t pid, char *cmdline);
int remove_job(job *jobs, int jid);
