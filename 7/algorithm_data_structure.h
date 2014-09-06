/**
 * @brief  目录链表的数据结构
 * @author 叶剑飞
 */

#ifndef _ALGORITHM_DATA_STRUCTURE
#define _ALGORITHM_DATA_STRUCTURE

#include "common.h"

// 链表的一个结点结构
typedef struct link_list_node
{
	char directory_name[MAX_PATH];
	struct link_list_node * prev;
	struct link_list_node * next;
} link_list_node;

// 链表结构
typedef link_list_node * link_list;

#endif // _ALGORITHM_DATA_STRUCTURE
