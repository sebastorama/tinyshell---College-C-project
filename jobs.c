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

void print_job(job j) {
	printf("[%d] (%d) ", j.jid, j.pid);
	switch (j.state) {
		case BG: 
			printf("Running ");
			break;
		case FG: 
			printf("Foreground ");
			break;
		case ST: 
			printf("Stopped ");
			break;
		case TM:
			printf("Terminated ");
			break;
		default:
			printf("listjobs: Internal error: job[%d].state=%d ", 
				   j.jid, j.state);
	}
	
	printf("- %s\n", j.cmdline);
}

void initialize_job(job *init, int jid) {
	init->jid = jid;
	init->pid = 0;
	init->state = UNDEF;
	memset(init->cmdline, 0, sizeof(char)*MAXLINE);
}


void set_job_status(job *j, int status) {	
	if (WIFSTOPPED(status)) {
		j->state = ST;
		print_job(*j);
	} else {
		j->state = TM;
		print_job(*j);
		initialize_job(j, j->jid);
		
		if(WIFSIGNALED(status)) {
			fprintf(stderr, "Terminated by signal %d.\n",
					 (int) j->pid, WTERMSIG(status));
		}
	}
}

void update_job_status(job *jobs) {
	int status;
	pid_t pid;
	do {
		pid = waitpid(-1, &status, WUNTRACED|WNOHANG);
		if(pid>0) {
			int jid = pid_to_jid(jobs, pid);
			if(jid != -1) set_job_status(&jobs[jid], status);
		}
	} while(pid > 0);
}


void list_jobs(job *jobs) {
	int i;
	char job_found = 0;
	for (i=0; i< MAXJOBS; i++) {
		if (jobs[i].pid != 0) {
			job_found = 1;
			print_job(jobs[i]);
		}
	}
	if(!job_found) printf("There isn't any job in this session\n");
}



void initialize_jobs(job *jobs) {
	int i;
	for (i=0; i<MAXJOBS; i++) {
		initialize_job(&jobs[i], i);
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
	initialize_job(&jobs[jid], jid);
}

int pid_to_jid(job *jobs, pid_t pid) {
	int i = 0;
	for (i=0; i<MAXJOBS; i++) {
		if (jobs[i].pid == pid) return i;
	}
	return -1;
}
