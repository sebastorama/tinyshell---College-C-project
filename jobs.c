/*
 *  jobs.c
 *  tsh
 *
 *  Created by Sebastião Ferreira on 9/28/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "jobs.h"

void initialize_jobs(job *jobs) {
	int i;
	for (i=0; i<MAXJOBS; i++) {
		jobs[i].pid = 0;
		jobs[i].state = UNDEF;
		memset(jobs[i].cmdline, 0, sizeof(char)*MAXLINE);
	}
}

int first_free_job(job *jobs) {
	int i;
	for (i=0; i<MAXJOBS; i++) {
		if (jobs[i].pid == 0) return i;
	}
	return -1;
}


int add_job(job *jobs, pid_t pid, char *cmdline) {
	if (pid > 0) {
		int index = first_free_job(jobs);
		if(index < 0) {
			return -1;
		} else {
			jobs[index].pid	= pid;
			jobs[index].state = BG;
			strcpy(jobs[index].cmdline, cmdline);
		}
	} else {
		return -1;
	}
	
}

int remove_job(job *jobs, int jid) {
	
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
