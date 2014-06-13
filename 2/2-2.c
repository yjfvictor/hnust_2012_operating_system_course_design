/*
 *  ʵ��� �嵥2-2 ���ӽ��̵ļ�ͨ�ż���ֹ��ʾ������
 *
 *
 *                            Ҷ����
 *                            2014��6��13��
 */

#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <stdlib.h>
#include <locale.h>
#include <Windows.h>

// �����������
static LPCTSTR g_szMutexName = TEXT("w2kdg.ProcTerm.mutex.Suicide");

// �����ӽ��̵ĺ���
BOOL StartClone(void)
{
	TCHAR szFilename[MAX_PATH];
	TCHAR szCommandLine[MAX_PATH];
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	BOOL bCreateOK;

	// ��õ�ǰ���̵Ŀ�ִ���ļ����ļ���
	GetModuleFileName(NULL, szFilename, MAX_PATH);

	// ���������в���
	_stprintf(szCommandLine, TEXT("\"%s\" child"), szFilename);
	// ʵ��2-2����3�����Ͼ��е��ַ���"child"��Ϊ����ַ��������±���ִ��

	// ע��ע�⣺������������ᵼ��ÿ���ӽ��̶�����Parent�������Ӷ��������޿����̣����ս�����Դ�ľ�������������
	// ����֮ǰ����˼���������һ��Ҫ��������ȷ���Ѿ������������Ҫ���ϣ�����

	// ��ʼ������������Ϣ
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	bCreateOK = CreateProcess(
		szFilename,           // ��ִ���ļ���·��
		szCommandLine,        // �������̵������в���
		NULL,                 // ���̰�ȫ��
		NULL,                 // �̰߳�ȫ��
		FALSE,                // �þ�����ɼ̳�
		CREATE_NEW_CONSOLE,   // ���µĿ���̨�������������
		NULL,                 // ָ���´����Ľ��̵Ļ�����ָ�롣NULL��ʾʹ�ø����̵Ļ����������
		NULL,                 // ���´����Ľ��̸���ĵ�ǰĿ¼�ľ���·����֧��UNC·����NULL��ʾ�ø����̵ĵ�ǰĿ¼��
		&si,                  // �����½��̵ĳ�ʼ��������Ϣ
		&pi                   // �����½��̵���Ϣ
		);

	if (bCreateOK)
	{
		// �ͷ��ӽ��̵Ľ��̾��
		CloseHandle(pi.hProcess);
		// �ͷ��ӽ��̵����̵߳��߳̾��
		CloseHandle(pi.hThread);
	}
	else
		_ftprintf(stderr, TEXT("�����ӽ���ʧ��\n"));

	return bCreateOK;
}

void Child(void)
{
	DWORD dwLastError = 0;

	// �򿪻����壬�������������ľ����
	HANDLE hMutexSuicide = OpenMutex(
		SYNCHRONIZE,      // �����ȡͬ��Ȩ�޵Ļ�������
		FALSE,            // �þ�����ɼ̳�
		g_szMutexName     // �����������
		);
	if (hMutexSuicide == NULL)
	{
		dwLastError = GetLastError();
		if (dwLastError == ERROR_FILE_NOT_FOUND)
			_ftprintf(stderr, TEXT("����Ϊ%s�Ļ�����ʧ�ܡ��û����岻���ڡ�\n"), g_szMutexName);
		exit(EXIT_FAILURE);
	}

	_tprintf(TEXT("�ӽ������ڵȴ������̷��͹ر�����\n"));

	// �ӽ��̽�������״̬���ȴ��������ͷŻ�����
	WaitForSingleObject(hMutexSuicide, INFINITE);
	// ʵ��2-2����5�����Ͼ��ΪWaitForSingleObject(hMutexSuicide, 0);���±���ִ��
	// Ԥ�棺���������Ľ�������ӽ��������˳������ȴ��������ͷŻ�����

	_tprintf(TEXT("�ӽ��������˳�\n"));

	// �رջ�������
	CloseHandle(hMutexSuicide);

	// �����������
	exit(EXIT_SUCCESS);
}

void Parent(void)
{
	DWORD dwLastError = 0;

	// ����������
	HANDLE hMutexSuicide = CreateMutex(
		NULL,          // ������İ�ȫ�Բ�����NULL��ʾȡȱʡֵ
		TRUE,          // �Ըû�����ӵ������Ȩ
		g_szMutexName  // �����������
		);
	if (hMutexSuicide ==NULL)
	{
		dwLastError = GetLastError();
		if (dwLastError == ERROR_ALREADY_EXISTS)
			_ftprintf(stderr, TEXT("������Ϊ%s�Ļ�����ʧ�ܡ��������ѱ�ռ�á�\n"), g_szMutexName);
		else if (dwLastError == ERROR_ACCESS_DENIED)
			_ftprintf(stderr, TEXT("��Ȩ�޴�����Ϊ%s�Ļ�����\n"), g_szMutexName);
		else
			_ftprintf(stderr, TEXT("��������Ϊ%s�Ļ����崴��ʧ�ܣ�������Ϊ0x%x��\n"), g_szMutexName, dwLastError);
		exit(EXIT_FAILURE);
	}

	_tprintf(TEXT("��ʼ�����ӽ���\n"));

	// �����ӽ���
	if (!StartClone())
	{
		_tprintf(TEXT("�����ӽ���ʧ��\n"));
		exit(EXIT_FAILURE);
	}

	_tprintf(TEXT("�밴�»س����ر��ӽ���\n"));

	// �ȴ������̵ļ�����Ӧ
	_gettchar();

	// �ͷŻ����塣����źŻᷢ�͸��ӽ��̵�WaitForSingleObject����
	ReleaseMutex(hMutexSuicide);

	// �رջ�������
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