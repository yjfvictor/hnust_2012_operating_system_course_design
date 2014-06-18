#ifndef _PARSE_STRING_H
#define _PARSE_STRING_H

bool split_command(const char * command, int * pargc, char *** pargv);
bool run_command(int argc, char * const * argv);
void print_last_error(void);

#endif // _PARSE_STRING_H
