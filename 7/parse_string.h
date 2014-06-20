#ifndef _PARSE_STRING_H
#define _PARSE_STRING_H

bool split_command(const char * command, int * pargc, char *** pargv);
bool run_command(int argc, char * const * argv);
void print_last_error(void);
void get_path_without_dot_or_dotdot(char * absolute_path);
void get_absolute_path(char * absolute_path, const char * path);

#endif // _PARSE_STRING_H
