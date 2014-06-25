/*
 *  目录链表的数据结构
 *
 *           叶剑飞
 *
 */

#ifndef _ALGORITHM_DATA_STRUCTURE
#define _ALGORITHM_DATA_STRUCTURE

#include "common.h"

typedef struct link_list
{
	char directory_name[MAX_PATH];
	struct link_list * prev;
	struct link_list * next;
} link_list_node;

typedef link_list_node * link_list;

#endif // _ALGORITHM_DATA_STRUCTURE
