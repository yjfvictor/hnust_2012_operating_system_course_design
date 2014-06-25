/*
 *   处理Shell命令的函数声明
 *
 *       叶剑飞
 *
 */

#ifndef _DEAL_COMMAND_H
#define _DEAL_COMMAND_H

bool ls( bool all, bool almost_all, bool long_list, const char * const * const list_path );
bool cd(const char * path);
bool cat( const char * path );
bool rm( const char * const * const remove_paths, bool recursive, bool force );

#endif // _DEAL_COMMAND_H
