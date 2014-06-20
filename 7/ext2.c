#include "common.h"
#include "ext2_fs.h"

#define BLOCK_LENGTH 1024
char useless_block[BLOCK_LENGTH];

static struct ext2_super_block super_block;

void read_super_block_data(void)
{
	// read the boot block
	ssize_t bytes = read(fd_ext2, useless_block, BLOCK_LENGTH);
	if ( bytes == -1 )
	{
		perror("Read filesystem failed");
		exit(EXIT_FAILURE);
	}
	else if ( bytes == 0 )
	{
		fprintf(stderr, "This is not a valid ext2 filesystem\n");
		exit(EXIT_FAILURE);
	}


	bytes = read(fd_ext2, &super_block, sizeof(super_block));
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
