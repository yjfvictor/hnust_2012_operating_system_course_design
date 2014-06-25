/*
 *
 *  处理Shell命令的各个函数的实现
 *
 *
 *                          叶剑飞
 *                          2014年6月
 *
 *
 */

#include "common.h"
#include "ext2.h"
#include "parse_string.h"

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

bool cat( const char * path )
{
	char absolute_path[MAX_PATH];
	assert(path != NULL);
	get_absolute_path(absolute_path, path);
	return output_file_data(absolute_path);
}

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
