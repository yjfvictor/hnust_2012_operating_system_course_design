/** 
 * @file    5-4.c
 * @brief   实验五 熟悉Unix/Linux操作系统 任务四
 * @details 用C语言编写一个反向打印的
 *          程序，使之能够按与输入文件
 *          中的文本行相反的次序来打印
 * @author  叶剑飞
 * @date    2014年6月15日
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LENTH 1000000000

typedef struct LinkList
{
	char * line;
	struct LinkList * next;
} LinkList;

/**
 * @brief 链表追加函数 
 * @details 把新增的一行，插入到链表的头部 
 * @param phead 链表头指针
 * @param line  追加的某一行
 * @return 成功返回true，失败返回false
 */
bool append(LinkList ** phead, const char * line)
{
	size_t len = strlen(line) + 1;
	char * buffer = (char *)malloc(len);
	LinkList * current_node = NULL;
	if ( buffer == NULL )
		return false;
	strncpy(buffer, line, len);

	current_node = (LinkList *)malloc(sizeof(LinkList));
	if ( current_node == NULL )
	{
		free(buffer);
		return false;
	}
	current_node->line = buffer;
	current_node->next = *phead;
	*phead = current_node;
	return true;
}

/**
 * @brief 输出全部行
 * @param head 链表的表头
 */
void outputAllLines( const LinkList * head )
{
	const LinkList * p = NULL;
	for ( p = head; p != NULL; p = p->next )
		puts(p->line);
}

/**
 * @brief 销毁链表
 * @param head 链表的表头指针的地址
 */
void destroyLinkList(LinkList ** head)
{
	LinkList * p = *head;
	LinkList * q = NULL;
	while ( p != NULL )
	{
		q = p->next;
		free(p);
		p = q;
	}
}

/**
 * @brief 主函数
 * @return 成功返回EXIT_SUCCESS，失败返回EXIT_FAILURE
 */
int main(void)
{
	LinkList * head = NULL;
	static char current_line[MAX_LENTH];
	while ( fgets(current_line, MAX_LENTH, stdin) != NULL )
	{
		current_line[MAX_LENTH-1] = '\0';
		current_line[strlen(current_line)-1] = '\0';
		if ( !append(&head, current_line) )
		{
			fprintf(stderr, "No sufficient memory\n");
			abort();
		}
	}
	outputAllLines(head);
	destroyLinkList(&head);
	return EXIT_SUCCESS;
}
