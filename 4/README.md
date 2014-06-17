# Windows存储管理 #

Windows存储管理，是一个非常复杂的东西，很多性能方面的问题都与此直接相关。Windows使用了虚拟内存页面文件的方式，来对地址空间进行管理。这个文件一般名叫`pagefile.sys`，位于分区的根目录下。

对Windows虚拟内存的操作，微软提供了几个API，分别是：

<table align="center" border="1" cellspacing="0" style="width:90%;">
<tr><th>Windows API 函数名</th><th>函数作用</th></tr>
<tr><td>VirtualQueryEx</td><td>检索指定进程的虚拟内存地址空间的页面范围的信息</td></tr>
<tr><td>VirtualAlloc</td><td>在主调进程的虚拟内存地址空间上检索或提交一个页面的区域</td></tr>
<tr><td>VirtualFree</td><td>在主调进程的虚拟内存地址空间上释放或撤消提交一个页面的区域</td></tr>
<tr><td>VirtualProtect</td><td>在主调进程的虚拟内存地址空间中的已提交的页面的区域上改变保护状态</td></tr>
<tr><td>VirtualLock</td><td>把进程的虚拟内存地址空间的指定区域锁定到物理内存上</td></tr>
<tr><td>VirtualUnlock</td><td>解除锁定虚拟内存地址空间到物理内存上的锁定，允许这块虚拟内存被交换出来</td></tr>
</table>

## 几个shlwapi函数的介绍 ##

### StrFormatByteSize ###

#### 函数原型 ####

    PWSTR StrFormatByteSizeW(
      LONGLONG qdw,
      PWSTR pszBuf,
      UINT cchBuf
    );
    
    PSTR StrFormatByteSizeA(
      DWORD dw,
      PSTR pszBuf,
      UINT cchBuf
    );
    
    #ifdef UNICODE
    #define StrFormatByteSize       StrFormatByteSizeW
    #else
    #define StrFormatByteSize       StrFormatByteSizeA
    #endif

#### 函数功能 ####

把一个数值转换成一个字符串，显示成B、KB、MB或GB的形式。

#### 形参说明 ####

- dw/qdw （输入参数）

需要转换的数值

- pszBuf（输出参数）

指向一个字符缓冲区的指针，用户接收转换之后的结果

- cchBuf（输入参数）

pszBuf的缓冲区的长度

#### 返回值说明 ####

返回一个转换完之后的字符串的首地址。如果转换失败，则返回NULL。


### PathStripPath ###

#### 函数原型 ####

    void PathStripPath(
      _Inout_  LPTSTR pszPath
    );

#### 函数功能 ####

删除路径部分，只留下文件名。

#### 形参说明 ####

- pszPath（输入输出参数）

一个指向C风格字符串（以TEXT('\0')结尾的字符数组），长度不能超过MAX_PATH。输入一个包含有文件的路径，输出不带路径的文件名。

#### 返回值说明 ####

这个函数没有返回值。

## 几个 Windows API 函数的介绍 ##

### GetSystemInfo ###

#### 函数原型 ####

    void WINAPI GetSystemInfo(
      LPSYSTEM_INFO lpSystemInfo
    );

#### 函数功能 ####

检索当前系统的信息。

#### 形参说明 ####

- lpSystemInfo（输出参数）

一个指向SYSTEM_INFO结构体类型的变量的指针，用于接收系统信息。

SYSTEM_INFO结构体的定义如下：

    typedef struct _SYSTEM_INFO {
      union {
        DWORD  dwOemId;                      // 历史遗留参数。已经没有用了。
        struct {
          WORD wProcessorArchitecture;       // 当前系统系统运行的处理器架构
          WORD wReserved;                    // 保留参数，供未来使用。
        };
      };
      DWORD     dwPageSize;                  // 页面大小和页面保护与提交粒度
      LPVOID    lpMinimumApplicationAddress; // 指向应用程序和动态链接库能访问的虚拟内存地址中的最低值的指针
      LPVOID    lpMaximumApplicationAddress; // 指向应用程序和动态链接库能访问的虚拟内存地址中的最高值的指针
      DWORD_PTR dwActiveProcessorMask;       // 表示在系统内配置的处理器集的掩码
      DWORD     dwNumberOfProcessors;        // 当前组的逻辑处理器的个数
      DWORD     dwProcessorType;             // 历史遗留参数。已经没有用了。
      DWORD     dwAllocationGranularity;     // 虚拟内存可被分配的起始地址的粒度
      WORD      wProcessorLevel;             // 架构导向的处理器级别，代表32位或64位
      WORD      wProcessorRevision;          // 架构导向的处理器版本号
    } SYSTEM_INFO;

### 返回值说明 ###

这个函数没有返回值。

### VirtualQueryEx ###

#### 函数原型 ####

    SIZE_T WINAPI VirtualQueryEx(
      HANDLE hProcess,
      LPCVOID lpAddress,
      PMEMORY_BASIC_INFORMATION lpBuffer,
      SIZE_T dwLength
    );

#### 函数功能 ####

检索关于指定进程的虚拟内存地址空间页面范围的信息。

#### 形参列表 ####

- hProcess（输入参数）

待查询进程的进程句柄。这个句柄有PROCESS_QUERY_INFORMATION权限。

- lpAddress（输入参数，可为NULL）

一个指向待查询页面区域的首地址。这个值会被向下到下一个页面的界限。可以用GetSystemInfo函数来得到主机的页面大小。

如果lpAddress指定的地址大于进程可访问的虚拟内存地址的最大值，那么这个函数将调用失败，并且GetLastError()的返回值将为ERROR_INVALID_PARAMETER。

- lpBuffer（输出参数）

指向MEMORY_BASIC_INFORMATION结构体变量的指针，用于获取指定页面范围的信息。

MEMORY_BASIC_INFORMATION结构体的定义如下：

    typedef struct _MEMORY_BASIC_INFORMATION {
      PVOID  BaseAddress;       // 指向页面区域的首地址的指针
      PVOID  AllocationBase;    // 指向用VirtualAlloc函数分配的页面范围首地址的指针
      DWORD  AllocationProtect; // 区域被初始分配时的内存保护选项
      SIZE_T RegionSize;        // 全部页面都有的标识性属性区域的首地址区域的字节数
      DWORD  State;             // 区域页面状态
      DWORD  Protect;           // 区域页面的访问保护权限
      DWORD  Type;              // 区域页面类型
    } MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;

- dwLength（输入参数）

lpBuffer指针指向的缓冲区的大小，以字节为单位。
#### 返回值类型 ####

返回值是输出到lpBuffer参数中的实际字节数。

如果函数调用失败，返回值是0。欲知错误详情，可以调用GetLastError函数。