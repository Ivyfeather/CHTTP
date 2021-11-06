#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// returns a pointer to a new allocated type
#define MALLOC(type) (type *)malloc(sizeof(type))

#define FILE_BUFF_LEN 10000

enum http_method {UNKNOWN = 0, GET, POST};
extern char *method_str[3];

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
    char *content;
}header;


// from get
int send_file(char *name, int cs);
int file_size(FILE *fp);

// from parser
header *http_decoder(char *str);
void print_options(header *hd);
#endif 