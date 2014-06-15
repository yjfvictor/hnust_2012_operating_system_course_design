/*
 *  实验四  Windows存储管理
 *
 *  清单4-1  了解和检测进程的虚拟内存空间
 *
 *
 *
 *                     叶剑飞
 *                     2014年6月16日
 */

#include <Windows.h>
#include <cstdio>
#include <Shlwapi.h>
#include <iomanip>
#include <locale.h>
#include <tchar.h>

#pragma comment(lib, "Shlwapi.lib")

// 以可读方式对用户显示保护的辅助方法。
// 保护标记表示允许应用程序对内存进行访问的类型
// 以及操作系统强制访问的类型
inline bool TestSet(DWORD dwTarget, DWORD dwMask)
{
	return ((dwTarget & dwMask) == dwMask);
}

# define SHOWMASK(dwTarget, type) \
if (TestSet(dwTarget, PAGE_##type)) \
	{ _tprintf(TEXT(", %s"), TEXT(#type)); }

void ShowProtection(DWORD dwTarget)
{
	SHOWMASK(dwTarget, READONLY);
	SHOWMASK(dwTarget, GUARD);
	SHOWMASK(dwTarget, NOCACHE);
	SHOWMASK(dwTarget, READWRITE);
	SHOWMASK(dwTarget, WRITECOPY);
	SHOWMASK(dwTarget, EXECUTE);
	SHOWMASK(dwTarget, EXECUTE_READ);
	SHOWMASK(dwTarget, EXECUTE_READWRITE);
	SHOWMASK(dwTarget, EXECUTE_WRITECOPY);
	SHOWMASK(dwTarget, NOACCESS);
}

// 遍历整个虚拟内存并对用户显示其属性的工作程序的方法
void WalkVM(HANDLE hProcess)
{
	// 首先，获得系统信息
	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(si));
	GetSystemInfo(&si);

	// 分配要存放信息的缓冲区
	MEMORY_BASIC_INFORMATION mbi;
	ZeroMemory(&mbi, sizeof(mbi));

	// 循环整个应用程序地址空间
	LPCVOID pBlock = (LPVOID)si.lpMinimumApplicationAddress;
	while (pBlock < si.lpMaximumApplicationAddress)
	{
		// 获得下一个虚拟内存块的信息
		if (VirtualQueryEx(
			hProcess,        // 相关的进程
			pBlock,          // 开始位置
			&mbi,            // 缓冲区
			sizeof(mbi)      // 大小的确认
			) == sizeof(mbi))
		{
			// 计算块的结尾及其大小
			LPCVOID pEnd = (PBYTE)pBlock + mbi.RegionSize;
			TCHAR szSize[MAX_PATH];
			StrFormatByteSize(mbi.RegionSize, szSize, MAX_PATH);

			// 显示块地址和大小
			_tprintf(TEXT("%08x-%08x"), (DWORD)pBlock, (DWORD)pEnd);
			_tprintf(TEXT(" (%7s) "), szSize);
			switch (mbi.State)
			{
			case MEM_COMMIT:
				_tprintf(TEXT("Committed"));
				break;
			case MEM_FREE:
				_tprintf(TEXT("Free"));
				break;
			case MEM_RESERVE:
				_tprintf(TEXT("Reserved"));
				break;
			}

			// 显示保护
			if (mbi.Protect == 0 && mbi.State != MEM_FREE)
				mbi.Protect = PAGE_READONLY;
			ShowProtection(mbi.Protect);

			// 显示类型
			switch (mbi.Type)
			{
			case MEM_IMAGE:
				_tprintf(TEXT(", Image"));
				break;
			case MEM_MAPPED:
				_tprintf(TEXT(", Mapped"));
				break;
			case MEM_PRIVATE:
				_tprintf(TEXT(", Private"));
				break;
			}

			// 检验可执行的影像
			TCHAR szFilename[MAX_PATH];
			if (GetModuleFileName(
				(HMODULE)pBlock,
				szFilename,
				MAX_PATH
				) > 0)
			{
				// 除去路径并显示
				PathStripPath(szFilename);
				_tprintf(TEXT(", Module: %s"), szFilename);
			}

			_putts(TEXT(""));

			// 移动块指针以获得下一个块
			pBlock = pEnd;
		}
	}
}

void ShowVirtualMemory()
{
	// 首先，让我们获得系统信息
	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(si));
	GetSystemInfo(&si);

	// 使用外壳辅助程序对一些尺寸进行格式化
	TCHAR szPageSize[MAX_PATH];
	StrFormatByteSize(si.dwPageSize, szPageSize, MAX_PATH);

	DWORD dwMemSize = (DWORD)si.lpMaximumApplicationAddress - (DWORD)si.lpMinimumApplicationAddress;
	TCHAR szMemSize[MAX_PATH];
	StrFormatByteSize(dwMemSize, szMemSize, MAX_PATH);

	// 将内存信息显示出来
	_tprintf(TEXT("Virtual memory page size: %s\n"), szPageSize);
	_tprintf(TEXT("Minimum application address: 0x%08x\n"), (DWORD)si.lpMinimumApplicationAddress);
	_tprintf(TEXT("Maximum application address: 0x%08x\n"), (DWORD)si.lpMaximumApplicationAddress);
	_tprintf(TEXT("Total available virtual memory: %s\n"), szMemSize);
}

int _tmain()
{
	setlocale(LC_ALL, "");

	// 显示虚拟内存的基本信息
	ShowVirtualMemory();

	_putts(TEXT(""));

	// 遍历当前进程的虚拟内存
	WalkVM(GetCurrentProcess());

	return EXIT_SUCCESS;
}