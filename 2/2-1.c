/*
 *  实验二 清单2-1 创建子进程
 *
 *
 *               叶剑飞
 *              2014年6月13日
 */


#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <locale.h>

// 最多允许创建的子进程个数。这里设置为5，最多只能创建5个。
const int c_nCloneMax = 5;


// 创建传递过来的进程的克隆过程并赋予其ID值
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
		_ftprintf(stderr, TEXT("创建Clone ID为%d的进程失败"), nCloneID);
}

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
