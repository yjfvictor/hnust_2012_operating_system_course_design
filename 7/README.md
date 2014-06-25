# Linux简单二级文件系统设计 #

这个题目实在太不明确。为了便于实现，这里具体选择一个实际存在的文件系统来操作。我这里选择的是正宗的ext2文件系统来操作。

这个工程巨大，需要很长时间才能完成。如果没有按时完成，我也没啥办法。做到哪里算哪里吧。

实验七采用了部分的Linux内核源代码，所以实验七的全部代码使用GNU GPL许可证发布。

## 数据结构 ##

可以从Linux内核源代码中找到`include/linux/ext2_fs.h`文件，里面定义了很多结构体，这些就是我们需要的数据结构。

## 超级块 ##

    struct ext2_super_block {
        __u32	s_inodes_count;		/* Inodes计数 */
        __u32	s_blocks_count;		/* 块计数 */
        ...
        __u32	s_free_blocks_count;	/* 空闲块计数 */
        __u32	s_free_inodes_count;	/* 空闲inode计数 */
        __u32	s_first_data_block;	/* 第一个数据块 */
        __u32	s_log_block_size;	    /* 块大小 */
        ...
        __u32	s_blocks_per_group;	/* 每组有多少块 */
        ...
        __u16	s_magic;		/* 魔数签名 */
        ...
    };

## 组描述符 ##

    struct ext2_group_desc
    {
        __u32	bg_block_bitmap;	    /* 块位图所在的块号 */
        __u32	bg_inode_bitmap;	    /* inode位图所在的块号 */
        __u32	bg_inode_table;		/* inode表的起始的块号 */
        __u16	bg_free_blocks_count;	/* 空闲块的数目 */
        __u16	bg_free_inodes_count;	/* 空闲inode数目 */
        __u16	bg_used_dirs_count;	/* 目录计数 */
        __u16	bg_pad;
        __u32	bg_reserved[3];
    };

## inode结构 ##

    struct ext2_inode {
        __u16   i_mode;         /* 文件类型和访问权限 */
        __u16   i_uid;          /* 文件所有者的用户标识符 */
        __u32   i_size;         /* 文件大小（字节） */
        __u32   i_atime;        /* 最近一次的访问时间 */
        __u32   i_ctime;        /* 创建时间 */
        __u32   i_mtime;        /* 最近一次的修改时间 */
        __u32   i_dtime;        /* 最近一次不用时间 */
        __u16   i_gid;          /* 文件所有组的用户组标识符 */
        __u16   i_links_count;  /* 链接数 */
        __u32   i_blocks;       /* 该文件占用的磁盘块数 */
        __u32   i_flags;        /* 文件标志 */
        ...
        __u32   i_block[EXT2_N_BLOCKS];  /* 指向的块的指针 */
        ...
    };

如果是目录，那么上面`ext2_inode`结构体中的`i_block[0]`指向的磁盘中储存的数据是下面这个结构体：

    struct ext2_dir_entry_2 {
        __u32    inode;			    /* inode数 */
        __u16    rec_len;		        /* 目录项的长度 */
        __u8     name_len;		    /* 文件名长度 */
        __u8     file_type;
        char     name[EXT2_NAME_LEN];	/* 文件名 */
    };

## 参考资料 ##

- [The Ext2 Filesystem](http://cs.smith.edu/~nhowe/262/oldlabs/ext2.html)
- [John's spec of the second extended filesystem](http://uranus.chrysocome.net/explore2fs/es2fs.htm)
