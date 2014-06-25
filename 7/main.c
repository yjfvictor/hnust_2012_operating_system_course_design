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
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include "parse_string.h"
#include "ext2.h"

void output_shell_prompt(void)
{
	printf("[%s@%s %s] $ ", username, hostname, current_path);
	fflush(stdout);
}

void exit_func(void)
{
	close(fd_ext2);
	fd_ext2 = -1;
}

void keyboard_interrupt(int number)
{
	puts("");
	output_shell_prompt();
}

void destroy_argv(char *** pargv)
{
	char ** p;
	for ( p = *pargv; *p != NULL; ++ p )
	{
		free(*p);
		*p = NULL;
	}
	free(*pargv);
	*pargv = NULL;
}

bool login(void)
{
#ifdef _DEBUG
	strcpy(username, "victor");
	strcpy(hostname, "victor-host");
	return true;
#else
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
#endif
}

int main(int argc, char * argv[])
{
	int cmd_argc;
	char ** cmd_argv;

	char command[MAX_COMMAND_LENGTH];

	signal(SIGINT, keyboard_interrupt);
	signal(SIGQUIT, keyboard_interrupt);

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

	fd_ext2 = open(argv[1], O_RDWR);
	if ( fd_ext2 == -1 )
	{
		perror("Error reading the ext2 file");
		return EXIT_FAILURE;
	}
	atexit(exit_func);

	strcpy(current_path, "/");

	read_super_block_data();
	read_group_descriptor();
	read_inode();

	output_shell_prompt();

	while ( fgets( command, MAX_COMMAND_LENGTH, stdin ) != NULL )
	{

		command[MAX_COMMAND_LENGTH - 1] = '\0';
		command[strlen(command) - 1] = '\0';

		cmd_argv = (char **)malloc((MAX_SEPERATORS + 1) * sizeof(char *));
		if ( !( split_command(command, &cmd_argc, &cmd_argv) && run_command(cmd_argc, cmd_argv) ) )
			print_last_error();

		destroy_argv(&cmd_argv);

		output_shell_prompt();
	}

	puts("");

	return EXIT_SUCCESS;
}
