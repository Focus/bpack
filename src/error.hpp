#ifndef _ERROR_H
#define _ERROR_H

#include <iostream>
#include <errno.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include "config.hpp"
/*
 * Error levels:
 * 0 - Display message and do nothing
 * 1 - Display message and ask user input
 * 2 - Display message and quit
 * 
 * Quit levels (from config):
 * 0 - Default
 * 1 - Ignore level 1 errors and carry on
 * 2 - Ignore level 2 errors and carry on (i.e. the user is either me or an idiot,...err... the first part was redundant
 * -1 - Ask to quit on 1 errors
 * -2 - Ask to quit on 2 errors
 * 			
 */
void err(std::string message, int level=0,bool errnoset=0);
bool ask(std::string);
#endif
