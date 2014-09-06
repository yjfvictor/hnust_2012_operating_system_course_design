/**
 * @file   deal_command.c
 * @brief  处理Shell命令的各个函数的实现
 * @author 叶剑飞
 * @date   2014年6月
 */

#include "common.h"
#include "ext2.h"
#include "parse_string.h"

/**
 * @brief 执行ls命令 
 * @details 列出指定目录的多个文件
 * @param all 是否输出全部文件，包括隐藏文件，以及.和..
 * @param almost_all 是否“几乎全部”输出，包括隐藏文件，不包括.和..
 * @param long_list  是否按长列表方式输出
 * @param list_path  需要列出文件的目录数组
 * @return bool  成功返回true，失败返回false
 */
bool ls( bool all, bool almost_all, bool long_list, const char * const * const list_path )
{
	char absolute_path[MAX_PATH];
	const char * const * p = NULL;

	if ( list_path == NULL )
		output_files(current_path, all, almost_all, long_list);
	else
	{
		for ( p = list_path; (*p) != NULL; ++ p )
		{
			get_absolute_path(absolute_path, *p);
			output_files(absolute_path, all, almost_all, long_list);
		}
	}
	return true;
}

/**
 * @brief 执行cd命令
 * @details 把当前目录切换到指定目录
 * @param path 需要切换到的目录
 * @return 成功返回true，失败返回false
 */
bool cd(const char * path)
{
	char absolute_path[MAX_PATH];
	assert(path != NULL);
	get_absolute_path(absolute_path, path);

	if ( exsit_dir_path(absolute_path) )
	{
		strncpy(current_path, absolute_path, sizeof(current_path));
		current_path[sizeof(current_path) - 1] = '\0';
		return true;
	}
	else
	{
		sprintf(lastError, "cd: %s: No such directory", path);
		return false;
	}
}

/**
 * @brief 执行cat命令 
 * @details 输出指定文件的全部内容
 * @param path 指定文件的路径
 * @return 成功返回true，失败返回false
 */
bool cat( const char * path )
{
	char absolute_path[MAX_PATH];
	assert(path != NULL);
	get_absolute_path(absolute_path, path);
	return output_file_data(absolute_path);
}

/**
 * @brief 执行rm命令 
 * @details 删除指定文件
 * @param remove_paths 需要删除的文件的路径
 * @param recursive    是否递归删除
 * @param force        是否强行删除只读文件
 * @return 成功返回true，失败返回false
 */
bool rm( const char * const * const remove_paths, bool recursive, bool force )
{
	char absolute_path[MAX_PATH];
	const char * const * p = NULL;

	assert(remove_paths != NULL);
	for ( p = remove_paths; (*p) != NULL; ++ p )
	{
		get_absolute_path(absolute_path, *p);
		if ( !remove_file(absolute_path) )
			fprintf(stderr, "rm: cannot remove ‘%s’: No such file or directory\n", (*p));
	}
	return true;
}
