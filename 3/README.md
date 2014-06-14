# Windows线程同步的经典算法 #

> 该代码实为Windows线程同步代码，而不是进程同步代码，指导书上的标题有误。

## 几个 Windows API 函数的介绍 ##

### CreateSemaphore ###

#### 函数原型 ####

    HANDLE WINAPI CreateSemaphore(
      LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
      LONG lInitialCount,
      LONG lMaximumCount,
      LPCTSTR lpName
    );

#### 函数功能 ####

创建或打开信号量对象。

#### 形参说明 ####

- lpSemaphoreAttributes（输入参数，可为NULL）

一个指向SECURITY_ATTRIBUTES结构体变量的指针，表示信号量的安全属性。如果这个参数为NULL，那么这个信号量使用缺省安全属性。

- lInitialCount（输入参数）

信号量对象的初始计数。这个数字必须大于或等于零，并且小于或等于lMaximumCount。当计数大于零时，这个信号量处于有信号状态；当计数等于零时，这个信号量处于无信号状态。当调用WaitForSingleObject或WaitForMultipleObjects的时候，计数减一。调用ReleaseSemaphore的时候，计数增加指定值。

- lMaximumCount（输入参数）

信号量对象的计数最大值。这个值必须大于零。

- lpName（输入参数，可为NULL）

信号量对象的名字，区分大小写，长度不能超过MAX_PATH个字符。如果这个参数为NULL，则创建无名信号量。

#### 返回值说明 ####

如果信号量对象创建成功，则返回信号量对象的句柄。如果是命名信号量，并且这个名字已经存在，则打开这个信号量并返回已存在命名信号量对象的句柄，然后GetLastError()返回ERROR_ALREADY_EXISTS。

如果信息量创建或打开失败，则返回NULL。具体失败原因可以调用GetLastError()查看。

### ReleaseSemaphore ###

#### 函数原型 ####

    BOOL WINAPI ReleaseSemaphore(
      HANDLE hSemaphore,
      LONG lReleaseCount,
      LPLONG lpPreviousCount
    );


#### 函数功能 ####

给指定的信号量对象计数增加指定值。

#### 形参说明 ####

- hSemaphore（输入参数）

信号量对象的句柄。这个句柄必须有SEMAPHORE_MODIFY_STATE的访问权限。

- lReleaseCount（输入参数）

信号量对象计数需要增加的值，必须大于零。如果增加后会超过在CreateSemaphore函数中指定的计数最大值lMaximumCount，那么这个函数返回FALSE并且计数保持原数不变。

- lpPreviousCount（输出参数，可为NULL）

一个指向LONG类型变量的指针，用于接收信号量对象计数的先前值。如果这个参数为NULL，则不接收信号量对象计数的先前值。

#### 返回值说明 ####

信号量对象计数如果增加失败，返回FALSE。如果增加成功，返回其它值（未必是TRUE）。如果想知道信号量对象计数如果增加失败的具体原因，可以调用GetLastError函数。

### CreateThread ###

#### 函数原型 ####

    HANDLE WINAPI CreateThread(
      LPSECURITY_ATTRIBUTES lpThreadAttributes,
      SIZE_T dwStackSize,
      LPTHREAD_START_ROUTINE lpStartAddress,
      LPVOID lpParameter,
      DWORD dwCreationFlags,
      LPDWORD lpThreadId
    );

#### 函数功能 ####

在当前进程中创建一个线程。

#### 形参说明 ####

- lpThreadAttributes（输入参数，可为NULL）

指向SECURITY_ATTRIBUTES结构体变量的指针，用于要求新线程返回的线程句柄是否可以被子进程继承。如果这个参数为NULL，那么返回出来的线程句柄不可被子进程继承。这个结构体的lpSecurityDescriptor成员变量决定这个线程的安全属性，如果为NULL，则使用缺省安全属性。

- dwStackSize（输入参数，可为零）

这个参数用于新线程的栈的初始大小，以字节为单位。系统会自动把这个值四舍五入到最接近的内存页的大小。如果这个参数为为零，那么新线程使用可执行文件的默认栈大小。

- lpStartAddress（输入参数）

函数指针，指向新线程的函数。这个形参的类型为LPTHREAD_START_ROUTINE，在<minwinbase.h>中的定义是：

    typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)(
        LPVOID lpThreadParameter
        );
    typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

这就意味着这个线程函数的函数原型必须是这样子的：

    DWORD WINAPI 线程函数名(LPVOID lpThreadParameter);


- lpParameter（输入参数，可为NULL）

这个参数将出数据传入上述线程函数的lpThreadParameter参数中。

- dwCreationFlags（输入参数）

这个参数控制线程的创建方式。可以取以下三个值：

<table align="center" border="1" cellspacing="0" style="width:90%;">
<tr><th>取值</th><th>意义</th></tr>
<tr><td>0</td><td>该线程创建成功后立即运行</td></tr>
<tr><td>CREATE_SUSPENDED</td><td>该线程创建成功后立即挂起，需要调用ResumeThread函数来使该线程运行起来</td></tr>
<tr><td>STACK_SIZE_PARAM_IS_A_RESERVATION</td><td>dwStackSize参数指定了栈的初始预留的大小。如果没有设置这个值，那么dwStackSize指定页面提交大小。</td></tr>
</table>

- lpThreadId（输出参数，可为NULL）

指向一个DWORD类型变量的指针，用于接收线程标识符。如果这个参数是NULL，那么就不接收线程标识符。

#### 返回值说明 ####

如果创建线程成功，则返回新创建的线程的线程句柄。如果创建线程失败，则返回NULL，可以用GetLastError函数来获得具体的创建线程失败原因。
