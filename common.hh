#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

// #define CHUNKED

#ifdef HTTPS
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

// returns a pointer to a new allocated type
#define MALLOC(type) (type *)malloc(sizeof(type))

#define SOCK_BUFF_LEN 4000
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
    int content_length;
    int keep_alive;
    char *content;
}header;

header *http_decoder(char *str);
int file_size(FILE *fp);

int send_file(char *name, void *net);
int receive_file(header *hd, void *net);

#ifndef HTTPS
    #define WRITE(to, str, len) write(to, str, len)
    #define READ(from, str, len) read(from, str, len)
#else
    #define WRITE(to, str, len) SSL_write(to, str, len)
    #define READ(from, str, len) SSL_read(from, str, len)
#endif

#endif // __COMMON_H_