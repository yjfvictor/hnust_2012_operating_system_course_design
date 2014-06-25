#include "common.h"
#include "ext2_fs.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BASE_OFFSET 1024 
#define BLOCK_OFFSET(block) (BASE_OFFSET + ( (block) - 1) * block_size)

static struct ext2_super_block super;
static unsigned int block_size = 0;

const int inode_no = 2;

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

void read_inode(const struct ext2_group_desc * pgroup, struct ext2_inode * pinode)
{
	lseek(fd_ext2, BLOCK_OFFSET(pgroup->bg_inode_table)+(inode_no-1)*sizeof(struct ext2_inode), SEEK_SET);
	read(fd_ext2, pinode, sizeof(struct ext2_inode));
}

void read_dir(const struct ext2_inode * pinode)
{
	void * block;

	if (S_ISDIR(pinode->i_mode))
	{
		struct ext2_dir_entry_2 * entry;
		unsigned int size = 0;

		if ((block = malloc(block_size)) == NULL)
		{
			fprintf(stderr, "No sufficient memory\n");
			exit(EXIT_FAILURE);
		}

		lseek(fd_ext2, BLOCK_OFFSET(pinode->i_block[0]), SEEK_SET);
		read(fd_ext2, block, block_size);                // read block from disk

		entry = (struct ext2_dir_entry_2 *) block;  // first entry in the directory
		// Notice that the list may be terminated with a NULL entry (entry->inode == NULL)
		while((size < pinode->i_size) && entry->inode)
		{
			printf("%s\n", entry->name);
			entry = (struct ext2_dir_entry_2 *)((void *)entry + entry->rec_len);
			size += entry->rec_len;
		}
		free(block);
	}
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
	struct ext2_group_desc group;
	struct ext2_inode inode;

	assert(absolute_path != NULL);
	assert(absolute_path[0] == '/');

	read_group_descriptor(&group);
	read_inode(&group, &inode);
	read_dir(&inode);
}

bool remove_file(const char * absolute_path)  // rm
{
	assert(absolute_path != NULL);
	assert(absolute_path[0] == '/');
	return true;
}
