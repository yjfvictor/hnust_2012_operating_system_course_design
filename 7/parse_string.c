/**
 * @file   parse_string.c
 * @brief  字符串解析函数
 * @author 叶剑飞
 * @date   2014年6月
 */

#include "common.h"
#include "deal_command.h"
#include <getopt.h>
#include "algorithm_data_structure.h"

/**
 * @brief 把一条命令分隔成命令行参数列表
 * @param[in]  command 命令字符串
 * @param[out] pargc   命令行参数的个数
 * @param[out] pargv   存放命令行参数的数组
 * @return 成功返回true，失败返回false
 */
bool split_command(const char * command, int * pargc, char *** pargv)
{
	char * saveptr;
	char command_operation[MAX_COMMAND_LENGTH];
	char * p = NULL;

	assert(command != NULL);
	assert(pargc != NULL);
	assert(pargv != NULL);

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

/**
 * @brief 运行命令
 * @param[in] argc 命令行参数的个数
 * @param[in] argv 命令行参数的数组
 * @return 成功返回true，失败返回false
 */
bool run_command(int argc, char * const * argv)
{
	int i;
	char directory[MAX_COMMAND_LENGTH];
	assert(argv != NULL);

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
		if ( optind == argc )
			return ls(all, almost_all, long_list, NULL);
		else
			return ls(all, almost_all, long_list, (const char * const* const)&argv[optind]);
	}

	else if ( !strcmp(argv[0], "cat" ) )
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

	else if ( !strcmp(argv[0], "rm") )
	{
		int c;
		bool recursive = false, force = false;
		int option_index = 0;
		const struct option long_options[] = {
			{ "recursive",	0, NULL, 'r' },
			{ "force",	0, NULL, 'f' },
			{ NULL,		0, NULL, 0 }
		};

		optarg = NULL;
		optind = 0;

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
		return rm( (const char * const* const)&(argv[optind]), recursive, force );
	}

	else
	{
		fprintf(stderr, "%s: command not found\n", argv[0]);
		return true;
	}

	return true;
}

/**
 * @brief 输出最后一个错误
 */
void print_last_error(void)
{
	fprintf(stderr, "%s\n", lastError);
}

/**
 * @brief 从绝对路径中生成路径链表
 * @details 把路径的分隔符'/'找出来，然后一级一级地进行分隔，每级放在一个链表节点中
 * @param[out] ppath_list 输出参数，路径链表
 * @param[in]  absolute_path 输入参数，绝对路径
 */
void generate_path_linklist(link_list * ppath_list, const char * absolute_path)
{
	link_list p = NULL;
	char * saveptr = NULL;
	char * pstr = NULL;

	char absolute_path_backup[MAX_PATH];
	char current_directory_name[MAX_PATH];

	assert(ppath_list != NULL);
	assert(absolute_path != NULL);
	assert(absolute_path[0] == '/');

	// backup the absolute_path
	strncpy(absolute_path_backup, absolute_path, MAX_PATH);
	absolute_path_backup[MAX_PATH - 1] = '\0';

	// generate the head node of the path list
	*ppath_list = (link_list)malloc(sizeof(link_list_node));
	if ( *ppath_list == NULL )
	{
		fprintf(stderr, "No sufficient memory\n");
		exit(EXIT_FAILURE);
	}
	(*ppath_list)->next = NULL;
	(*ppath_list)->prev = NULL;
	strncpy((*ppath_list)->directory_name, "/", 2);

	// generate the path list
	p = *ppath_list;
	for( pstr = absolute_path_backup; ; pstr = NULL )
	{
		pstr = strtok_r( pstr, "/", &saveptr );
		if ( pstr == NULL )
			break;
		strncpy(current_directory_name, pstr, MAX_PATH);
		current_directory_name[MAX_PATH - 1] = '\0';

		if ( !strcmp(current_directory_name, ".") )
			continue;
		else if ( !strcmp(current_directory_name, "..") )
		{
			if ( p->prev != NULL )
			{
				p = p->prev;
				free(p->next);
				p->next = NULL;
			}
			continue;
		}

		p->next = (link_list)malloc(sizeof(link_list_node));
		if ( p->next == NULL )
		{
			fprintf(stderr, "No sufficient memory\n");
			exit(EXIT_FAILURE);
		}
		p->next->next = NULL;
		p->next->prev = p;
		p = p->next;

		strncpy(p->directory_name, current_directory_name, MAX_PATH);
		p->directory_name[MAX_PATH - 1] = '\0';
	}
}

/**
 * @brief 销毁路径链表，释放内存
 * @param[in,out] ppath_list 路径链表
 */
void destroy_path_linklist(link_list * ppath_list)
{
	link_list p = NULL;
	link_list q = NULL;

	assert(ppath_list != NULL);

	p = *ppath_list;
	while ( p != NULL )
	{
		q = p;
		p = p->next;
		free(q);
	}
	*ppath_list = p = q = NULL;
}

/**
 * @brief 获取无.和..的路径名
 * @details 绝对路径中可能带有.和..，需要清理掉
 * @param[in,out] absolute_path 绝对路径
 */
void get_path_without_dot_or_dotdot(char * absolute_path)
{
	link_list path_list = NULL;
	link_list p = NULL;

	assert(absolute_path != NULL);
	assert(absolute_path[0] == '/');

	generate_path_linklist(&path_list, absolute_path);

	// generate the path
	strncpy(absolute_path, "/", 2);
	for ( p = path_list->next; p != NULL; p = p->next )
	{
		strncat(absolute_path, p->directory_name, MAX_PATH - strlen(absolute_path));
		absolute_path[MAX_PATH - 1] = '\0';
		strncat(absolute_path, "/", 2);
	}
	absolute_path[strlen(absolute_path) - 1] = '\0';

	if ( strlen(absolute_path) == 0 )
		strncpy(absolute_path, "/", 2);

	destroy_path_linklist(&path_list);
}

/**
 * @brief 获得绝对路径
 * @param[out] absolute_path 传出参数，需要获得的绝对路径
 * @param[in]  path 传入参数，路径，可以是相对路径也可以是绝对路径
 */
void get_absolute_path(char * absolute_path, const char * path)
{
	assert(absolute_path != NULL);
	assert(path != NULL);
	if ( path[0] != '/' )
	{
		strncpy(absolute_path, current_path, MAX_PATH);
		absolute_path[MAX_PATH - 1] = '\0';
	}
	else
		absolute_path[0] = '\0';

	if ( absolute_path[strlen(absolute_path)-1] != '/' )
		strncat(absolute_path, "/", MAX_PATH - strlen(absolute_path));
	strncat(absolute_path, path, MAX_PATH - strlen(absolute_path));
	absolute_path[MAX_PATH - 1] = '\0';
	get_path_without_dot_or_dotdot(absolute_path);
}
