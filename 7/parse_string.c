#include "common.h"

static char lastError[MAX_NAME] = "";

bool split_command(const char * command, int * pargc, char *** pargv)
{
	char * saveptr;
	char command_operation[MAX_COMMAND_LENGTH];
	char * p = NULL;

	memset( command_operation, 0, sizeof(command_operation) );
	strncpy( command_operation, command, MAX_COMMAND_LENGTH - 1);

	for( *pargc = 0, p = command_operation; *pargc < MAX_SEPERATORS; ++(*pargc), p = NULL )
	{
		(*pargv)[*pargc] = strtok_r( p, " ", &saveptr );
		if ( (*pargv)[*pargc] == NULL )
			return true;
	}

	strcpy(lastError, "Command too long");
	return false;
}

bool run_command(int argc, const char * argv[])
{
	if ( argc == 0 || argv[0][0] == '#' )
		return true;

	if ( !strcmp(argv[0], "exit") )
	{
		if ( argc == 2 )
			exit(atoi(argv[1]));
		else
			exit(EXIT_SUCCESS);
	}

	return true;
}

void print_last_error(void)
{
	fprintf(stderr, "%s\n", lastError);
}
