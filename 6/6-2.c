/** 
 * @file    6-2.c 
 * @brief   实验六 Linux进程间通信 任务二 
 * @details 通信方式： XSI IPC 共享内存
 * @author  叶剑飞
 * @date    2014年6月14日
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// 消息的最长的长度
#define MAX_LENGTH 8

// 按照指导书上的要求，建立key为75的消息队列
const key_t key = 75;

/**
 * @brief 服务器端
 */
void server(void)
{
	// 进程间共享内存的映射区
	volatile char * shmaddr;
	// 这里使用volatile是避免编译器优化
	// 因为它的修改是来自另一个进程的进程的修改
	// 如果不用volatile，没做“左值”(lvalue)的代码段可能会被编译器优化到寄存器中

	// IPC_CREAT表示如果不存在这个key值的消息队列，则创建它
	// 八进制600，表示当前用户有读写权限
	int shmflg = IPC_CREAT | 0600;

	size_t size = MAX_LENGTH;

	// 分配System V共享内存段
	int shmid = shmget(key, size, shmflg);

	if ( shmid == -1 )
	{
		perror("(server) shmget failed");
		exit(EXIT_FAILURE);
	}

	// 把进程间共享内存映射到进程内存中
	shmflg = 0;
	shmaddr = (volatile char *)shmat(shmid, NULL, shmflg);
	if ( shmaddr == (void *)(-1) )
	{
		perror("(server) shmat failed");
		exit(EXIT_FAILURE);
	}

	do
	{
		// 向共享内存中写入数据
		shmaddr[0] = (char)(-1);

		// 等待共享内存中的数据发生改变
		while ( shmaddr[0] == (char)(-1) )
		{ }
		puts("(server) received");
	} while ( shmaddr[0] != 0 );

	// 释放共享内存映射区的内存
	if ( shmdt((const void *)shmaddr) == -1 )
		perror("(server) shmdt failed");
	shmaddr = NULL;
}

/**
 * @brief 客户端
 */
void client(void)
{
	volatile char * shmaddr;
	int i;  // 循环计数
	int shmflg = IPC_CREAT | 0600;
	size_t size = MAX_LENGTH;
	int shmid = shmget(key, size, shmflg);

	if ( shmid == -1 )
	{
		perror("(client) shmget failed");
		exit(EXIT_FAILURE);
	}

	shmflg = 0;
	shmaddr = (volatile char *)shmat(shmid, NULL, shmflg);
	if ( shmaddr == (void *)(-1) )
	{
		perror("(client) shmat failed");
		exit(EXIT_FAILURE);
	}

	for ( i = 9; i >= 0; -- i )
	{
		// 等待共享内存中的数据发生改变
		while ( shmaddr[0] != (char)(-1) )
		{ }
		// 向共享内存中写入数据
		shmaddr[0] = i;
		puts("(client) sent");
	}

	if ( shmdt((const void *)shmaddr) == -1 )
		perror("(client) shmdt failed");
	shmaddr = NULL;
}

/**
 * @brief 主函数 
 * @return 成功返回EXIT_SUCCESS，失败返回EXIT_FAILURE
 */
int main(void)
{
	pid_t fork_result, server_pid, client_pid;
	int server_status, client_status;

	size_t size;
	int shmflg, shmid, cmd;
	struct shmid_ds buf;

	// 创建子进程——进程分叉
	if ( (fork_result = fork()) == -1 )
	{
		// fork的返回值为-1,表示进程创建失败
		perror("(parent) fork failed");
		return EXIT_FAILURE;
	}
	else if ( fork_result == 0 )
	{
		// fork返回值为0的，表示当前进程为子进程
		// 第一个子进程调用server函数
		server();
		return EXIT_SUCCESS;
	}
	else
	{
		// fork返回值为正数，表示当前进程为父进程
		// 返回值代表该父进程新创建的子进程的进程标识符
		server_pid = fork_result;
		if ( (fork_result = fork()) == -1 )
		{
			perror("(parent) fork failed");
			return EXIT_FAILURE;
		}
		else if ( fork_result == 0 )
		{
			// 第二个子进程调用client函数
			client();
			return EXIT_SUCCESS;
		}
		else
		{
			client_pid = fork_result;

			// 等待进程标识符为client_pid的子进程结束，退出状态储存在client_status变量中
			if ( waitpid(client_pid, &client_status, 0) == -1 )
				perror("(parent) wait for client failed");
			else
			{
				// WEXITSTATUS(client_status) 是进程退出码（即main函数的返回值）
				printf("(parent) the exit code of client process is %d\n", WEXITSTATUS(client_status));
			}
			if ( waitpid(server_pid, &server_status, 0) == -1 )
				perror("(parent) wait for server failed");
			else
				printf("(parent) the exit code of server process is %d\n", WEXITSTATUS(server_status));

			// 在msgctl函数中传入IPC_RMID参数，表示删除掉指定消息队列
			shmflg = IPC_CREAT | 0600;
			size = MAX_LENGTH;
			shmid = shmget(key, size, shmflg);

			cmd = IPC_RMID;
			if ( shmctl(shmid, cmd, &buf) == -1 )
			{
				perror("(parent) shmctl IPC_RMID failed");
				exit(EXIT_FAILURE);
			}

			return EXIT_SUCCESS;
		}
	}
}
