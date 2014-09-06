/**
 * @file  parse_string.h
 * @brief 字符串解析的函数声明
 * @author 叶剑飞
 */

#ifndef _PARSE_STRING_H
#define _PARSE_STRING_H

#include "algorithm_data_structure.h"

/**
 * @brief 把一条命令分隔成命令行参数列表
 * @param command 命令字符串
 * @param pargc   命令行参数的个数
 * @param pargv   存放命令行参数的数组
 * @return 成功返回true，失败返回false
 */
bool split_command(const char * command, int * pargc, char *** pargv);

/**
 * @brief 运行命令
 * @param argc 命令行参数的个数
 * @param argv 命令行参数的数组
 * @return 成功返回true，失败返回false
 */
bool run_command(int argc, char * const * argv);

/**
 * @brief 输出最后一个错误
 */
void print_last_error(void);

/**
 * @brief 获取无.和..的路径名
 * @details 绝对路径中可能带有.和..，需要清理掉
 * @param absolute_path 绝对路径
 */
void get_path_without_dot_or_dotdot(char * absolute_path);

/**
 * @brief 获得绝对路径
 * @param absolute_path 传出参数，需要获得的绝对路径
 * @param path 传入参数，路径，可以是相对路径也可以是绝对路径
 */
void get_absolute_path(char * absolute_path, const char * path);

/**
 * @brief 销毁路径链表，释放内存
 * @param ppath_list 路径链表
 */
void destroy_path_linklist(link_list * ppath_list);

/**
 * @brief 从绝对路径中生成路径链表
 * @details 把路径的分隔符'/'找出来，然后一级一级地进行分隔，每级放在一个链表节点中
 * @param ppath_list 输出参数，路径链表
 * @param absolute_path 输入参数，绝对路径
 */
void generate_path_linklist(link_list * ppath_list, const char * absolute_path);

#endif // _PARSE_STRING_H
