/**
 * @file    5-3.c
 * @brief   实验五  熟悉Unix/Linux操作系统 任务三
 * @details 使用C语言编写一个词频统计
 *          程序，使之能给出各个单词
 *          在输入文件中出现的次数。
 * @author  叶剑飞
 * @date    2014年6月15日
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

/**
 * @brief              在链表中找到指定的单词，获取这个节点
 * @param head         链表头
 * @param current_word 需要查找的指定单词
 * @return             返回找到指定单词的链表节点的地址。若不存在，则返回NULL。
 */
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

/** 
 * @brief   自增函数 
 * @details 在链表指定的节点处，统计数自增一
 * @param pos 链表的指定节点的地址
 */
void increase( LinkList * pos )
{
	assert( pos != NULL );
	++(pos->count);
}

/**
 * @brief              链表追加函数
 * @details            把新单词加入链表中
 * @param phead        链表头指针
 * @param current_word 待加入的新单词
 * @return             加入成功返回true，否则返回false
 */
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

/**
 * @brief 输出统计结果
 * @param head 链表头节点
 */
void outputStatistic(const LinkList * head)
{
	const LinkList * p = NULL;
	for( p = head; p != NULL; p = p->next )
		printf("%s %d\n", p->word, p->count);
}

/**
 * @brief 释放链表的虚拟内存地址空间
 * @param phead 链表的表头指针的地址
 */
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

/**
 * @brief 主函数 
 * @return 若成功，则返回EXIT_SUCCESS，否则返回EXIT_FAILURE
 */
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
