/* 
 *
 * Alex Palmeira Fragoso              - 5377692
 * Lucas Pierre Figueiredo            - 6426847
 * Sebastião Giacheto Ferreira Júnior - 5634611
 *
 *  * Grupo B14 - SO2
 */
#include "constants.h"

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */
#define TM 4	/* signaled/terminated */

typedef struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
	int jid;
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE];  /* command line */
} job;


/* update_job_status() - Set the real state for the jobs on the
 * "jobs" job list using the waitpid call to set the current state.
 */
void update_job_status(job *jobs);

/* list_jobs() - print jobs list
 *
 * The list_jobs() function prints the job list "jobs" 
 */
void list_jobs(job *jobs);

/* initialize_jobs() - Initialize a job list array
 *
 * The initialize_job() function initialize the "jobs" array with
 * default values on each field of the job_t struct.
 */
void initialize_jobs(job *jobs);

/* add_job() - add job to a job list
 *
 * The add_job() function add a job with "pid" and "cmdline" to
 * the job list "jobs" on the very first avaliable position (jid).
 * If you attempt to insert an invalid "pid", returns -1. On success
 * returns the jid.
 */
int add_job(job *jobs, pid_t pid, int state, char *cmdline);

/* remove_job_by_pid() - remove job from the jobs list
 *
 * The remove_job_by_pid() function remove the current job
 * that holds "pid" from the "jobs" job list and return it's jid.
 * If no such job was found, returns -1
 */
int remove_job_by_pid(job *jobs, pid_t pid);

/* remove_job() - remove job from the jobs list
 *
 * The remove_job() funcion removes the job identified by "jid" from
 * the job list "jobs".
 * WARNING: No checking is made here, if your jid is don't exist,
 * bad things will happen. So be sure to check this first!
 */
void remove_job(job *jobs, int jid);

/* pid_to_jid() - Fetch the the jid from a given pid
 *
 * The pid_to_jid() search for the job which the current
 * pid is "pid" on the job list "jobs" and return it's jid.
 * If nothing was found, returns -1.
 */
int pid_to_jid(job *jobs, pid_t pid);
