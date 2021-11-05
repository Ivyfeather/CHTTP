#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define FILE_BUFF_LEN 4000
// from chttp_act.cc
int send_file(char *name, int cs);
int file_size(FILE *fp);


#endif 