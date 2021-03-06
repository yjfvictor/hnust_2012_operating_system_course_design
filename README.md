# 湖南科技大学2012级操作系统课程设计代码 #
# Hunan University of Science of Technology <br /> 2012 Operating System Course Design #

本项目为湖南科技大学2012级操作系统课程设计的示例代码。由于本届的《操作系统课程设计指导书》中的示例代码错误实在过多，多到已经实在是<strong>不忍直视</strong>的程度，故我特此写一份正确的示例代码。希望同学们能够得到正确的指引。

This project is the demo code for Hunan University of Science of Technology 2012 Operating System Course Design. There are too many errors in "Guidebook of Operating System Course Design" and therefore it <strong>really cannot be tolerated</strong>. As a result, I write the demo code on purpose. May the schoolmates be able to get proper guidance.

有人问这个md文件怎么打开。这个很简单。md文件是Markdown语言源文件，属于纯文本文件，用一般的纯文件编辑器都可以查看。如果需要看即时预览的效果，在Windows操作系统中可以用[MardownPad](http://markdownpad.com/)来即时预览。在Linux操作系统中可以用[ReText](http://sourceforge.net/projects/retext/)来即时预览，一般可以用 `sudo apt-get install retext` 命令或 `sudo yum install retext` 命令来下载安装ReText。在Mac OS X操作系统中可以用[Mou](http://mouapp.com/)来即时预览。

Someone has asked how to open the md file. It is very easy. File with extension md stands for the Markdown source file, belonging to plain text file type, and therefore can be opened by any plain text editor. If instance preview is needed, you can use [MardownPad](http://markdownpad.com/) in Windows, [ReText](http://sourceforge.net/projects/retext/) in Linux (just simply type `sudo apt-get install retext` command or `sudo yum install retext` command), or [Mou](http://mouapp.com/) in Mac OS X.

## 实验一 Windows进程初识 ##

### 任务一 ###

熟悉Windows下的编码、编译、链接、执行流程。学会简单的Hello World编码流程。（清单1-1）

### 任务二 ###

编写一个程序，测试指定进程的内核态运行时间与用户态运行时间比。（清单1-2、清单1-3）

## 实验二 Windows进程管理 ##

### 任务一 ###
创建指定数量的Windows进程。

### 任务二 ###

父子进程的简单通信及进程的终止。

## 实验三 Windows线程同步的经典算法 ##

### 任务一 ###

生产者消费者问题。

### 任务二 ###

第一类读者写者问题——读者优先。

## 实验四 Windows存储管理 ##

### 任务一 问答问题 ###


- 什么是“分页过程”？
- 什么是“内存共享”？
- 什么是“未分页合并内存”和“分页合并内存”？
- Windows分页文件默认设置的最小容量和最大容量是多少？

### 任务二 学习使用Windows任务管理器 ###

- 查看指定进程的映像名称、进程标识符（PID）、CPU、CPU时间、内存使用
- 查看指定进程的页面缓冲池、非页面缓冲池

### 任务三 提高分页性能 ###

- 虚拟内存页面文件的建立
- 页面文件的合理分配
- 计算分页大小

### 任务四 编码 ###

编码Win32控制台应用程序，读取虚拟内存空间的相关信息，需要读取的数据是：

- 虚拟内存页面大小、最小应用程序地址、最大应用程序地址
- 当前进程对每个虚拟内存块的使用情况、块的状态、块的保护情况、已载入的模块。

## 实验五 熟悉Unix/Linux操作系统 ##

### 任务一 基本操作 ###

使用自己的注册号注册Unix或Linux操作系统，学习如何增加用户、设置用户缺省信息与修改用户口令、修改用户和用户组属性、删除用户注册号和用户组、目录操作、文件操作。

### 任务二 学会使用vi编辑器 ###

弄清“命令模式”、“插入模式”和“底行模式”。学会h（左）、j（下）、k（上）、l（右）的使用。学会几个基本的底行模式的命令，例如“:w”表示保存，“:q”表示退出，“:wq”表示保存并退出，“:q!”表示不保存直接退出。

### 任务三 编程题 ###
在Linux操作系统中，使用C语言编写一个词频统计程序，使之能够给出各个单词在输入文件中的出现次数。

### 任务四 编程题 ###
在Linux操作系统中，使用C语言编写一个反向打印程序，使之能按与输入文本行相反的次序来打印。

## 实验六 Linux进程间通信 ##

在类Unix操作系统中，进程间通信的方式有四种，分别是：管道、FIFO、XSI IPC、套接字。本实验要求写的是XSI的进程间通信。

### 任务一 消息队列 ###

使用msgget、msgsnd、msgrcv、msgctl编制消息的发送和接收程序。

父进程先使用fork()系统调用来创建两个子进程，两个子进程分别调用server()函数和client()函数。进行通信。

server进程和client进程都使用key为75的消息队列。

server进程用key为75的消息队列，等待其它进程发来的消息。当遇到类型为1的消息，则进程结束。

client进程用key为75的消息队列，先后发送类型从10到1的消息，然后退出。

### 任务二 共享内存段 ###

使用shmget、shmsnd、shmrcv、shmctl编制消息的发送和接收程序。

父进程先使用fork()系统调用来创建两个子进程，两个子进程分别调用server()函数和client()函数。进行通信。

server进程和client进程都使用key为75的共享内存段。

server进程用key为75的共享内存段，把第一个字节置为-1，作为数据空的标志，等待其它进程发来的消息。当该字节的值发生变化时，表示收到了信息，可以进行处理。读取完再置为-1。当消息为0时，退出。

client进程用key为75的共享内存段。当第一个字节为-1的时候，说明server已就绪，可以发送消息。分别发送9至0的消息。操作完后退出。

## 实验七 Linux简单二级文件系统设计 ##

为Linux设计一个简单的二级文件系统，要求至少做到以下几点：

1、 可以实现下列几条命令

<pre>
    用户登录命令
    列出文件命令
    创建文件命令
    删除文件命令
    打开文件命令
    关闭文件命令
    读取文件命令
    写入文件命令
</pre>

2、 列目录时要列出文件名、物理地址、保护码和文件长度

3、 可以对文件进行读写保护
