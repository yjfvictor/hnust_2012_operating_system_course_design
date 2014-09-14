/**
 * @file   ext2.h
 * @brief  ext2文件系统操作的一些函数的函数声明
 * @author 叶剑飞
 */

#ifndef _EXT2_H
#define _EXT2_H

/**
 * @brief 读取超级块的数据
 */
void read_super_block_data(void);

/**
 * @brief 判断是否存在该路径
 * @param[in] absolute_path 绝对路径
 * @return 存在返回true，不存在返回false
 */
bool exsit_path(const char * absolute_path);

/**
 * @brief 判断是否存在该目录
 * @param[in] absolute_path 绝对路径
 * @return 存在返回true，不存在返回false
 */
bool exsit_dir_path(const char * absolute_path);

/**
 * @brief 输出文件中的内容
 * @details cat函数调用这个函数
 * @param[in] absolute_path 绝对路径
 * @return 成功返回true，失败返回false
 */
bool output_file_data(const char * absolute_path);

/**
 * @brief 删除文件
 * @param[in] absolute_path 文件的绝对路径
 * @return 删除成功返回true，删除失败返回false
 */
bool remove_file(const char * absolute_path);

/**
 * @brief 输出文件 
 * @param[in] absolute_path 文件的绝对路径
 * @param[in] all           是否输出全部文件，包括隐藏文件，以及.和..
 * @param[in] almost_all    是否“几乎全部”输出，包括隐藏文件，不包括.和..
 * @param[in] long_list     是否按长列表方式输出
 */
void output_files( const char * absolute_path, bool all, bool almost_all, bool long_list );

#endif  // _EXT2_H
