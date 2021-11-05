#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define FILE_BUFF_LEN 4000
enum http_method {UNKNOWN = 0, GET, POST};
typedef struct _option{
    char *key;
    char *value;
    _option *next;
}option;

typedef struct _header{
    int method;
    char *url;
    int version;
    option *options;
}header;


// from get
int send_file(char *name, int cs);
int file_size(FILE *fp);

// from parser
header *http_decoder(char *str);

#endif 