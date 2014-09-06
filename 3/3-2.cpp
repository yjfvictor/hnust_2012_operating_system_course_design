/** 
 * @file       3-2.cpp 
 * @brief      实验三  任务二
 * @details    根据实验（1）中所熟悉的P、V原主对应的
 *             实际Windows API函数，并参考教材中读者、
 *             写者问题的算法原理，尝试利用Windows API函数
 *             实现第一类读者问题（读者优先）
 *
 * @author     叶剑飞
 * @date       2014年6月15日
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <locale.h>
#include <tchar.h>

#define SIZE_OF_BUFFER 100000

int readcount = 0;
HANDLE hReadSemaphore = NULL;
HANDLE hWriteSemaphore = NULL;

int data = 0;

/**
 * @brief 读数据
 */
void readData()
{
	_tprintf(TEXT("读取到数据：%d\n"), data);
}

/**
 * @brief 写数据
 */
void writeData()
{
	data = rand();
	_tprintf(TEXT("写入数据：%d\n"), data);
}

/**
 * @brief 读者线程
 * @param lpThreadParameter 线程参数（此处未被使用）
 * @return  正常退出，返回0
 */
DWORD WINAPI reader(LPVOID lpThreadParameter)
{
	srand((unsigned int)time(NULL));
	
	WaitForSingleObject(hReadSemaphore, INFINITE);
	++readcount;
	if (readcount == 1)
		WaitForSingleObject(hWriteSemaphore, INFINITE);
	ReleaseSemaphore(hReadSemaphore, 1, NULL);
	readData();
	WaitForSingleObject(hReadSemaphore, INFINITE);
	--readcount;
	if (readcount == 0)
		ReleaseSemaphore(hWriteSemaphore, 1, NULL);
	ReleaseSemaphore(hReadSemaphore, 1, NULL);
	return 0;
}

/**
 * @brief 写者线程
 * @param lpThreadParameter 线程参数（此处未被使用）
 * @return  正常退出，返回0
 */
DWORD WINAPI writer(LPVOID lpThreadParameter)
{
	srand((unsigned int)time(NULL));
	WaitForSingleObject(hWriteSemaphore, INFINITE);
	writeData();
	ReleaseSemaphore(hWriteSemaphore, 1, NULL);
	return 0;
}

/**
 * @brief 请求停止所有线程的线程
 * @param lpThreadParameter 线程参数（此处未被使用）
 * @return 正常退出，返回0
 */
DWORD WINAPI stopAllThreads(LPVOID lpThreadParameter)
{
	_gettchar();
	return 0;
}

/**
 * @brief 读者写者线程的线程启动函数
 * @param readerThread 读者线程的函数
 * @param writerThread 写者线程的函数
 */
void parbegin(LPTHREAD_START_ROUTINE readerThread, LPTHREAD_START_ROUTINE writerThread)
{
	DWORD dwThreadID = 0;
	

	HANDLE hStopAllThreads = CreateThread(NULL, 0, stopAllThreads, NULL, 0, &dwThreadID);
	if (hStopAllThreads == NULL)
	{
		_ftprintf(stderr, TEXT("创建线程失败。错误码是：0x%x\n"), GetLastError());
		exit(EXIT_FAILURE);
	}

	srand((unsigned int)time(NULL));
	while (WaitForSingleObject(hStopAllThreads, 0) == WAIT_TIMEOUT)
	{
		int randomNumber = rand();
		HANDLE hThread = NULL;
		if (randomNumber % 2)
			hThread = CreateThread(NULL, 0, readerThread, NULL, 0, &dwThreadID);
		else
			hThread = CreateThread(NULL, 0, writerThread, NULL, 0, &dwThreadID);

		if (hThread == NULL)
		{
			_ftprintf(stderr, TEXT("创建线程失败。错误码是：0x%x\n"), GetLastError());
			exit(EXIT_FAILURE);
		}
		CloseHandle(hThread);
		hThread = NULL;

		Sleep(randomNumber % 1000);
	}
	CloseHandle(hStopAllThreads);
	hStopAllThreads = NULL;
}

/**
 * @brief 主函数
 * @return 成功返回EXIT_SUCCESS，失败返回EXIT_FAILURE
 */
int _tmain()
{
	setlocale(LC_ALL, "");
	hReadSemaphore = CreateSemaphore(NULL, 1, SIZE_OF_BUFFER, NULL);
	if (hReadSemaphore == NULL)
	{
		_ftprintf(stderr, TEXT("只读信号量创建失败。错误码是：0x%x\n"), GetLastError() );
		return EXIT_FAILURE;
	}
	hWriteSemaphore = CreateSemaphore(NULL, 1, SIZE_OF_BUFFER, NULL);
	if (hWriteSemaphore == NULL)
	{
		_ftprintf(stderr, TEXT("只写信号量创建失败。错误码是：0x%x\n"), GetLastError());
		return EXIT_FAILURE;
	}

	parbegin(reader, writer);

	return EXIT_SUCCESS;
}
