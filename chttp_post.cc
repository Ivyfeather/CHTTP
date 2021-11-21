#include "common.hh"
#include "log.hh"

// post a file to server
int receive_file(header *hd, void *net){
    FILE *fp = NULL;

    // create a file using url in HTTP POST
    if( (fp = fopen(hd->url, "w")) == NULL){
        LOG(ERROR, "POST write file failed");
        return -1;
    }

#ifndef HTTPS
	int  from = *((int *)net);
#else
	SSL *from = (SSL *)net;
#endif

    char msg[SOCK_BUFF_LEN];
    int msg_len = 0;
    int file_len = hd->content_length;

    // write the content in this HTTP POST packet
    fwrite(hd->content, 1, strlen(hd->content), fp);
    file_len -= strlen(hd->content);

    // listening for more content in following packets
    while (file_len > 0 && (msg_len = READ(from, msg, sizeof(msg))) > 0) {
        fwrite(msg, 1, msg_len, fp);
        file_len -= msg_len;
    }

    int filesize = file_size(fp);
    fclose(fp);

    // return HTTP 200 OK
    char buff[100];
    snprintf(buff, 100, "HTTP/1.1 200 OK\r\n\r\n");
	WRITE(from, buff, strlen(buff));

    

    return 0;
}


// fill in "login.index"
/*
int echo_post(header *hd, int cs){
    char content[1000];
    snprintf(content,100,"Welcome!\n");
    char *kv;

    while((kv = strsep(&hd->content, "&"))){
        strcat(content, "\n");
        strcat(content, kv);
    }

	char head[100];
	snprintf(head, 100, "HTTP/1.1 200 OK\r\nContent-Length: %lu\r\n\r\n", strlen(content));
	write(cs, head, strlen(head));
    write(cs, content, strlen(content));

    return 0;
}*/
