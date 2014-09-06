/**
 * @file    3-1.cpp
 * @brief   实验三 清单3-1 
 * @details 生产者消费者问题
 * @author  叶剑飞
 * @date    2014年6月13日
 */

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <tchar.h>
#include <locale.h>

const unsigned short SIZE_OF_BUFFER = 2;       // 缓冲区长度
unsigned short productID = 0;                  // 产品号
unsigned short consumeID = 0;                  // 将被消耗的产品号
unsigned short in = 0;                         // 产品输入缓冲区时缓冲区下标
unsigned short out = 0;                        // 产品输出缓冲区的缓冲区下标

int buffer[SIZE_OF_BUFFER];                    // 缓冲区是个循环队列
volatile bool p_ccontinue = true;              // 控制程序结束
HANDLE hMutex;                                 // 用于线程间的互斥
HANDLE hFullSemaphore;                         // 当缓冲区满时迫使生产者等待
HANDLE hEmptySemaphore;                        // 当缓冲区空时迫使消费者等待

/**
 * @brief 生产一个产品
 */
void Produce()
{
	// 输出产品ID号，产品自增一
	_tprintf(TEXT("Producing %d ... "), ++productID);
	_tprintf(TEXT("Succeed\n"));
}

/**
 * @brief 把新生产的产品放入缓冲区
 */
void Append()
{
	_ftprintf(stderr, TEXT("Appending a product ... "));
	buffer[in] = productID;
	in = (in + 1) % SIZE_OF_BUFFER;
	_ftprintf(stderr, TEXT("Succeed\n"));

	// 输出缓冲区的当前状态
	for (int i = 0; i < SIZE_OF_BUFFER; ++i)
	{
		_tprintf(TEXT("%d: %d"), i, buffer[i]);
		if (i == in)
			_tprintf(TEXT(" <-- 生产"));
		if (i == out)
			_tprintf(TEXT(" <-- 消费"));
		_tprintf(TEXT("\n"));
	}
}

/**
 * @brief 从缓冲区中取出一个产品
 */
void Take()
{
	_ftprintf(stderr, TEXT("Taking a product ..."));
	consumeID = buffer[out];
	buffer[out] = 0;
	out = (out + 1) % SIZE_OF_BUFFER;
	_ftprintf(stderr, TEXT("Succeed\n"));

	// 输出缓冲区的当前状态
	for (int i = 0; i < SIZE_OF_BUFFER; ++i)
	{
		_tprintf(TEXT("%d: %d"), i, buffer[i]);
		if (i == in)
			_tprintf(TEXT(" <-- 生产"));
		if (i == out)
			_tprintf(TEXT(" <-- 消费"));
		_tprintf(TEXT("\n"));
	}
}

/**
 * @brief 消耗一个产品
 */
void Consume()
{
	_tprintf(TEXT("Consuming %d ... "), consumeID);
	_tprintf(TEXT("Succeed\n"));
}

/**
 * @brief 生产者线程
 * @param lpParameter 线程参数，此处未被使用
 * @return 若正常退出，则返回0
 */
DWORD WINAPI Producer(LPVOID lpParameter)
{
	while (p_ccontinue)
	{
		WaitForSingleObject(hEmptySemaphore, INFINITE);     // P(empty)
		WaitForSingleObject(hMutex, INFINITE);              // P(mutex)
		Produce();
		Append();
		Sleep(1500);
		ReleaseMutex(hMutex);                               // V(mutex)
		ReleaseSemaphore(hFullSemaphore, 1, NULL);          // V(full)
	}
	return 0;
}

/**
 * @brief 消费者线程
 * @param lpParameter 线程参数，此处未被使用
 * @return 若正常退出，则返回0
 */
DWORD WINAPI Consumer(LPVOID lpParameter)
{
	while (p_ccontinue)
	{
		WaitForSingleObject(hFullSemaphore, INFINITE);      // P(full)
		WaitForSingleObject(hMutex, INFINITE);              // P(mutex)
		Take();
		Consume();
		Sleep(1500);
		ReleaseMutex(hMutex);                               // V(mutex)
		ReleaseSemaphore(hEmptySemaphore, 1, NULL);         // V(empty)
	}
	return 0;
}

/**
 * @brief 主函数
 * @return 成功返回EXIT_SUCCESS，失败返回EXIT_FAILURE
 */
int _tmain()
{
	::setlocale(LC_ALL, "");

	// 创建各个互斥信号
	hMutex = CreateMutex(NULL, FALSE, NULL);
	hEmptySemaphore = CreateSemaphore(NULL, SIZE_OF_BUFFER, SIZE_OF_BUFFER, NULL);
	// 将上句做如下修改，看看结果会怎么样
	// hEmptySemaphore = CreateSemaphore(NULL, 0, SIZE_OF_BUFFER - 1, NULL);
	hFullSemaphore = CreateSemaphore(NULL, 0, SIZE_OF_BUFFER, NULL);

	// 调整下面的数值，可以发现，当生产者个数多于消费者个数时，
	// 生产速度快，生产者经常等待消费者；反之，消费者经常等待
	const unsigned short PRODUCERS_COUNT = 3;    // 生产者的个数
	const unsigned short CONSUMERS_COUNT = 1;    // 消费者的个数

	// 总的线程数
	const unsigned short THREADS_COUNT = PRODUCERS_COUNT + CONSUMERS_COUNT;

	HANDLE hThreads[THREADS_COUNT];              // 各线程的句柄
	DWORD dwProducerID[PRODUCERS_COUNT];         // 生产者线程的线程标识符
	DWORD dwConsumerID[CONSUMERS_COUNT];         // 消费者线程的线程标识符

	// 把进程句柄全部置0
	ZeroMemory(hThreads, sizeof(hThreads));

	// 创建生产者线程
	for (int i = 0; i < PRODUCERS_COUNT; ++i)
	{
		hThreads[i] = CreateThread(NULL, 0, Producer, NULL, 0, &dwProducerID[i]);
		if (hThreads[i] == NULL)
			_ftprintf(stderr, TEXT("线程%d创建失败\n"), (PRODUCERS_COUNT + i));
	}
	// 创建消费者线程
	for (int i = 0; i < CONSUMERS_COUNT; ++i)
	{
		hThreads[PRODUCERS_COUNT + i] = CreateThread(NULL, 0, Consumer, NULL, 0, &dwConsumerID[i]);
		if (hThreads[PRODUCERS_COUNT + i] == NULL)
			_ftprintf(stderr, TEXT("线程%d创建失败\n"), (PRODUCERS_COUNT + i));
	}

	while (p_ccontinue)
	{
		if (_gettchar())   // 按回车键后终止程序运行
			p_ccontinue = false;
	}

	/*
	// 等待生产者线程结束
	for (int i = 0; i < PRODUCERS_COUNT; ++i)
	{
		WaitForSingleObject(hThreads[i], INFINITE);
		CloseHandle(hThreads[i]);
		hThreads[i] = NULL;
	}
	// 等待消费者线程结束
	for (int i = 0; i < CONSUMERS_COUNT; ++i)
	{
		WaitForSingleObject(hThreads[PRODUCERS_COUNT + i], INFINITE);
		CloseHandle(hThreads[PRODUCERS_COUNT + i]);
		hThreads[PRODUCERS_COUNT + i] = NULL;
	}
	*/

	return EXIT_SUCCESS;
}