/** 
 * @file    1-3.cpp 
 * @brief   实验一  清单1-3 
 * @details 内核态运行和用户态运行时间测试程序
 * @author  叶剑飞
 * @date    2014年6月13日
 */

#include <cstdio>
#include <tchar.h>
#include <cstdlib>

int main()
{
	int i, j;
	while (true)
	{
		for (i = 0; i < 1000; ++i)
		{ }
		for (j = 0; j < 1000; ++j)
			_tprintf(_T("enter kernel mode running.\n"));
	}
	return EXIT_SUCCESS;
}
