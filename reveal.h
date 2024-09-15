#ifndef __REVEAL_H
#define __REVEAL_H

void fileInfo(struct dirent *entry, struct stat *file_stat);
int compare_entries(const void *a, const void *b);
int handle_path(char *path, const char *currentdirectory, const char *homedirectory, const char *previousdirectory);
int list_directory(const char *path, int l_flag, int a_flag);
int reveal(char **args);
int strcasecmp(const char *s1, const char *s2);

#endif