/*
 * 实验五 熟悉Unix/Linux操作系统
 *
 *    任务四 用C语言编写一个反向打印的
 *           程序，使之能够按与输入文件
 *           中的文本行相反的次序来打印
 *
 *                 叶剑飞
 *                 2014年6月15日
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

void outputAllLines( const LinkList * head )
{
	const LinkList * p = NULL;
	for ( p = head; p != NULL; p = p->next )
		puts(p->line);
}

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
