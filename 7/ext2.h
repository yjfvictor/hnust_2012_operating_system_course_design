/*
 *
 *  ext2文件系统操作的一些函数的函数声明
 *
 *
 *                           叶剑飞
 *
 */

#ifndef _EXT2_H
#define _EXT2_H

void read_super_block_data(void);
bool exsit_path(const char * absolute_path);
bool exsit_dir_path(const char * absolute_path);
bool output_file_data(const char * absolute_path);
bool remove_file(const char * absolute_path);
void output_files( const char * absolute_path, bool all, bool almost_all, bool long_list );

#endif  // _EXT2_H
