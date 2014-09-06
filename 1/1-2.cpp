/** 
 * @file    1-2.cpp 
 * @brief   实验一  清单1-2 
 * @details 内核态运行时间与用户态运行时间比的计算
 * @author  叶剑飞
 * @date    2014年6月13日
 */

#include <cstdlib>
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <clocale>

/**
 * @brief 获取内核态时间的百分比
 * @param ftKernel 内核态时间
 * @param ftUser   用户态时间
 * @return 内核态时间的百分比
 */
DWORD GetKernelModePercentage(const FILETIME & ftKernel, const FILETIME & ftUser)
{
	// 将FILETIME结构转化为64位整数
	ULONGLONG qwKernel = (((ULONGLONG)ftKernel.dwHighDateTime) << 32) + ftKernel.dwLowDateTime;
	ULONGLONG qwUser = (((ULONGLONG)ftUser.dwHighDateTime) << 32) + ftUser.dwLowDateTime;
	ULONGLONG qwTotal = qwKernel + qwUser;

	// 将消耗时间相加，然后计算消耗在内核态下的时间百分比
	DWORD dwPct = (DWORD)(((ULONGLONG)100 * qwKernel) / qwTotal);
	return dwPct;
}

/**
 * @brief 主函数 
 * @param argc 命令行参数的个数，必须为2
 * @param argv 命令行参数向量，第2个参数为需要查询的进程名
 * @return 成功返回EXIT_SUCCESS，失败返回EXIT_FAILURE
 */
int _tmain(int argc, TCHAR * argv[])
{
	DWORD dwLastError = 0;
	::setlocale(LC_ALL, "");
	if (argc < 2)
	{
		_putts(TEXT("请给出你要查询的程序名"));
		return EXIT_FAILURE;
	}

	// 对当前系统中运行的过程抓取“快照”
	HANDLE hSnapshot = ::CreateToolhelp32Snapshot(
		TH32CS_SNAPPROCESS,    // 提取全部进程
		0					   // 前一个参数为TH32CS_SNAPPROCESS时，该参数被忽略
		);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		dwLastError = ::GetLastError();
		_tprintf(TEXT("CreateToolhelp32Snapshot函数运行失败，错误码为0x%x\n"), dwLastError);
		return EXIT_FAILURE;
	}

	// 初始化进程入口
	PROCESSENTRY32 pe;
	::ZeroMemory(&pe, sizeof(pe));
	pe.dwSize = sizeof(pe);
	BOOL bMore = ::Process32First(hSnapshot, &pe);
	bool found = false;
	while (bMore)
	{
		// 打开用于读取的进程
		if (!_tcscmp(pe.szExeFile, argv[1]))
		{
			found = true;
			HANDLE hProcess = ::OpenProcess(
				PROCESS_QUERY_INFORMATION,    // 申请返回一个拥有查询进程信息权限的进程句柄
				FALSE,     // 不必继承这一句柄
				pe.th32ProcessID    // 要打开的进程的“进程标识符”（PID）
				);
			if (hProcess == NULL)
			{
				dwLastError = ::GetLastError();
				_tprintf(TEXT("打开进程标识符为%lu的进程失败，错误码为0x%x\n"), pe.th32ProcessID, dwLastError);
				if (dwLastError == ERROR_ACCESS_DENIED)
					_tprintf(TEXT("无权限访问这个进程\n"));
				_tprintf(TEXT("\n"));
			}
			else
			{
				FILETIME ftCreation, ftKernelMode, ftUserMode, ftExit;

				// 获取进程时间
				::GetProcessTimes(
					hProcess,           // 需要得到进程信息的进程句柄
					&ftCreation,        // 进程的启动时间
					&ftExit,            // 进程的结束时间
					&ftKernelMode,      // 进程的内核态下消耗的时间
					&ftUserMode         // 进程的用户态下消耗的时间
					);

				// 计算内核态消耗的时间的百分比
				DWORD dwPctKernel = ::GetKernelModePercentage(
					ftKernelMode,        // 在内核态下消耗的时间
					ftUserMode           // 在用户态下消耗的时间
					);
				// 向用户显示进程的某些信息
				_tprintf(TEXT("进程标识符（PID）：%lu\n"), pe.th32ProcessID);
				_tprintf(TEXT("exe文件名：%s\n"), pe.szExeFile);
				_tprintf(TEXT("内核态时间：%lu\n"), dwPctKernel);
				_tprintf(TEXT("\n"));

				::CloseHandle(hProcess);  // 关闭进程句柄
				hProcess = NULL;
			}
		}
		bMore = ::Process32Next(hSnapshot, &pe);
	}
	if (!found)
		_tprintf(TEXT("当前操作系统没有检测到可执行文件的文件名为%s的进程正在运行\n"), argv[1]);
	return EXIT_SUCCESS;
}