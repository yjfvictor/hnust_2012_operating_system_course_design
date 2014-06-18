/*
 *  主函数 
 *
 *  输入命令行参数，为一个装有ext2文件系统的文件  
 *  
 *
 *
 *                       叶剑飞
 *                       2014年6月18日
 *
 */

#include "common.h"
#include "parse_string.h"

bool login(void)
{
	char password[MAX_NAME];
	strcpy(hostname, "victor-host");
	
	memset(username, 0, sizeof(username));
	memset(password, 0, sizeof(password));

	printf("%s login: ", hostname);
	fgets( username, sizeof(username)-1, stdin );
	username[strlen(username) - 1] = '\0';

	if ( !strcmp(getpass("Password: "), "123456") && !strcmp(username, "victor") )
		return true;
	else
		return false;
}

int main(int argc, char * argv[])
{
	int cmd_argc;
	char ** cmd_argv;

	char command[MAX_COMMAND_LENGTH];

	if ( argc != 2 )
	{
		fprintf(stderr, "Usage: %s [ext2file]\n", argv[0]);
		return EXIT_FAILURE;
	}

	if ( !login() )
	{
		fprintf(stderr, "Password error. Log in failed.\n");
		return EXIT_FAILURE;
	}

	strcpy(current_path, "/");

	printf("[%s@%s %s] $ ", username, hostname, current_path);
	fflush(stdout);

	while ( fgets( command, MAX_COMMAND_LENGTH, stdin ) != NULL )
	{

		command[MAX_COMMAND_LENGTH - 1] = '\0';
		command[strlen(command) - 1] = '\0';

		cmd_argv = (char **)malloc((MAX_SEPERATORS + 1) * sizeof(char *));
		if ( split_command(command, &cmd_argc, &cmd_argv) )
			run_command(cmd_argc, cmd_argv);
		else
			print_last_error();

		free(cmd_argv);

		printf("[%s@%s %s] $ ", username, hostname, current_path);
		fflush(stdout);
	}

	puts("");

	return EXIT_SUCCESS;
}
