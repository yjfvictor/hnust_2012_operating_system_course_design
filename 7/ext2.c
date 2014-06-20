#include "common.h"
#include "ext2_fs.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BASE_OFFSET 1024 
#define BLOCK_OFFSET(block) (BASE_OFFSET + ( (block) - 1) * block_size)

static struct ext2_super_block super;
static unsigned int block_size = 0;

static void read_inode(int fd,                        // file descriptor for the Ext2 file
		int inode_no,                         // the inode number to read
		const struct ext2_group_desc * group, // the block group to which the inode belongs
		struct ext2_inode * inode)            // where to put the inode from disk
{
	lseek(fd, BLOCK_OFFSET(group->bg_inode_table)+(inode_no-1)*sizeof(struct ext2_inode), SEEK_SET);
	read(fd, inode, sizeof(struct ext2_inode));
} /* read_inode() */

void read_super_block_data(void)
{
	int i;
	ssize_t bytes = -1;
	struct ext2_group_desc group;
	struct ext2_inode inode;

	// read the boot block
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

	printf("Reading super-block from device:\n"
			"Inodes count            : %u\n"
			"Blocks count            : %u\n"
			"Reserved blocks count   : %u\n"
			"Free blocks count       : %u\n"
			"Free inodes count       : %u\n"
			"First data block        : %u\n"
			"Block size              : %u\n"
			"Blocks per group        : %u\n"
			"Inodes per group        : %u\n"
			"Creator OS              : %u\n"
			"First non-reserved inode: %u\n"
			"Size of inode structure : %hu\n"
			,
			super.s_inodes_count,  
			super.s_blocks_count,
			super.s_r_blocks_count,     /* reserved blocks count */
			super.s_free_blocks_count,
			super.s_free_inodes_count,
			super.s_first_data_block,
			block_size,
			super.s_blocks_per_group,
			super.s_inodes_per_group,
			super.s_creator_os,
			super.s_first_ino,          /* first non-reserved inode */
			super.s_inode_size);

	puts("");

	lseek(fd_ext2, BASE_OFFSET + block_size, SEEK_SET);
	bytes = read(fd_ext2, &group, sizeof(group));
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
	printf("Reading first group-descriptor from device :\n"
			"Blocks bitmap block: %u\n"
			"Inodes bitmap block: %u\n"
			"Inodes table block : %u\n"
			"Free blocks count  : %u\n"
			"Free inodes count  : %u\n"
			"Directories count  : %u\n"
			,
			group.bg_block_bitmap,
			group.bg_inode_bitmap,
			group.bg_inode_table,
			group.bg_free_blocks_count,
			group.bg_free_inodes_count,
			group.bg_used_dirs_count);    /* directories count */

	puts("");

	read_inode(fd_ext2, 2, &group, &inode);

	printf("Reading root inode\n"
			"File mode: %hu\n"
			"Owner UID: %hu\n"
			"Size     : %u bytes\n"
			"Blocks   : %u\n"
			,
			inode.i_mode,
			inode.i_uid,
			inode.i_size,
			inode.i_blocks);

	for(i=0; i<EXT2_N_BLOCKS; i++)
		if (i < EXT2_NDIR_BLOCKS)         /* direct blocks */
			printf("Block %2u : %u\n", i, inode.i_block[i]);
		else if (i == EXT2_IND_BLOCK)     /* single indirect block */
			printf("Single   : %u\n", inode.i_block[i]);
		else if (i == EXT2_DIND_BLOCK)    /* double indirect block */
			printf("Double   : %u\n", inode.i_block[i]);
		else if (i == EXT2_TIND_BLOCK)    /* triple indirect block */
			printf("Triple   : %u\n", inode.i_block[i]);
}

bool exsit_path(const char * absolute_path)
{
	assert(absolute_path != NULL);
	assert(absolute_path[0] == '/');
	return true;
}

bool output_file_data(const char * absolute_path) // cat
{
	assert(absolute_path != NULL);
	assert(absolute_path[0] == '/');
	return true;
}

void output_files( const char * absolute_path, bool all, bool almost_all, bool long_list )  // ls
{
	assert(absolute_path != NULL);
	assert(absolute_path[0] == '/');
}

bool remove_file(const char * absolute_path)  // rm
{
	assert(absolute_path != NULL);
	assert(absolute_path[0] == '/');
	return true;
}
