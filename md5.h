#ifndef __MD5_H_
#define __MD5_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int md5(const char *str, size_t len, char *output, size_t out_max_len);
int md5_file(const char *file_path, char *output, size_t out_max_len);
#endif
