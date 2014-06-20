#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>

#define MAX_SEPERATORS 128
#define MAX_NAME 256
#define MAX_PATH 2048
#define MAX_COMMAND_LENGTH 4096


char username[MAX_NAME];
char hostname[MAX_NAME];
char current_path[MAX_PATH];
char lastError[MAX_NAME];
int  fd_ext2;

#endif // _COMMON_H
