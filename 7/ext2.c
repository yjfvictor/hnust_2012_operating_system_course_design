/**
 * @file   ext2.c
 * @brief  ext2文件系统的读写、遍历等操作
 * @author 叶剑飞
 * @date   2014年6月
 */

#include "common.h"
#include "ext2_fs.h"
#include "parse_string.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_PERMISSION_STRING_LENGTH 12
#define BASE_OFFSET 1024 
#define BLOCK_OFFSET(block) (BASE_OFFSET + ( (block) - 1) * block_size)


static struct ext2_super_block super;
static unsigned int block_size = 0;

const int root_inode_no = 2;

/**
 * @brief 读取超级块的数据
 */
void read_super_block_data(void)
{
	ssize_t bytes = -1;

	// jump the boot block
	off_t offset = lseek(fd_ext2, BASE_OFFSET, SEEK_SET);
	if ( offset == (off_t)(-1) )
	{
		perror("Read filesystem failed");
		exit(EXIT_FAILURE);
	}
	else if ( bytes == 0 )
	{
		fprintf(stderr, "Not a Ext2 filesystem\n");
		exit(EXIT_FAILURE);
	}


	// read super-block data
	bytes = read(fd_ext2, &super, sizeof(super));
	if ( bytes == -1 )
	{
		perror("ext2 filesystem error");
		exit(EXIT_FAILURE);
	}
	else if ( bytes == 0 )
	{
		fprintf(stderr, "ext2 filesystem error\n");
		exit(EXIT_FAILURE);
	}
	else if ( super.s_magic != EXT2_SUPER_MAGIC )
	{
		fprintf(stderr, "Not a Ext2 filesystem\n");
		exit(EXIT_FAILURE);
	}

	block_size = 1024 << super.s_log_block_size;
}

/**
 * @brief 读取组描述符的数据
 * @param[out] pgroup 组描述符
 */
void read_group_descriptor(struct ext2_group_desc * pgroup)
{
	ssize_t bytes = -1;

	lseek(fd_ext2, BASE_OFFSET + block_size, SEEK_SET);
	bytes = read(fd_ext2, pgroup, sizeof(*pgroup));
	if ( bytes == -1 )
	{
		perror("ext2 filesystem error");
		exit(EXIT_FAILURE);
	}
	else if ( bytes == 0 )
	{
		fprintf(stderr, "ext2 filesystem error\n");
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief 读取inode的数据
 * @param[in]  pgroup    组描述符
 * @param[out] pinode    inode
 * @param[in]  inode_no  inode号
 */
void read_inode(const struct ext2_group_desc * pgroup, struct ext2_inode * pinode, int inode_no )
{
	lseek(fd_ext2, BLOCK_OFFSET(pgroup->bg_inode_table)+(inode_no-1)*sizeof(struct ext2_inode), SEEK_SET);
	read(fd_ext2, pinode, sizeof(struct ext2_inode));
}

/**
 * @brief 获取文件的inode号
 * @param[in] absolute_path  文件的绝对路径
 * @return 返回文件的inode号，失败返回-1
 */
int get_file_inode_no(const char * absolute_path)
{
	struct ext2_group_desc group;
	struct ext2_inode inode;
	void * block = NULL;
	int inode_no = root_inode_no;

	link_list path_list = NULL;
	link_list p = NULL;

	assert(absolute_path != NULL);
	assert(absolute_path[0] == '/');

	generate_path_linklist(&path_list, absolute_path);
	for ( p = path_list; p != NULL; p = p->next )
	{
		read_group_descriptor(&group);
		read_inode(&group, &inode, inode_no);

		struct ext2_dir_entry_2 * entry = NULL;
		unsigned int size = 0;

		if ((block = malloc(block_size)) == NULL)
		{
			fprintf(stderr, "No sufficient memory\n");
			exit(EXIT_FAILURE);
		}

		lseek(fd_ext2, BLOCK_OFFSET(inode.i_block[0]), SEEK_SET);
		read(fd_ext2, block, block_size);                // read block from disk

		entry = (struct ext2_dir_entry_2 *) block;  // first entry in the directory
		// Notice that the list may be terminated with a NULL entry (entry->inode == NULL)
		if ( p->next == NULL )
		{
			free(block);
			block = NULL;
			break;
		}
		else
		{
			inode_no = -1;
			while((size < inode.i_size) && entry->inode)
			{
				char file_name[EXT2_NAME_LEN+1];
				memcpy(file_name, entry->name, entry->name_len);
				file_name[entry->name_len] = '\0';
				if ( !strcmp(file_name, p->next->directory_name) )
				{
					inode_no = entry->inode;
					break;
				}
				entry = (struct ext2_dir_entry_2 *)((void *)entry + entry->rec_len);
				size += entry->rec_len;
			}
			free(block);
			block = entry = NULL;
		}

		if( inode_no == -1 )
			break;
	}
	destroy_path_linklist(&path_list);
	return inode_no;
}

/**
 * @brief 以文本形式输出entry
 * @param[in] entry      指定文件的entry
 * @param[in] long_list  是否以长列表形式输出
 * @param[in] inode_no   inode号 
 */
void output_entry(const struct ext2_dir_entry_2 * entry, bool long_list, int inode_no)
{
	char file_name[EXT2_NAME_LEN+1];
	struct ext2_group_desc group;
	struct ext2_inode inode;

	memcpy(file_name, entry->name, entry->name_len);
	file_name[entry->name_len] = '\0';

	if ( long_list )
	{
		char permission[MAX_PERMISSION_STRING_LENGTH];
		memset(permission, 0, sizeof(permission));
		read_group_descriptor(&group);
		read_inode(&group, &inode, inode_no);

		if ( S_ISDIR(inode.i_mode) )
			permission[0] = 'd';
		else
			permission[0] = '-';

		if ( inode.i_mode & S_IRUSR )
			permission[1] = 'r';
		else
			permission[1] = '-';

		if ( inode.i_mode & S_IWUSR )
			permission[2] = 'w';
		else
			permission[2] = '-';
	
		if ( inode.i_mode & S_IXUSR )
			permission[3] = 'x';
		else
			permission[3] = '-';
	
		if ( inode.i_mode & S_IRGRP )
			permission[4] = 'r';
		else
			permission[4] = '-';
	
		if ( inode.i_mode & S_IWGRP )
			permission[5] = 'w';
		else
			permission[5] = '-';
	
		if ( inode.i_mode & S_IXGRP )
			permission[6] = 'x';
		else
			permission[6] = '-';
	
		if ( inode.i_mode & S_IROTH )
			permission[7] = 'r';
		else
			permission[7] = '-';
	
		if ( inode.i_mode & S_IWOTH )
			permission[8] = 'w';
		else
			permission[8] = '-';
	
		if ( inode.i_mode & S_IXOTH )
			permission[9] = 'x';
		else
			permission[9] = '-';
	
		printf("%s\t%s\n", permission, file_name);
	}
	else
		printf("%s\t", file_name);
}

/**
 * @brief 输出文件 
 * @param[in] absolute_path 文件的绝对路径
 * @param[in] all           是否输出全部文件，包括隐藏文件，以及.和..
 * @param[in] almost_all    是否“几乎全部”输出，包括隐藏文件，不包括.和..
 * @param[in] long_list     是否按长列表方式输出
 */
void output_files( const char * absolute_path, bool all, bool almost_all, bool long_list )  // ls
{
	struct ext2_group_desc group;
	struct ext2_inode inode;
	void * block = NULL;

	int inode_no = get_file_inode_no(absolute_path);
	if ( inode_no == -1 )
	{
		fprintf(stderr, "Current directory does not exist.\n");
		return;
	}
	read_group_descriptor(&group);
	read_inode(&group, &inode, inode_no);

	if (S_ISDIR(inode.i_mode))
	{
		struct ext2_dir_entry_2 * entry = NULL;
		unsigned int size = 0;
		if ((block = malloc(block_size)) == NULL)
		{
			fprintf(stderr, "Insufficient memory.\n");
			exit(EXIT_FAILURE);
		}

		lseek(fd_ext2, BLOCK_OFFSET(inode.i_block[0]), SEEK_SET);
		read(fd_ext2, block, block_size);                // read block from disk

		entry = (struct ext2_dir_entry_2 *) block;  // first entry in the directory
		// Notice that the list may be terminated with a NULL entry (entry->inode == NULL)
		while((size < inode.i_size) && entry->inode)
		{
			if ( entry->name[0] == '.' )
			{
				if ( all )
					output_entry(entry, long_list, entry->inode);
				else if ( almost_all )
				{
					if ( strcmp(entry->name, ".") && strcmp(entry->name, "..") )
						output_entry(entry, long_list, entry->inode);
				}
			}
			else
				output_entry(entry, long_list, entry->inode);
			entry = (struct ext2_dir_entry_2 *)((void *)entry + entry->rec_len);
			size += entry->rec_len;
		}
		puts("");

		free(block);
		block = entry = NULL;
	}
	else
		fprintf(stderr, "Current directory does not exist.\n");
}

/**
 * @brief 判断是否存在该路径
 * @param[in] absolute_path 绝对路径
 * @return 存在返回true，不存在返回false
 */
bool exsit_path(const char * absolute_path)
{
	assert(absolute_path != NULL);
	assert(absolute_path[0] == '/');

	if ( get_file_inode_no(absolute_path) == -1 )
		return false;

	return true;
}

/**
 * @brief 判断是否存在该目录
 * @param[in] absolute_path 绝对路径
 * @return 存在返回true，不存在返回false
 */
bool exsit_dir_path(const char * absolute_path)
{
	struct ext2_group_desc group;
	struct ext2_inode inode;
	int inode_no = -1;

	assert(absolute_path != NULL);
	assert(absolute_path[0] == '/');

	if ( ( inode_no = get_file_inode_no(absolute_path) ) == -1 )
		return false;

	read_group_descriptor(&group);
	read_inode(&group, &inode, inode_no);

	if (S_ISDIR(inode.i_mode))
		return true;
	else
		return false;
}

/**
 * @brief 输出文件中的内容
 * @details cat函数调用这个函数
 * @param[in] absolute_path 绝对路径
 * @return 成功返回true，失败返回false
 */
bool output_file_data(const char * absolute_path) // cat
{
	int i, j;
	int inode_no = -1;

	struct ext2_group_desc group;
	struct ext2_inode inode;
	void * block = NULL;

	assert(absolute_path != NULL);
	assert(absolute_path[0] == '/');

	inode_no = get_file_inode_no(absolute_path);

	if ( inode_no == -1 )
	{
		fprintf(stderr, "cat: %s: No such file or directory\n", absolute_path);
		return false;
	}

	read_group_descriptor(&group);
	read_inode(&group, &inode, inode_no);
	if (S_ISDIR(inode.i_mode))
	{
		fprintf(stderr, "cat: %s: Is a directory\n", absolute_path);
		return false;
	}
	else if ( S_ISREG(inode.i_mode) )
	{
		if ((block = malloc(block_size)) == NULL)
		{
			fprintf(stderr, "Insufficient memory.\n");
			exit(EXIT_FAILURE);
		}

		for ( i = 0; i < inode.i_blocks; ++ i )
		{
			lseek(fd_ext2, BLOCK_OFFSET(inode.i_block[i]), SEEK_SET);
			read(fd_ext2, block, block_size);                // read block from disk
			if ( i < EXT2_NDIR_BLOCKS )
			{
				for ( j = 0; j < block_size; ++ j )
					putchar(((char *)block)[j]);
			}
			else if ( i == EXT2_IND_BLOCK )
			{
				// very complicated here, unfinished
				/*
				inode_no = inode.i_block[i];
				read_group_descriptor(&group);
				read_inode(&group, &inode_single_indirect_block, inode_no);
				for ( j = 0; j < 4 && i + j < block_size; ++ j )
				{
					lseek(fd_ext2, BLOCK_OFFSET(inode_single_indirect_block.i_block[j]), SEEK_SET);
					read(fd_ext2, block, block_size);                // read block from disk
					for ( k = 0; k < block_size; ++ k )
						putchar(((char *)block)[k]);
				}
				*/
			}
			else if ( i == EXT2_DIND_BLOCK )
			{
			}
			else if ( i == EXT2_TIND_BLOCK )
			{
			}
		}
	}
	else
	{
		fprintf(stderr, "cat: %s: Not supported\n", absolute_path);
		return false;
	}

	return true;
}

/**
 * @brief 删除文件
 * @param[in] absolute_path 文件的绝对路径
 * @return 删除成功返回true，删除失败返回false
 */
bool remove_file(const char * absolute_path)  // rm
{
	struct ext2_group_desc group;
	struct ext2_inode inode;
	ssize_t bytesread = 0;

	void * block = NULL;
	int inode_no = root_inode_no;
	link_list path_list = NULL;
	link_list p = NULL;

	assert(absolute_path != NULL);
	assert(absolute_path[0] == '/');

	generate_path_linklist(&path_list, absolute_path);
	for ( p = path_list; p->next != NULL; p = p->next )
	{
		read_group_descriptor(&group);
		read_inode(&group, &inode, inode_no);

		struct ext2_dir_entry_2 * last_entry = NULL;
		struct ext2_dir_entry_2 * this_entry = NULL;
		unsigned int size = 0;

		if ((block = malloc(block_size)) == NULL)
		{
			fprintf(stderr, "No sufficient memory\n");
			exit(EXIT_FAILURE);
		}

		lseek(fd_ext2, BLOCK_OFFSET(inode.i_block[0]), SEEK_SET);
		bytesread = read(fd_ext2, block, block_size);                // read block from disk

		this_entry = (struct ext2_dir_entry_2 *) block;  // first entry in the directory
		// Notice that the list may be terminated with a NULL entry (entry->inode == NULL)

		inode_no = -1;
		while((size < inode.i_size) && this_entry->inode)
		{
			char file_name[EXT2_NAME_LEN+1];
			memcpy(file_name, this_entry->name, this_entry->name_len);
			file_name[this_entry->name_len] = '\0';
			if ( !strcmp(file_name, p->next->directory_name) )
			{
				inode_no = this_entry->inode;
				if ( p->next->next == NULL )
				{
					last_entry->rec_len += this_entry->rec_len;
					lseek(fd_ext2, BLOCK_OFFSET(inode.i_block[0]), SEEK_SET);
					write(fd_ext2, block, bytesread);
				}
				break;
			}
			last_entry = this_entry;
			this_entry = (struct ext2_dir_entry_2 *)((void *)this_entry + this_entry->rec_len);
			size += this_entry->rec_len;
		}

		free(block);
		block = this_entry = last_entry = NULL;

		if( inode_no == -1 )
			break;
	}
	destroy_path_linklist(&path_list);


	if ( inode_no == -1 )
		return false;

	return true;
}
