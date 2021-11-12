#include "common.hh"
#include "log.hh"

int echo_post(header *hd, int cs){
    char content[1000];
    snprintf(content,100,"Welcome!\n");
    char *kv;

    while((kv = strsep(&hd->content, "&"))){
        strcat(content, "\n");
        strcat(content, kv);
    }

	char head[100];
	snprintf(head, 100, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", strlen(content));
	write(cs, head, strlen(head));
    write(cs, content, strlen(content));
    printf("CCCC:%s",content);
    return 0;
}