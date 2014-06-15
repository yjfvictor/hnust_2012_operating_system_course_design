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

