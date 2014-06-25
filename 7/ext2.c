#include "common.h"
#include "ext2_fs.h"
#include "parse_string.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BASE_OFFSET 1024 
#define BLOCK_OFFSET(block) (BASE_OFFSET + ( (block) - 1) * block_size)

static struct ext2_super_block super;
static unsigned int block_size = 0;

const int root_inode_no = 2;

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

void read_inode(const struct ext2_group_desc * pgroup, struct ext2_inode * pinode, int inode_no )
{
	lseek(fd_ext2, BLOCK_OFFSET(pgroup->bg_inode_table)+(inode_no-1)*sizeof(struct ext2_inode), SEEK_SET);
	read(fd_ext2, pinode, sizeof(struct ext2_inode));
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

void output_entry(const struct ext2_dir_entry_2 * entry, bool long_list)
{
#ifdef _DEBUG
	//printf("%10u\t", entry->inode);
#endif
	char file_name[EXT2_NAME_LEN+1];
	memcpy(file_name, entry->name, entry->name_len);
	file_name[entry->name_len] = '\0';

	if ( long_list )
	{
		printf("%s\n", file_name);
	}
	else
		printf("%s\t", file_name);
}

void output_files( const char * absolute_path, bool all, bool almost_all, bool long_list )  // ls
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
		inode_no = -1;
		if (S_ISDIR(inode.i_mode))
		{
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
				while((size < inode.i_size) && entry->inode)
				{
					if ( entry->name[0] == '.' )
					{
						if ( all )
							output_entry(entry, long_list);
						else if ( almost_all )
						{
							if ( strcmp(entry->name, ".") && strcmp(entry->name, "..") )
								output_entry(entry, long_list);
						}
					}
					else
						output_entry(entry, long_list);
					entry = (struct ext2_dir_entry_2 *)((void *)entry + entry->rec_len);
					size += entry->rec_len;
				}
				puts("");
				break;
			}
			else
			{
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
			}
			free(block);
		}
		if( inode_no == -1 )
		{
			fprintf(stderr, "Current directory does not exist.\n");
			break;
		}
	}
	destroy_path_linklist(&path_list);
}

bool remove_file(const char * absolute_path)  // rm
{
	assert(absolute_path != NULL);
	assert(absolute_path[0] == '/');
	return true;
}
