# 实验二的代码说明 #
首先，本代码也已经经过ANSI和UNICODE字符集两种方式编译，都编译通过。字符集的问题详见实验一的说明文档，位于“1”目录中。

对于《操作系统课程设计指导书》中原代码一些问题，在这里已作修复。对于两个操作，也已做出注释，并在注释中详细解释了修改代码后会遇到的问题。

## 几个Windows API函数的说明 ##

### CreateProcess ###

#### 函数原型 ####

    BOOL WINAPI CreateProcess(
      LPCTSTR lpApplicationName,
      LPTSTR lpCommandLine,
      LPSECURITY_ATTRIBUTES lpProcessAttributes,
      LPSECURITY_ATTRIBUTES lpThreadAttributes,
      BOOL bInheritHandles,
      DWORD dwCreationFlags,
      LPVOID lpEnvironment,
      LPCTSTR lpCurrentDirectory,
      LPSTARTUPINFO lpStartupInfo,
      LPPROCESS_INFORMATION lpProcessInformation
    );

#### 函数作用 ####
创建子进程。

#### 形参说明 ####

- lpApplicationName（输入参数，可为NULL）

需要执行的可执行文件的文件名。这可以是基于Windows的应用程序，也可以是在本操作系统支持的一些子系统的可执行文件（如MS-DOS或OS/2的）。

该字符串指定了新进程的完整路径，包括文件名，也可以是部分的名字。使用部分路径时，将会从当前目录为基础来补全路径。这个函数不会使用搜索路径。这个参数必须包括文件的扩展名。没有所谓缺省扩展名。

这个参数可以是NULL。如果是这样的话，进程名就是从第二个形参lpCommandLine中的第一个空格分隔字段读取，或者从第一个双引号引住的字段读取。

如果可执行文件中16位应用程序，这个参数必须是NULL，从第二个参数lpCommandLine中读取进程名。

- lpCommandLine（输入输出参数，可为NULL）

传入进程的命令行参数。最长长度为32768个字符，包括Unicode结尾的空字符（L'\0'）在内。如果第一个参数lpApplicationName是NULL，那么进程名就从这个字符串中读取，进程名部分不能超过MAX_PATH个字符。lpApplicationName与lpCommandLine中的两个进程名不符时，以前者为准。

- lpProcessAttributes（输入参数，可为NULL）

这个参数为一个指向SECURITY_ATTRIBUTES类型的结构体变量的指针，表示待创建的进程安全属性。这个参数可以为NULL，表示进程安全属性取缺省值。如果这个参数为NULL，那么这个句柄不可被继承。

- lpThreadAttributes（输入参数，可为NULL）

这个参数为一个指向SECURITY_ATTRIBUTES类型的结构体变量的指针，表示待创建的进程的主线程的安全属性。这个参数可以为NULL，表示取缺省的安全属性。如果这个参数为NULL，那么这个句柄不可被继承。


- bInheritHandles（输入参数）

如果这个参数为TRUE，那么本进程的每个可继承的句柄都被新进程继承。如果这个参数为FALSE，那么句柄不被继承。注意被继承的句柄与原句柄有相同的值和访问权限。

- dwCreationFlags（输入参数）

这个标识用于控制待创建进程的优先级与进程的创建方式。例如NORMAL_PRIORITY_CLASS表示普通优先级，BELOW_NORMAL_PRIORITY_CLASS表示低于标准优先级，CREATE_NEW_CONSOLE表示创建新控制台，等等。

- lpEnvironment（输入参数，可为NULL）

指向待创建的进程的环境变量块的指针。如果这个参数为NULL，那么新进程将使用当前进程的环境变量。

- lpCurrentDirectory（输入参数，不可为NULL）

这个参数是一个绝对路径的字符串，用于给新进程指定“当前目录”，支持UNC格式的路径。如果这个参数为NULL，那么新进程的当前目录为父进程的当前目录。

- lpStartupInfo（输出参数，不可为NULL）

这个参数为一个指向STARTUPINFO或STARTUPINFOEX类型的结构体变量的指针，用于指定进程的启动信息，如控制台应用程序进程的标题栏，指定标准输入句柄等等。

- lpProcessInformation

这个参数为一个指向PROCESS_INFORMATION类型的结构体变量的指针，用于获取新进程的一些信息，包括新进程的句柄、新进程的主线程的句柄、新进程的进程标识符、新进程的主线程的线程标识符。

#### 返回值 ####
进程创建失败时，返回FALSE；进程创建成功时，返回其它值。可以用GetLastError函数来读取进程创建失败的具体原因。

### CreateMutex ###

#### 函数原型 ####

    HANDLE WINAPI CreateMutex(
      LPSECURITY_ATTRIBUTES lpMutexAttributes,
      BOOL bInitialOwner,
      LPCTSTR lpName
    );

#### 函数作用 ####
创建互斥体。

#### 形参说明 ####

- lpMutexAttributes（输入参数，可为NULL）

这个参数为一个指向SECURITY_ATTRIBUTES类型的结构体变量的指针，表示这个互斥体的安全属性。这个参数可以为NULL，表示取缺省安全属性。如果这个参数取NULL，那么该句柄不可被继承。

- bInitialOwner（输入参数）

如果这个参数为TRUE并且成功创建了这个互斥体，那么这个线程就拥有了这个互斥体对象的初始的所有权。如果这个参数为FALSE，那么并没有拥有互斥体对象的所有权。


- lpName（输入参数，可为NULL）

指定这个互斥体的名字，区分大小写。这个名字在操作系统中必须是唯一的，否则互斥体会创建失败。这个参数可以为NULL，若为NULL，则创建无名互斥体，也就意味着无法被OpenMutex。

#### 返回值 ####
进程创建失败时，返回FALSE；进程创建成功时，返回其它值（不一定是TRUE）。可以用GetLastError函数来读取进程创建失败的具体原因。

### OpenMutex ###

#### 函数原型 ####

    HANDLE WINAPI OpenMutex(
      DWORD dwDesiredAccess,
      BOOL bInheritHandle,
      LPCTSTR lpName
    );

#### 函数作用 ####
打开互斥体，申请获取已被创建的互斥体的指定权限句柄。

#### 形参说明 ####

- dwDesiredAccess（输入参数）

需要的互斥体访问权限。例如可以取值SYNCHRONIZE，表示申请具有同步权限的互斥体句柄；再例如可以取值MUTEX_ALL_ACCESS，表示申请具有修改互斥体的安全性的权限的句柄；等等。

- bInheritHandle（输入参数）

如果这个参数是TRUE，被这个进程创建的子进程将可以继承这个句柄；否则子进程不能继承这个句柄。

- lpName（输入参数）

需要打开的互斥体的名字，区分大小写。

#### 返回值 ####

指定权限的互斥体句柄。

### ReleaseMutex ###

#### 函数原型 ####

    BOOL WINAPI ReleaseMutex(
      HANDLE hMutex
    );

#### 函数作用 ####

释放互斥体。

#### 形参说明 ####

- hMutex（输入参数）

互斥体的句柄，该句柄由CreateMutex或CreateMutexEx或OpenMutex函数返回。

#### 返回值 ####

若互斥体释放失败，则返回FALSE；若互斥体释放成功，则返回其它值（不一定是TRUE）。