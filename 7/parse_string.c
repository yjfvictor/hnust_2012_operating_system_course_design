#include "common.h"

static char lastError[MAX_NAME] = "";

bool ls( const char * path )
{
	return true;
}

bool cd(const char * path)
{
	return true;
}

bool cat( const char * path )
{
	return true;
}

bool rm( const char * path )
{
	return true;
}

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
	int i;
	char directory[MAX_COMMAND_LENGTH];
	if ( argc == 0 || argv[0][0] == '#' )
		return true;

	if ( !strcmp(argv[0], "exit") )
	{
		if ( argc == 2 )
			exit(atoi(argv[1]));
		else
			exit(EXIT_SUCCESS);
	}

	if ( !strcmp(argv[0], "ls") || !strcmp(argv[0], "dir") )
	{
		return true;
	}

	if ( !strcmp(argv[0], "cat" ) )
	{
		for ( i = 1; i < argc; ++ i )
			cat( argv[i] );
		return true;
	}

	if ( !strcmp(argv[0], "cd" ) )
	{
		memset(directory, 0, sizeof(directory));
		if ( argc == 1 )
			strcpy(directory, "~");
		else
			strncpy(directory, argv[1], MAX_COMMAND_LENGTH - 1);
		if ( !strcmp( directory, "~" ) || !strcmp(directory, "#") )
		{
			const char * temp = getenv("HOME");
			if ( temp == NULL )
			{
				strcpy(lastError, "No HOME environment varible set");
				return false;
			}
			else
				strncpy(directory, temp, MAX_COMMAND_LENGTH - 1);
		}
		return cd(directory);
	}

	if ( !strcmp(argv[0], "rm") )
	{
		return true;
	}

	return true;
}

void print_last_error(void)
{
	fprintf(stderr, "%s\n", lastError);
}
