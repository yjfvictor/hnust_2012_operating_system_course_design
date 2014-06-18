#include "common.h"
#include "deal_command.h"
#include <getopt.h>

bool split_command(const char * command, int * pargc, char *** pargv)
{
	char * saveptr;
	char command_operation[MAX_COMMAND_LENGTH];
	char * p = NULL;

	memset( command_operation, 0, sizeof(command_operation) );
	strncpy( command_operation, command, MAX_COMMAND_LENGTH - 1);

	for( *pargc = 0, p = command_operation; *pargc < MAX_SEPERATORS; ++(*pargc), p = NULL )
	{
		p = strtok_r( p, " ", &saveptr );
		if ( p == NULL )
		{
			(*pargv)[*pargc] = NULL;
			return true;
		}
		(*pargv)[*pargc] = (char *)malloc( (MAX_PATH+1) * sizeof(char));
		if ((*pargv)[*pargc] == NULL)
		{
			fprintf(stderr, "No sufficient memory\n");
			exit(EXIT_FAILURE);
		}
		memset((*pargv)[*pargc], 0, sizeof(MAX_PATH+1));
		strncpy((*pargv)[*pargc], p, MAX_PATH);
	}

	strcpy(lastError, "Command too long");
	return false;
}

bool run_command(int argc, char * const * argv)
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
		int c;
		int option_index = 0;
		char filename[MAX_PATH];
		memset(filename, 0, sizeof(filename));
		bool all = false, almost_all = false, long_list = false;
		const struct option long_options[] = {
			{ "all",	0, NULL, 'a' },
			{ "almost-all",	0, NULL, 'A' },
			{ NULL,		0, NULL, 0 }
		};

		optarg = NULL;
		optind = 0;

		while ( true )
		{
			c = getopt_long(argc, argv, "aAl", long_options, &option_index);
      			if (c == -1)
				break;

			switch(c)
			{
				case 'a':
					all = true;
					break;
				case 'A':
					almost_all = true;
					break;
				case 'l':
					long_list = true;
					break;
				default:
					return true;
					break;
			}
		}
		return ls(current_path, all, almost_all, long_list, filename);
	}

	if ( !strcmp(argv[0], "cat" ) )
	{
		for ( i = 1; i < argc; ++ i )
			cat( argv[i] );
		puts("");
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
		int c;
		bool recursive = false, force = false;
		int option_index = 0;
		const struct option long_options[] = {
			{ "recursive",	0, NULL, 'r' },
			{ "force",	0, NULL, 'f' },
			{ NULL,		0, NULL, 0 }
		};

		while ( true )
		{
			c = getopt_long(argc, argv, "rRf", long_options, &option_index);
      			if (c == -1)
				break;

			switch(c)
			{
				case 'r':
				case 'R':
					recursive = true;
					break;
				case 'f':
					force = true;
					break;
				case '?':
					return true;
					break;
				default:
					break;
			}
		}
		return rm( current_path, (const char * const* const)&(argv[optind]), recursive, force );
	}

	return true;
}

void print_last_error(void)
{
	fprintf(stderr, "%s\n", lastError);
}
