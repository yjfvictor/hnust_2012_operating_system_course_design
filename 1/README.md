# 实验一的代码说明 #
本代码经过ANSI和UNICODE两种字符集模式编译，都通过了。

## VC/VS版本的字符集问题 ##

自从VC6.0开始，就已经支持UNICODE字符集，但是VC6.0<strong>默认</strong>的是“多字节字符集”（MBCS），使用UNICODE字符集需要自行在项目中定义`UNICODE`宏和`_UNICODE`宏；然而VS.NET2002以及更高版本<strong>默认</strong>的却是UNICODE字符集，即定义了`UNICODE`宏和`_UNICODE`宏，除非你在项目属性中人工指定使用“多字节字符集”（MBCS），那样就不会定义`UNICODE`宏和`_UNICODE`宏。

VS2013的MFC更是完全废除了MBCS，使用MBCS会给编译错误信息：MBCS is depreciated。

### 什么是ANSI字符集和UNICODE字符集？ ###

#### ANSI代码页（ANSI code page） ####

> 特别强调：这里说的确实是ANSI，不是ASCII。

最早的时候，计算机字符编码只有0-127的编码，叫做ASCII字符集，然而其它国家的文字就无法被表示。于是，西欧国家就创建了更大的字符集，名叫latin-1字符集。后来，日本为日文编写了一个字符编码，名叫Shift-JIS；中国大陆也为简体字日文编写了一个字符编码，名叫GB2312-80；中国台湾也为繁体字日文编写了一个字符编码，名叫BIG-5。后来中国大陆又嫌GB2312-80的汉字大少，只有简体字，所以又把繁体字拉进来，出现了GBK字符集，后来又出现了GB18030-2000字符集，等等。

这么多字符集混乱不堪，那么计算机怎么知道你要用哪套字符集呢？于是，提出了一套“治标不治本”的方案，这就是“ANSI代码页”。每种字符集规定一个数字作为唯一标识符，叫做“代码页”，更换这个唯一标识符就切换字符集。例如ASCII的代码页是437，latin-1的代码页是1252，GB2312-80的代码页是20936，GBK的代码页是936，BIG-5的代码页是950，GB18030的代码页是54936等等。

起初，Windows用的就是“ANSI代码页”切换方案，其字符串的储存方式就是用char数组来储存，叫做“多字节字符集”（MBCS），之所以这么叫是因为有些字符占不止一个字符，例如一个GBK汉字占两个字节。

微软还在C标准以外，私自定义了以下几个“C风格窄字符串”（以'\0'结尾的窄字符数组）类型：

    #define CONST const
    typedef char CHAR;
    typedef CHAR *NPSTR, *LPSTR, *PSTR;
    typedef CONST CHAR *LPCSTR, *PCSTR;

#### UNICODE字符集 ####

上面说过了，这几个方法只是“治标不治本”的方案。依靠切换ANSI代码页根本靠不住，例如同屏显示简繁体字就成了个技术难题（所以才有了“同文堂”等软件）。如果同时显示中文和日文，那难度更大。更严重的是，你的软件在你的电脑上用简体中文Windows上运行得很好（用936代码页），别人的电脑上可能是繁体中文（代码页950）或英文（代码页1252）看到的只能是一堆乱码！！！

那怎么办？为什么要多种字符集“各自为政”，每种文字体系一种字符编码集？难道不能全球统一都用同一种字符编码集吗？只要这种字符编码集包含了全球一切文字就行了啊！是的，就是这样，于是Unicode出现了。“Unicode联盟”（Unicode Consortium）与“国际标准化组织”（ISO）共同创建了一套包含全球一切文字的字符编码集，前者叫做“Unicode字符集”，后者叫做“ISO/IEC 10646”国际标准。由于这两个标准是相互兼容的，我们可以认为是同一套标准。正是这套标准，才从根本上解决了乱码问题。

Unicode（ISO/IEC 10646）的字符编码有三种“表示方式”，分别是UTF-8、UTF-16、UTF-32。后两者又有大小端的区别（读者可自行搜索“大端模式”和“小端模式”，这里不细讲）。

【ISO国际标准】根据ISO的C标准和C++标准，字符类型有两种：一种叫做“窄字符”（narrow character），用char表示，每个单位占一个字节；另一种叫做“宽字符”（wide character），用wchar_t表示，每个单位占的字节数必须多于char，但没有规定具体占几个字节。窄字符和宽字符的表示方式分别是：`char s[] = "narrow character";` 和 `wchar_t ws[] = L"wide character";`。

【微软方案】在微软的编译器中，宽字符集被叫做“UNICODE字符集”，而且特指“UTF-16小端模式”（UTF-16 little endian），每个单位占两个字节，类型用wchar_t表示。

微软还在C标准以外，私自定义了以下几个“C风格宽字符串”（以L'\0'结尾的宽字符数组）类型：

    #define CONST const
    typedef wchar_t WCHAR;
    typedef WCHAR *NWPSTR, *LPWSTR, *PWSTR;
    typedef CONST WCHAR *LPCWSTR, *PCWSTR;

#### TCHAR类型 ####
为了便于在ANSI字符集和UNICODE字符集之间的切换，微软私自定义了一个字符类型——`TCHAR`。在&lt;winnt.h&gt;中定义如下：

    typedef wchar_t WCHAR;
    #ifdef  UNICODE
    typedef WCHAR TCHAR, *PTCHAR;
    typedef LPWSTR PTSTR, LPTSTR;
    typedef LPCWSTR PCTSTR, LPCTSTR;
    #else
    typedef char TCHAR, *PTCHAR;
    typedef LPSTR PTSTR, LPTSTR;
    typedef LPCSTR PCTSTR, LPCTSTR, PCUTSTR, LPCUTSTR;
    #endif

此外，微软还在&lt;winnt.h&gt;定义了`TEXT`宏：

    #ifdef  UNICODE
    #define __TEXT(quote) L##quote
    #else
    #define __TEXT(quote) quote
    #endif
    #define TEXT(quote) __TEXT(quote)

微软还在&lt;tchar.h&gt;头文件中定义了`_T`宏和`_TEXT`宏：

    #ifdef _UNICODE
    #define __T(x)      L ## x
    typedef wchar_t     TCHAR;
    #else
    #define __T(x)      x
    typedef char        TCHAR;
    #endif
    #define _T(x)       __T(x)
    #define _TEXT(x)    __T(x)

ISO的C和C++标准也有对应的窄字符集函数printf、strcat、strcmp和宽字符集函数wprintf、strcat、strcmp等等。微软也私自在&lt;tchar.h&gt;中宏定义了这些函数的TCHAR风格类型：

    #ifdef _UNICODE
    #define _tprintf        wprintf
    #define _tcscmp         wcscmp
    #define _tcscat         wcscat
    #else
    #define _tprintf        printf
    #define _tcscmp         strcmp
    #define _tcscat         strcat
    #endif

### 跨平台时要注意的几点 ###

【ISO国际标准】ISO的C标准和C++标准只规定了窄字符char为一个字节，宽字符wchar_t的字节数大于char。没有规定wchar_t具体占几个字节。没有TCHAR这个东西，不存在&lt;tchar.h&gt;头文件。

【微软方案】在微软的编译器中，char每个单位占一个字节，表示ANSI字符集；wchar_t占两个字节，表示UTF-16小端模式。可以用TCHAR类型（在&lt;tchar.h&gt;头文件中），并用是否定义`UNICODE`宏和`_UNICODE`宏来切换。

【Linux通常情况】在大多数Linux下的编译器中，char每个单位占一个字节，通常表示UTF-8字符集；wchar_t占四个字节，表示UTF-32字符集，大小端与CPU相关。没有TCHAR这种东西。

## 什么是句柄 ##

<strong>句柄</strong>（handle），是某个资源的标识符。在操作系统中有很多内核对象，这些对象用户态进程不可直接访问，因此不能返回一个指针给用户态进程。于是现代操作系统很多都是返回一个内核对象的标识符供用户态进程调用。用户进程可以调用相应的系统API，传入这个内核对象标识符，来操作这个内核对象。这样的“标识符”就叫做<strong>句柄</strong>。

例如你想操作某个进程，就可以用这个进程的句柄。比如结束指定进程，就可以把该进程的进程句柄传入结束进程的函数中。你想读取在某个进程内分配虚拟内存地址空间，就可以把该进程的进程句柄传入分配虚拟内存地址空间函数中。

## 几个 Windows API 函数的介绍 ##

### CreateToolhelp32Snapshot ###

#### 函数原型 ####

    HANDLE WINAPI CreateToolhelp32Snapshot(
      DWORD dwFlags,
      DWORD th32ProcessID
    );

#### 函数功能 ####

抓取指定的进程列表的快照，包括这些进程的堆、模块、线程等信息。

#### 形参说明 ####

- dwFlags（输入参数）

快照包括的系统的一部分。这个参数可以是下列一个或多个的值的位或。

<table align="center" border="1" cellspacing="0" style="width:90%;">
<tr><th>取值</th><th>意义</th></tr>
<tr><td>TH32CS_INHERIT</td><td>表示获得的快照句柄是可以继承的</td></tr>
<tr><td>TH32CS_SNAPALL</td><td>包括这个系统中的全部进程和线程，外加在th32ProcessID中指定的进程的堆和模块。这个参数等价于 TH32CS_SNAPHEAPLIST | TH32CS_SNAPMODULE | TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD 参数</td></tr>
<tr><td>TH32CS_SNAPHEAPLIST</td><td>在快照中包括th32ProcessID中指定的进程的全部堆</td></tr>
<tr><td>TH32CS_SNAPMODULE</td><td>在快照中包括在th32ProcessID中指定的进程的全部模块</td></tr>
<tr><td>TH32CS_SNAPMODULE32</td><td>64位进程中用这个参数表示在快照中包括在th32ProcessID中指定的进程的全部的32位模块</td></tr>
<tr><td>TH32CS_SNAPPROCESS</td><td>在快照中包括系统中的全部进程</td></tr>
<tr><td>TH32CS_SNAPTHREAD</td><td>在快照中包括系统中的全部线程</td></tr>
</table>

- th32ProcessID（输入参数）

包括于快照中的进程的进程标识符。这个参数可以为零，表示当前进程。这个参数仅当dwFlags参数为`TH32CS_SNAPHEAPLIST`、`TH32CS_SNAPMODULE`、`TH32CS_SNAPMODULE32`、`TH32CS_SNAPALL`之一时才有效。否则这个参数被忽略。

#### 返回值说明 ####

如果进程快照抓取成功，则该函数返回指定快照的句柄。

如果进程快照抓取失败，则该函数返回`INVALID_HANDLE_VALUE`。查看详细错误原因，请调用GetLastError函数。可能的错误代码包括`ERROR_BAD_LENGTH`。


### Process32First ###

#### 函数原型 ####

    BOOL WINAPI Process32First(
      HANDLE hSnapshot,
      LPPROCESSENTRY32 lppe
    );

#### 函数功能 ####

从系统快照中检索第一个遇到的进程的信息。

#### 形参说明 ####

- hSnapshot（输入参数）

快照的句柄，由CreateToolhelp32Snapshot函数返回。

- lppe（输入输出参数）

指向PROCESSENTRY32结构体变量的指针。它包含了进程信息，例如可执行文件的文件名、进程标识符、父进程的进程标识符，等等。

PROCESSENTRY32结构体如下定义：

    typedef struct tagPROCESSENTRY32 {
      DWORD     dwSize;              // 结构体的大小，即sizeof(PROCESSENTRY32)
      DWORD     cntUsage;            // 历史遗留参数。现在已经没有用了，必须置0。
      DWORD     th32ProcessID;       // 该进程的进程标识符
      ULONG_PTR th32DefaultHeapID;   // 历史遗留参数。现在已经没有用了，必须置0。
      DWORD     th32ModuleID;        // 历史遗留参数。现在已经没有用了，必须置0。
      DWORD     cntThreads;          // 该进程创建的线程总数
      DWORD     th32ParentProcessID; // 该进程的父进程的进程标识符
      LONG      pcPriClassBase;      // 由该进程创建的任何线程的基本优先级
      DWORD     dwFlags;             // 历史遗留参数。现在已经没有用了，必须置0。
      TCHAR     szExeFile[MAX_PATH]; // 该进程的可执行文件的文件名
    } PROCESSENTRY32, *PPROCESSENTRY32;

#### 返回值说明 ####

如果进程列表第一个入口被成功地复制到lppe中，则返回TRUE，否则返回FALSE。

### Process32Next ###

#### 函数原型 ####

    BOOL WINAPI Process32Next(
      HANDLE hSnapshot,
      LPPROCESSENTRY32 lppe
    );

#### 函数功能 ####

从系统快照中检索下一个遇到的进程的信息。

#### 形参说明 ####

- hSnapshot（输入参数）

快照的句柄，由CreateToolhelp32Snapshot函数返回。

- lppe（输出参数）

指向PROCESSENTRY32结构体变量的指针。它包含了进程信息，例如可执行文件的文件名、进程标识符、父进程的进程标识符，等等。

#### 返回值说明 ####

如果进程列表下一个入口被成功地复制到lppe中，则返回TRUE，否则返回FALSE。

### OpenProcess ###

#### 函数原型 ####

    HANDLE WINAPI OpenProcess(
      DWORD dwDesiredAccess,
      BOOL  bInheritHandle,
      DWORD dwProcessId
    );

#### 函数功能 ####

打开已存在的本地进程对象。

这里解释一下，所谓“打开”某个对象，就是指获取这个对象的句柄。

#### 形参说明 ####

- dwDesiredAccess（输入参数）

进程的访问权限。可以取以下值的位或：

<table align="center" border="1" cellspacing="0" style="width:90%;">
<tr><th>取值</th><th>意义</th></tr>
<tr><td>DELETE</td><td>返回出来的句柄是有删除权限的句柄</td></tr>
<tr><td>READ_CONTROL</td><td>返回出来的句柄是有在安全描述符中读取信息权限的句柄</td></tr>
<tr><td>SYNCHRONIZE</td><td>返回出来的句柄是有等待进程运行结束权限的句柄</td></tr>
<tr><td>WRITE_DAC</td><td>返回出来的句柄是拥有在安全描述符中修改DACL权限的的句柄</td></tr>
<tr><td>WRITE_OWNER</td><td>返回出来的句柄是拥有在安全描述符中修改所有者权限的的句柄</td></tr>
<tr><td>PROCESS_ALL_ACCESS</td><td>返回出来的句柄是对进程有完全控制权限的句柄</td></tr>
<tr><td>PROCESS_CREATE_PROCESS</td><td>返回出来的句柄是有创建进程权限的句柄</td></tr>
<tr><td>PROCESS_CREATE_THREAD</td><td>返回出来的句柄是有创建远程线程权限的句柄</td></tr>
<tr><td>PROCESS_DUP_HANDLE</td><td>返回出来的句柄是有复制句柄权限的句柄</td></tr>
<tr><td>PROCESS_QUERY_INFORMATION</td><td>返回出来的句柄是有查询进程信息权限的句柄</td></tr>
<tr><td>PROCESS_QUERY_LIMITED_INFORMATION</td><td>返回出来的句柄是有查询少量进程信息权限的句柄</td></tr>
<tr><td>PROCESS_SET_INFORMATION</td><td>返回出来的句柄是有设置进程信息权限的句柄</td></tr>
<tr><td>PROCESS_SUSPEND_RESUME</td><td>返回出来的句柄是有挂起进程和恢复进程权限的句柄</td></tr>
<tr><td>PROCESS_TERMINATE</td><td>返回出来的句柄是有结束进程权限的句柄</td></tr>
<tr><td>PROCESS_VM_READ</td><td>返回出来的句柄是有读取进程虚拟内存地址空间权限的句柄</td></tr>
<tr><td>PROCESS_VM_WRITE</td><td>返回出来的句柄是有写入进程虚拟内存地址空间权限的句柄</td></tr>
</table>

- bInheritHandle（输入参数）

该句柄是否可被继承。如果这个值置TRUE，那么子进程可以继承这个句柄；如果这个值置FALSE，那么子进程不能继承这个句柄。

- dwProcessId（输入参数）

欲打开的进程的进程标识符。

#### 返回值说明 ####

如果打开进程成功，返回值就是指定进程的指定访问权限的句柄。

如果打开进程失败，返回值是NULL。欲知打开失败原因，可以调用GetLastError函数。

### CloseHandle ###

#### 函数原型 ####

    BOOL WINAPI CloseHandle(
      HANDLE hObject
    );

#### 函数功能 ####

关闭一个已打开的对象的句柄。释放掉句柄资源。

#### 形参说明 ####

- hObject（输入参数）

需要关闭掉的合法句柄。

#### 返回值说明 ####

如果句柄关闭成功，则返回非零值。

如果句柄关闭失败，则返回FALSE。句柄关闭失败的原因，可以调用GetLastError来查看。

### GetProcessTimes ###

#### 函数原型 ####

    BOOL WINAPI GetProcessTimes(
      HANDLE     hProcess,
      LPFILETIME lpCreationTime,
      LPFILETIME lpExitTime,
      LPFILETIME lpKernelTime,
      LPFILETIME lpUserTime
    );


#### 函数功能 ####

检索指定进程的时间信息。

#### 形参说明 ####

- hProcess（输入参数）

需要检索时间信息的进程的进程句柄。这个句柄需要有“查询进程信息”（`PROCESS_QUERY_INFORMATION`）的权限或者有“查询少量进程信息”（`PROCESS_QUERY_LIMITED_INFORMATION`）的权限。


- lpCreationTime（输出参数）

指向FILETIME结构体变量类型的指针，用于传出进程创建时间。

- lpExitTime（输出参数）

指向FILETIME结构体变量类型的指针，用于传出进程退出时间。

- lpKernelTime（输出参数）

指向FILETIME结构体变量类型的指针，用于传出进程的内核态时间。

- lpUserTime（输出参数）

指向FILETIME结构体变量类型的指针，用于传出进程的用户态时间。

FILETIME结构体定义如下：

    typedef struct _FILETIME {
      DWORD dwLowDateTime;    // 文件时间的低32位部分
      DWORD dwHighDateTime;   // 文件时间的高32位部分
    } FILETIME, *PFILETIME;

#### 返回值说明 ####

如果检索成功，则返回非零值。

如果检索失败，则返回FALSE。如果想知道检索失败原因，可以调用GetLastError来查看。