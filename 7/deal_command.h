#ifndef _DEAL_COMMAND_H
#define _DEAL_COMMAND_H

bool ls( const char * path, bool all, bool almost_all, bool long_list, const char * filename );
bool cd(const char * path);
bool cat( const char * path );
bool rm( const char * current_path, const char * const * const remove_paths, bool recursive, bool force );

#endif // _DEAL_COMMAND_H
