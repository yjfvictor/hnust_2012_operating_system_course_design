/** 
 * @file    2-1.c 
 * @brief   实验二 清单2-1 
 * @details 创建子进程
 * @author  叶剑飞
 * @date    2014年6月13日
 */

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <locale.h>

// 最多允许创建的子进程个数。这里设置为5，最多只能创建5个。
const int c_nCloneMax = 5;

/**
 * @brief 克隆一个进程
 * @param nCloneID 克隆ID号
 */
void StartClone(int nCloneID)
{
	TCHAR szFilename[MAX_PATH];
	TCHAR szCommandLine[MAX_PATH];
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	BOOL bCreateOK = FALSE;

	// 获取当前进程的绝对路径
	GetModuleFileName(NULL, szFilename, MAX_PATH);

	// 构造新创建的进程的命令行参数
	_stprintf(szCommandLine, TEXT("\"%s\" %d"), szFilename, nCloneID);

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	// 创建新进程
	bCreateOK = CreateProcess(
		szFilename,           // 可执行文件的路径
		szCommandLine,        // 启动进程的命令行参数
		NULL,                 // 进程安全性
		NULL,                 // 线程安全性
		FALSE,                // 该句柄不可继承
		CREATE_NEW_CONSOLE,   // 在新的控制台中启动这个进程
		NULL,                 // 指向新创建的进程的环境块指针。NULL表示使用父进程的环境块变量。
		NULL,                 // 给新创建的进程赋予的当前目录的绝对路径，支持UNC路径。NULL表示用父进程的当前目录。
		&si,                  // 传入新进程的初始化参数信息
		&pi                   // 传出新进程的信息
		);
	if (bCreateOK)
	{
		// 释放子进程的进程句柄
		CloseHandle(pi.hProcess);
		// 释放子进程的主线程的线程句柄
		CloseHandle(pi.hThread);
	}
	else
		_ftprintf(stderr, TEXT("创建Clone ID为%d的进程失败\n"), nCloneID);
}

/**
 * @brief 主函数
 * @param argc 命令行参数的个数
 * @param argv 命令行参数向量
 * @return 成功返回EXIT_SUCCESS，失败返回EXIT_FAILURE 
 * @note 如果 argc > 1，那么argv[1]中储存的是克隆ID号
 */
int _tmain(int argc, TCHAR * argv[])
{
	// 确定派生出几个进程，及派生进程在进程列表中的位置
	int nClone = 0;
	setlocale(LC_ALL, "");

	if (argc > 1)
		_stscanf(argv[1], TEXT("%d"), &nClone);


	// 显示进程位置
	_tprintf(TEXT("Process ID: %lu\n"), GetCurrentProcessId());
	_tprintf(TEXT("Clone ID: %lu\n\n"), nClone);

	// 检查是否有创建子进程的需要
	if (nClone < c_nCloneMax)
		StartClone(++nClone);

	_gettchar();
	return EXIT_SUCCESS;
}
