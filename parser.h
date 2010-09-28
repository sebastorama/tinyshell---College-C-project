/*
 *  parser.h
 *  tsh
 *
 *  Created by Sebastião Ferreira on 9/28/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "constants.h"


int builtin_cmd(char **argv);
int parseline(const char *cmdline, char **argv, int *argc_out);
char * which(char * argv0);
int file_exists(const char * filename);
