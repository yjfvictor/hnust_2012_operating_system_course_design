/** 
 * @file   algorithm_data_structure.h 
 * @brief  目录链表的数据结构
 * @author 叶剑飞
 */

#ifndef _ALGORITHM_DATA_STRUCTURE_H_
#define _ALGORITHM_DATA_STRUCTURE_H_

#include "common.h"

/** 链表的一个结点结构 */
typedef struct link_list_node
{
	char directory_name[MAX_PATH]; /**< 目录名 */
	struct link_list_node * prev;  /**< 链表前驱 */
	struct link_list_node * next;  /**< 链表后继 */
} link_list_node;

/** 链表结构 */
typedef link_list_node * link_list;

#endif // _ALGORITHM_DATA_STRUCTURE_H_
