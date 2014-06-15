/*
 *  实验五  熟悉Unix/Linux操作系统
 *
 *     任务三 使用C语言编写一个词频统计
 *            程序，使之能给出各个单词
 *            在输入文件中出现的次数。
 *
 *
 *             叶剑飞
 *             2014年6月15日
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_LENGTH 1000

// 定义一个“链表”类型的结构体
typedef struct LinkList
{
	char word[MAX_LENGTH];
	int count;
	struct LinkList * next;
} LinkList;

// 在链表中找到指定单词，返回其节点指针。若不存在，则返回NULL指针。
LinkList * locate( LinkList * head, const char * current_word )
{
	LinkList * p = NULL;
	for ( p = head; p != NULL; p = p->next )
	{
		if ( !strcmp(p->word, current_word) )
			return p;
	}
	return NULL;
}

// 在指定的链表节点的统计数自增一
void increase( LinkList * pos )
{
	assert( pos != NULL );
	++(pos->count);
}

// 把新单词加入链表中
bool append( LinkList ** phead, const char * current_word )
{
	LinkList * p = (LinkList *)malloc(sizeof(LinkList));
	if ( p == NULL )
		return false;
	strncpy(p->word, current_word, MAX_LENGTH);
	p->word[MAX_LENGTH-1] = '\0';
	p->count = 1;
	p->next = *phead;
	*phead = p;
	return true;
}

// 输出统计结果
void outputStatistic(const LinkList * head)
{
	const LinkList * p = NULL;
	for( p = head; p != NULL; p = p->next )
		printf("%s %d\n", p->word, p->count);
}

// 释放链表的虚拟内存地址空间
void destroyLinkList(LinkList ** phead)
{
	LinkList * p = *phead;
	LinkList * q = NULL;
	*phead = NULL;
	while ( p != NULL )
	{
		q = p->next;
		free(p);
		p = q;
	}
}

int main(void)
{
	// 储存最近一次scanf读入的数据
	static char current_word[MAX_LENGTH];

	// 链表的头节点
	LinkList * head = NULL;

	// 定位指定单词的节点
	LinkList * pos = NULL;

	while ( scanf("%100s", current_word) != EOF )
	{
		if ( (pos = locate(head, current_word)) != NULL )
			increase(pos);
		else
		{
			if ( !append(&head, current_word) )
			{
				fprintf(stderr, "No sufficient memory!!!\n");
				abort();
			}
		}
	}
	outputStatistic(head);
	destroyLinkList(&head);
	return EXIT_SUCCESS;
}
