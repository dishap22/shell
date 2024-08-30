#ifndef __SEEK_H
#define __SEEK_H

int seek(char **args);
void print_match(const char *path, int is_dir);
int search_directory(const char *dir_path, const char *search_name, int only_dirs, int only_files, int use_e_flag, char **found_file, char **found_dir);
int match_name(const char *entry_name, const char *search_name);

#endif