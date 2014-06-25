#ifndef _PARSE_STRING_H
#define _PARSE_STRING_H

#include "algorithm_data_structure.h"

bool split_command(const char * command, int * pargc, char *** pargv);
bool run_command(int argc, char * const * argv);
void print_last_error(void);
void get_path_without_dot_or_dotdot(char * absolute_path);
void get_absolute_path(char * absolute_path, const char * path);
void destroy_path_linklist(link_list * ppath_list);
void generate_path_linklist(link_list * ppath_list, const char * absolute_path);

#endif // _PARSE_STRING_H
