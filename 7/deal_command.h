/**
 * @file  deal_command.h
 * @brief  处理Shell命令的函数声明
 * @author 叶剑飞
 */

#ifndef _DEAL_COMMAND_H
#define _DEAL_COMMAND_H

/**
 * @brief 执行ls命令 
 * @details 列出指定目录的多个文件
 * @param all 是否输出全部文件，包括隐藏文件，以及.和..
 * @param almost_all 是否“几乎全部”输出，包括隐藏文件，不包括.和..
 * @param long_list  是否按长列表方式输出
 * @param list_path  需要列出文件的目录数组
 * @return bool  成功返回true，失败返回false
 */
bool ls( bool all, bool almost_all, bool long_list, const char * const * const list_path );

/**
 * @brief 执行cd命令
 * @details 把当前目录切换到指定目录
 * @param path 需要切换到的目录
 * @return 成功返回true，失败返回false
 */
bool cd(const char * path);

/**
 * @brief 执行cat命令 
 * @details 输出指定文件的全部内容
 * @param path 指定文件的路径
 * @return 成功返回true，失败返回false
 */
bool cat( const char * path );

/**
 * @brief 执行rm命令 
 * @details 删除指定文件
 * @param remove_paths 需要删除的文件的路径
 * @param recursive    是否递归删除
 * @param force        是否强行删除只读文件
 * @return 成功返回true，失败返回false
 */
bool rm( const char * const * const remove_paths, bool recursive, bool force );

#endif // _DEAL_COMMAND_H
