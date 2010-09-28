/* 
 *
 * Alex Palmeira Fragoso              - 5377692
 * Lucas Pierre Figueiredo            - 6426847
 * Sebastião Giacheto Ferreira Júnior - 5634611
 *
 *  * Grupo B14 - SO2
 */
#include "constants.h"


int builtin_cmd(char **argv);
int parseline(const char *cmdline, char **argv, int *argc_out);
char * which(char * argv0);
int file_exists(const char * filename);
