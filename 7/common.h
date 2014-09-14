/**
 * @file   common.h
 * @brief  通用的一些外部变量与一些通用的宏
 * @author 叶剑飞
 */

#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>

/**
 * 路径被分隔的段数的最大值
 */
#define MAX_SEPERATORS 128

/**
 * 文件名的长度的最大值
 */
#define MAX_NAME 256

/**
 * 文件路径的长度的最大值
 */
#define MAX_PATH 2048

/**
 * 输入的一条命令的最长的长度
 */
#define MAX_COMMAND_LENGTH 4096


char username[MAX_NAME];     /**< 登录用户名 */
char hostname[MAX_NAME];     /**< 登录主机名 */
char current_path[MAX_PATH]; /**< 当前工作目录 */
char lastError[MAX_NAME];    /**< 最后一条错误信息 */
int  fd_ext2;                /**< ext2文件系统的文件描述符 */

#endif // _COMMON_H
