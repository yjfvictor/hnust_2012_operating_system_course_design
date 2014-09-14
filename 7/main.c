/**
 * @file    main.c
 * @brief   主程序
 * @details 输入命令行参数，为一个装有ext2文件系统的文件  
 * @author  叶剑飞
 * @date    2014-06-18
 */

#include "common.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include "parse_string.h"
#include "ext2.h"

/**
 * @brief 输出shell的提示符
 */
void output_shell_prompt(void)
{
	printf("[%s@%s %s] $ ", username, hostname, current_path);
	fflush(stdout);
}

/**
 * @brief 退出时运行的函数
 */
void exit_func(void)
{
	close(fd_ext2);
	fd_ext2 = -1;
}

/**
 * @brief 键盘中断
 * @param[in] number 中断号，这里没有用到
 */
void keyboard_interrupt(int number)
{
	puts("");
	output_shell_prompt();
}

/**
 * @brief 释放argv参数申请的空间
 * @param[in,out] pargv 需要释放的参数的地址
 */
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

/**
 * @brief  判断是否登录成功的函数
 * @return 登录成功返回true，登录失败返回false
 */
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

/**
 * @brief  主函数
 * @param[in]  argc 命令行参数的个数
 * @param[in]  argv 命令行参数的字符串数组，argv[1]为存有ext2文件系统的文件的文件名
 * @return 成功返回EXIT_SUCCESS，失败返回EXIT_FAILURE
 */
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
