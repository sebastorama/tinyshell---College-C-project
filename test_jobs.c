/*
 *  test_jobs.c
 *  tsh
 *
 *  Created by Sebastião Ferreira on 9/28/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#define _ANSI_SOURCE

#include "constants.h"
#include "jobs.h"

job jobs[MAXJOBS]; /* The job list */

int main(void) {

	initialize_jobs(jobs);
	add_job(jobs, 23, "Meu primeiro job");
	add_job(jobs, 22, "Meu segundo job");
	list_jobs(jobs);
	remove_job_by_pid(jobs, 23);
	add_job(jobs, 50, "Meu terceiro job");
	printf("%d %d %d", pid_to_jid(jobs, 22), pid_to_jid(jobs, 50), pid_to_jid(jobs, 45));
	return 0;
}