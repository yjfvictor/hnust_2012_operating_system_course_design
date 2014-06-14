/*
 *  实验二 清单2-2 父子进程的简单通信及终止的示例程序
 *
 *
 *                            叶剑飞
 *                            2014年6月13日
 */

#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <stdlib.h>
#include <locale.h>
#include <Windows.h>

// 互斥体的名字
static LPCTSTR g_szMutexName = TEXT("w2kdg.ProcTerm.mutex.Suicide");

// 创建子进程的函数
BOOL StartClone(void)
{
	TCHAR szFilename[MAX_PATH];
	TCHAR szCommandLine[MAX_PATH];
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	BOOL bCreateOK;

	// 获得当前进程的可执行文件的文件名
	GetModuleFileName(NULL, szFilename, MAX_PATH);

	// 构造命令行参数
	_stprintf(szCommandLine, TEXT("\"%s\" child"), szFilename);
	// 实验2-2步骤3：将上句中的字符串"child"改为别的字符串，重新编译执行

	// 注意注意：上面这个操作会导致每个子进程都调用Parent函数，从而导致无限开进程，最终进程资源耗尽，死机！！！
	// 操作之前请三思！！！如果一定要操作，请确保已经保存好所有重要资料！！！

	// 初始化进程启动信息
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

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
		_ftprintf(stderr, TEXT("创建子进程失败\n"));

	return bCreateOK;
}

void Child(void)
{
	DWORD dwLastError = 0;

	// 打开互斥体，返回这个互斥体的句柄。
	HANDLE hMutexSuicide = OpenMutex(
		SYNCHRONIZE,      // 申请获取同步权限的互斥体句柄
		FALSE,            // 该句柄不可继承
		g_szMutexName     // 互斥体的名字
		);
	if (hMutexSuicide == NULL)
	{
		dwLastError = GetLastError();
		if (dwLastError == ERROR_FILE_NOT_FOUND)
			_ftprintf(stderr, TEXT("打开名为%s的互斥体失败。该互斥体不存在。\n"), g_szMutexName);
		exit(EXIT_FAILURE);
	}

	_tprintf(TEXT("子进程正在等待父进程发送关闭命令\n"));

	// 子进程进入阻塞状态，等待父进程释放互斥体
	WaitForSingleObject(hMutexSuicide, INFINITE);
	// 实验2-2步骤5，将上句改为WaitForSingleObject(hMutexSuicide, 0);重新编译执行
	// 预告：上述操作的结果就是子进程立刻退出。不等待父进程释放互斥体

	_tprintf(TEXT("子进程正在退出\n"));

	// 关闭互斥体句柄
	CloseHandle(hMutexSuicide);

	// 结束自身进程
	exit(EXIT_SUCCESS);
}

void Parent(void)
{
	DWORD dwLastError = 0;

	// 创建互斥体
	HANDLE hMutexSuicide = CreateMutex(
		NULL,          // 互斥体的安全性参数。NULL表示取缺省值
		TRUE,          // 对该互斥体拥有所有权
		g_szMutexName  // 互斥体的名字
		);
	if (hMutexSuicide ==NULL)
	{
		dwLastError = GetLastError();
		if (dwLastError == ERROR_ALREADY_EXISTS)
			_ftprintf(stderr, TEXT("创建名为%s的互斥体失败。该名字已被占用。\n"), g_szMutexName);
		else if (dwLastError == ERROR_ACCESS_DENIED)
			_ftprintf(stderr, TEXT("无权限创建名为%s的互斥体\n"), g_szMutexName);
		else
			_ftprintf(stderr, TEXT("互斥体名为%s的互斥体创建失败，错误码为0x%x。\n"), g_szMutexName, dwLastError);
		exit(EXIT_FAILURE);
	}

	_tprintf(TEXT("开始创建子进程\n"));

	// 创建子进程
	if (!StartClone())
	{
		_tprintf(TEXT("创建子进程失败\n"));
		exit(EXIT_FAILURE);
	}

	_tprintf(TEXT("请按下回车键关闭子进程\n"));

	// 等待父进程的键盘响应
	_gettchar();

	// 释放互斥体。这个信号会发送给子进程的WaitForSingleObject函数
	ReleaseMutex(hMutexSuicide);

	// 关闭互斥体句柄
	CloseHandle(hMutexSuicide);
}

int _tmain(int argc, TCHAR * argv[])
{
	setlocale(LC_ALL, "");
	if (argc > 1 && !_tcscmp(argv[1], TEXT("child")))
		Child();
	else
		Parent();
	return EXIT_SUCCESS;
}