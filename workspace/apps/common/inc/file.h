#ifndef FILE_H_
#define FILE_H_

<#include <qapi_fs_types.h>

#undef PATH_MAX
#define PATH_MAX QAPI_FS_NAME_MAX


int *fopen(const char *path, const char *mode);
int fclose(int *fd);
int fseek(int *fd, long offset, int whence);
long ftell(int *fd);
int fstat(int fd, struct qapi_FS_Stat_Type_s *statbuf);
int lstat(const char *pathname, struct qapi_FS_Stat_Type_s *statbuf);
size_t fread(void *ptr, size_t size, size_t nmemb, int *fd);

#endif