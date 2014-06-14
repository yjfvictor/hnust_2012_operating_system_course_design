/*
 * 实验六 Linux进程间通信
 *
 *   任务一  通信方式： XSI IPC 消息队列
 *
 *                叶剑飞
 *                2014年6月14日
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// 消息的最长的长度
#define MAX_LENGTH 5

typedef struct
{
	long mtype;   // 消息类型，必须为正数
	char mtext[MAX_LENGTH]; // 发出的消息的数据
} msgbuf;

// 按照指导书上的要求，建立key为75的消息队列
const key_t key = 75;

void server(void)
{
	msgbuf msg;
	ssize_t msg_size;
	size_t msgsz;
	long msgtyp;

	// IPC_CREAT表示如果不存在这个key值的消息队列，则创建它
	// 八进制600，表示当前用户有读写权限
	int msgflg = IPC_CREAT | 0600;

	// 获得System V的消息队列标识符
	int msqid = msgget(key, msgflg);

	if ( msqid == -1 )
	{
		perror("(server) msgget failed");
		exit(EXIT_FAILURE);
	}

	msg.mtype = 2;  // 接收2号消息
	msgsz = MAX_LENGTH;
	msgtyp = 0;
	msgflg = MSG_NOERROR;  // 如果消息太长，则直接截段并丢弃掉超长部分
	do
	{
		// 接收消息
		msg_size = msgrcv( msqid, &msg, msgsz, msgtyp, msgflg);
		if ( msg_size == -1 )
			perror("(server) msgrcv failed");
		else
			puts("(server) received");
	} while ( msg.mtext[0] != 1 );
}

void client(void)
{
	int i; // 循环计数
	int msgflg = IPC_CREAT | 0600;
	int msqid = msgget(key, msgflg);
	msgbuf msg;
	size_t msgsz;

	if ( msqid == -1 )
	{
		perror("(client) msgget failed");
		exit(EXIT_FAILURE);
	}

	msg.mtype = 2;  // 发送2号消息
	msgsz = sizeof(msg.mtext);
	msgflg = 0;
	for ( i = 10; i > 0; -- i )
	{
		// 待发送的消息的数据
		msg.mtext[0] = (char)i;

		// 发送消息
		if (  msgsnd(msqid, &msg, msgsz, msgflg) == -1 )
			perror("(client) msgsnd failed");
		else
			puts("(client) sent");
	}
}

int main(void)
{
	pid_t fork_result, server_pid, client_pid;
	int server_status, client_status;

	struct msqid_ds buf;
	int cmd;
	int msgflg;
	int msqid;

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
			msgflg = IPC_CREAT | 0600;
			msqid = msgget(key, msgflg);
			cmd = IPC_RMID;
			if ( msgctl(msqid, cmd, &buf) == -1 )
			{
				perror("(client) msgctl IPC_RMID failed");
				exit(EXIT_FAILURE);
			}
			return EXIT_SUCCESS;
		}
	}
}
