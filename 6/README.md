# Linux进程间通信 #

进程间通信（InterProcess Communication），简称IPC。在类Unix操作系统中，进程间通信的方式有五种，分别是：

- 管道（pipe）
- FIFO
- XSI IPC
- POSIX IPC
- 套接字（socket）

其中，XSI IPC又分为三种，分别是：

- 消息队列（message queue）
- 信号量（semaphore）
- 共享内存（shared memory）

在2012级的《操作系统课程设计指导书》的实验六中，用到的进程间通信方式为XSI IPC。任务一用的是消息队列的方式，任务二用的是共享内存的方式。

## 系统调用函数的介绍 ##

### fork ###

#### 函数原型 ####

    pid_t fork(void);

#### 头文件 ####

    #include <unistd.h>

#### 函数功能 ####

“进程分叉”——创建POSIX进程。

创建出来的子进程与父进程的内存地址的数据是完全一样的，即把整个父进程的内存完全复制到子进程的内存中（底层实现可能采用“写时复制”(COW)技术来提高效率）。然后父进程和子进程一起接着往下运行。

#### 返回值说明 ####

如果子进程创建失败，返回值为-1，并置外部变量errno的值。可以用perror函数来输出进程创建失败的原因。

如果子进程创建成功，那么该函数给父进程返回子进程的进程标识符，并且该函数给子进程返回零。

### msgget ###

#### 函数原型 ####

    int msgget(key_t key, int msgflg);

#### 头文件 ####

    #include <sys/types.h>
    #include <sys/ipc.h>
    #include <sys/msg.h>

#### 函数功能 ####

获取 System V 消息队列标识符

#### 形参说明 ####

- key（输入参数）

消息队列的键值，必须为非负整数。该参数可以取值为IPC_PRIVATE，即0，表示生成一个新的消息队列。

- msgflg（输入参数）

消息队列的标志。可以是如下几个值的位或：

<table border="1" cellspacing="1">
<tr><th>取值</th><th>意义</th></tr>
<tr><td>IPC_CREAT</td><td>如果键值为指定值的消息队列不存在，则创建新的消息队列</td></tr>
<tr><td>IPC_EXCL</td><td>创建互斥消息队列</td></tr>
<tr><td>0400</td><td>当前用户可读</td></tr>
<tr><td>0200</td><td>当前用户可写</td></tr>
<tr><td>0040</td><td>当前用户组可读</td></tr>
<tr><td>0020</td><td>当前用户组可写</td></tr>
<tr><td>0004</td><td>其它用户组可读</td></tr>
<tr><td>0002</td><td>其它用户组可写</td></tr>
</table>


#### 返回值说明 ####
       
如果成功，则返回消息队列标识符（非负整数），否则返回-1，并置外部变量errno的值，可以用perror来输出出错原因。

### msgsnd ###

#### 函数原型 ####

    int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);

#### 头文件 ####

    #include <sys/types.h>
    #include <sys/ipc.h>
    #include <sys/msg.h>

#### 函数功能 ####

发送 System V 消息。

#### 形参说明 ####

- msqid（输入参数）

消息队列标识符。消息将发往该消息队列。

- msgp（输入参数）

指向如下类型的变长结构体变量的指针：

    struct msgbuf {
        long mtype;      /* 消息类型，必须大于零 */
        char mtext[];    /* 消息数据，为gcc零长数组或C99弹性数组成员 */
    };

需要给mtype赋一个正数值，表示消息类型。待发送的消息置于mtext中。mtext的长度可以为零（即可以没有mtext成员）。

- msgsz（输入参数）

待发送的消息的实际长度，即`((struct msgbuf *)msgp)->mtext`数组的实际长度，以字节为单位。

- msgflg（输入参数）

消息发送标志，可以置IPC_NOWAIT值，表示在超过队列限制时，直接丢弃待发送的消息。

#### 返回值说明 ####

如果消息发送失败，则返回-1，并置errno值。

如果消息发送成功，则返回0。

### msgrcv ###

#### 函数原型 ####

    ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp,
                   int msgflg);

#### 头文件 ####

    #include <sys/types.h>
    #include <sys/ipc.h>
    #include <sys/msg.h>

#### 函数功能 ####

接收 System V 消息。

#### 形参说明 ####

- msqid（输入参数）

消息队列标识符。将接收该消息队列的消息。

- msgp（输入输出参数）

指向如下类型的变长结构体变量的指针：

    struct msgbuf {
        long mtype;      /* 消息类型，必须大于零 */
        char mtext[];    /* 消息数据，为gcc零长数组或C99弹性数组成员 */
    };

mtype为输入参数，需要给mtype赋一个正数值，表示消息类型。mtext为输出参数，消息将会接收到mtext中。

- msgsz（输入参数）

待接收消息的最大长度，即`((struct msgbuf *)msgp)->mtext`数组允许接收的消息的最大长度，以字节为单位。

- msgtyp（输入参数）

指定下列请求的消息类型：
<table border="1" cellspacing="1">
<tr><th>取值</th><th>意义</th></tr>
<tr><td>0</td><td>读取指定消息队列中的第一条消息</td></tr>
<tr><td>正数</td><td>如果msgflg置了MSG_EXCEPT值，则从指定消息队列中读取类型不等于msgtyp的第一条消息。否则读取指定消息队列中的第一条消息</td></tr>
<tr><td>负数</td><td>在消息队列中读取小于或等于msgtyp的绝对值的最低的类型的第一条消息</td></tr>
</table>

- msgflg（输入参数）

消息发送标志。可以置以下三个值：

<table border="1" cellspacing="1">
<tr><th>取值</th><th>意义</th></tr>
<tr><td>IPC_NOWAIT</td><td>若指定消息队列为空，则不阻塞立刻返回</td></tr>
<tr><td>MSG_EXCEPT</td><td>如果msgtyp>0，则从指定消息队列中读取类型不等于msgtyp的第一条消息</td></tr>
<tr><td>MSG_NOERROR</td><td>如果消息超过msgsz个字节，则直接截断，丢弃掉多出来的字节</td></tr>
</table>

#### 返回值说明 ####

如果消息发送失败，则返回-1，并置errno值。

如果消息发送成功，则返回`((struct msgbuf *)msgp)->mtext`数组读取到的消息的实际字节数。

### msgctl ###

#### 函数原型 ####

    int msgctl(int msqid, int cmd, struct msqid_ds *buf);

#### 头文件 ####

    #include <sys/types.h>
    #include <sys/ipc.h>
    #include <sys/msg.h>

#### 函数功能 ####

System V 信号量控制操作

#### 形参说明 ####

- msqid（输入参数）

消息队列标识符。指定要进行操作的消息队列。

- cmd（输入参数）

对消息队列的操作方式，可以取如下值：

<table border="1" cellspacing="1">
<tr><th>取值</th><th>意义</th></tr>
<tr><td>IPC_STAT</td><td>把指定消息队列的内核数据结构的信息复制到buf参数中</td></tr>
<tr><td>IPC_SET</td><td>把buf中的一些数据写入指定消息队列的内核数据结构中</td></tr>
<tr><td>IPC_RMID</td><td>立刻删除这个消息队列</td></tr>
</table>

- buf（输出参数）

指向`struct msqid_ds`结构体类型变量的指针，指向储存输出的数据。这个结构体类型为：

    struct msqid_ds {
        struct ipc_perm msg_perm;     /* 所有权和许可权 */
        time_t          msg_stime;    /* 最后一次调用msgsnd的时间 */
        time_t          msg_rtime;    /* 最后一次调用msgrcv的时间 */
        time_t          msg_ctime;    /* 最后一次变更时间 */
        unsigned long   __msg_cbytes; /* 消息队列的当前字节数（非标准） */
        msgqnum_t       msg_qnum;     /* 消息队列的当前消息数 */
        msglen_t        msg_qbytes;   /* 消息队列允许的最大字节数 */
        pid_t           msg_lspid;    /* 最后一次调用msgsnd的进程标识符 */
        pid_t           msg_lrpid;    /* 最后一次调用msgrcv的进程标识符 */
    };

#### 返回值说明 ####

如果 System V 信号量控制成功，则返回0。

如果 System V 信号量控制失败，则返回-1，并置errno值。


### shmget ###

#### 函数原型 ####

    int shmget(key_t key, size_t size, int shmflg);

#### 头文件 ####

    #include <sys/ipc.h>
    #include <sys/shm.h>

#### 函数功能 ####

分配 System V 共享内存段

#### 形参说明 ####

- key（输入参数）

共享内存段的键值，必须为非负整数。该参数可以取值为IPC_PRIVATE，即0，表示生成一个新的共享内存队列。

- shmflg（输入参数）

共享内存段的标志。可以是如下几个值的位或：

<table border="1" cellspacing="1">
<tr><th>取值</th><th>意义</th></tr>
<tr><td>IPC_CREAT</td><td>如果键值为指定值的消息队列不存在，则创建新的消息队列</td></tr>
<tr><td>IPC_EXCL</td><td>创建互斥消息队列</td></tr>
<tr><td>SHM_HUGETLB</td><td>用“巨大的内存页”来分配共享内存段</td></tr>
<tr><td>SHM_NORESERVE</td><td>不为这个共享内存段预留交换空间</td></tr>
<tr><td>0400</td><td>当前用户可读</td></tr>
<tr><td>0200</td><td>当前用户可写</td></tr>
<tr><td>0040</td><td>当前用户组可读</td></tr>
<tr><td>0020</td><td>当前用户组可写</td></tr>
<tr><td>0004</td><td>其它用户组可读</td></tr>
<tr><td>0002</td><td>其它用户组可写</td></tr>
</table>


#### 返回值说明 ####
       
如果成功，则返回共享内存段的标识符，否则返回-1，并置外部变量errno的值，可以用perror来输出出错原因。

### shmat ###

#### 函数原型 ####

    void *shmat(int shmid, const void *shmaddr, int shmflg);

#### 头文件 ####

    #include <sys/types.h>
    #include <sys/shm.h>

#### 函数功能 ####

把 System V 共享内存段映射到指定的进程虚拟内存地址空间。

#### 形参说明 ####

- shmid（输入参数）

共享内存段标识符。指定要进行映射的共享内存段。

- shmaddr（输入参数，可为NULL）

如果这个参数不为NULL，那么就把共享内存段映射到以shmaddr为首地址的虚拟内存地址空间。如果这个参数为NULL，就自动分配一个适合的进程虚拟内存地址空间，把共享内存映射过去。

- shmflg（输入参数）

指定内存地址的映射方式。可以取如下值：

<table border="1" cellspacing="1">
<tr><th>取值</th><th>意义</th></tr>
<tr><td>SHM_RND</td><td>向下取整到最接近的SHMLBA多重结构</td></tr>
<tr><td>SHM_RDONLY</td><td>映射过来的虚拟内存地址空间是只读的（即不能做左值使用）</td></tr>
</table>

#### 返回值说明 ####
       
如果映射成功，则返回共享内存映射到的进程虚拟内存地址空间的首地址。

如果映射失败，则返回`(void *)(-1)`，并置errno值。

### shmdt ###

#### 函数原型 ####

    int shmdt(const void *shmaddr);

#### 头文件 ####

    #include <sys/types.h>
    #include <sys/shm.h>

#### 函数功能 ####

解除指定进程虚拟内存地址空间对 System V 共享内存段的映射。

#### 形参说明 ####

- shmaddr（输入参数）

待解除映射的进程虚拟内存地址空间的首地址。

#### 返回值说明 ####
       
如果成功，则返回0。如果失败，则返回-1，并置外部变量errno的值。

### shmctl ###

#### 函数原型 ####

    int shmctl(int shmid, int cmd, struct shmid_ds *buf);

#### 头文件 ####

    #include <sys/ipc.h>
    #include <sys/shm.h>

#### 函数功能 ####

System V 共享内存段控制操作

#### 形参说明 ####

- shmid（输入参数）

共享内存段标识符。指定要进行相应操作的共享内存段。

- cmd（输入参数）

对共享内存段的操作方式。可以取如下值：

<table border="1" cellspacing="1">
<tr><th>取值</th><th>意义</th></tr>
<tr><td>IPC_STAT</td><td>把指定的共享内存段的内核数据结构的信息复制到buf参数中</td></tr>
<tr><td>IPC_SET</td><td>把buf中的一些数据写入指定的共享内存段的内核数据结构中</td></tr>
<tr><td>IPC_RMID</td><td>立刻删除这个共享内存段</td></tr>
</table>

- buf（输出参数）

指向`struct shmid_ds`结构体类型变量的指针，指向储存输出的数据。这个结构体类型为：

    struct shmid_ds {
        struct ipc_perm shm_perm;    /* 所有权和许可权 */
        size_t          shm_segsz;   /* 共享内存段的字节数 */
        time_t          shm_atime;   /* 最后一次映射的时间 */
        time_t          shm_dtime;   /* 最后一次解除映射的时间 */
        time_t          shm_ctime;   /* 最后一次改变的时间 */
        pid_t           shm_cpid;    /* 创建者进程的进程标识符 */
        pid_t           shm_lpid;    /* 最后一个调用shmat/shmdt的进程标识符 */
        shmatt_t        shm_nattch;  /* 当前映射到的虚拟内存地址空间的个数 */
        ...
    };

#### 返回值说明 ####
       
如果操作失败，则返回-1，并置errno值。如果操作成功，则返回0。

## 多进程的gdb调试说明 ##
进程在fork的时候被创建，gdb默认只跟踪调试父进程。如果要跟踪调试子进程，那么请在fork运行之前，在gdb提示符`(gdb)`后面键入`set follow-fork-mode child`命令，然后调试到fork之后，gdb就会跟踪到子进程中。如果已经设置了`set follow-fork-mode child`，又遇到了fork，现在如果又想跟踪调试父进程，那么请在fork运行之前，在gdb提示符`(gdb)`后面键入`set follow-fork-mode parent`命令，然后调试到fork之后，gdb就会跟踪到父进程中。
