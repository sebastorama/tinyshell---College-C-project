/* 
 *
 * Alex Palmeira Fragoso              - 5377692
 * Lucas Pierre Figueiredo            - 6426847
 * Sebastião Giacheto Ferreira Júnior - 5634611
 *
 *  * Grupo B14 - SO2
 */

#include "jobs.h"


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
			printf("- %s\n", jobs[i].cmdline);
		}
	}
}

void initialize_job(job *init) {
	init->pid = 0;
	init->state = UNDEF;
	memset(init->cmdline, 0, sizeof(char)*MAXLINE);
}

void initialize_jobs(job *jobs) {
	int i;
	for (i=0; i<MAXJOBS; i++) {
		initialize_job(&jobs[i]);
	}
}

int first_free_job(job *jobs) {
	int i;
	for (i=0; i<MAXJOBS; i++) {
		if (jobs[i].pid == 0) return i;
	}
	return -1;
}


int add_job(job *jobs, pid_t pid, int state, char *cmdline) {
	if (pid > 0) {
		int index = first_free_job(jobs);
		if(index < 0) {
			return -1;
		} else {
			jobs[index].pid	= pid;
			jobs[index].state = state;
			strcpy(jobs[index].cmdline, cmdline);
			return index;
		}
	}
	return -1;
}

int remove_job_by_pid(job *jobs, pid_t pid) {
	int jid = pid_to_jid(jobs, pid);
	if(jid >= 0) {
		remove_job(jobs, jid);
		return jid;
	} else {
		return -1;
	}
}

void remove_job(job *jobs, int jid) {
	initialize_job(&jobs[jid]);
}

int pid_to_jid(job *jobs, pid_t pid) {
	int i = 0;
	for (i=0; i<MAXJOBS; i++) {
		if (jobs[i].pid == pid) return i;
	}
	return -1;
}
