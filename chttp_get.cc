#include "common.hh"
#include "log.hh"

// get file size
int file_size(FILE *fp) {
	fseek(fp, 0, SEEK_END);
	int filelen = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return filelen;
}

int send_file(char *name, void *net){
	FILE *fp = NULL;
	if(name[0]=='\0'){
		name = "file.html";
	}

#ifndef HTTPS
	int  to = *((int *)net);
#else
	SSL *to = (SSL *)net;
#endif

	// file not found
	if( (fp = fopen(name, "rb")) == NULL ){
		LOG(INFO, "file %s does not exist.\n", name);
		char *N404 = "HTTP/1.1 404 File not found\r\n\r\n";
		WRITE(to, N404, strlen(N404));
		return -1;
	}

	// file found, start transmission
	char head[100];
#ifndef CHUNKED
	snprintf(head, 100, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", file_size(fp));
	WRITE(to, head, strlen(head));	
	
	//   Body
	char buff[FILE_BUFF_LEN + 1];
	int ReadCount;
	while( (ReadCount = fread(buff, 1, FILE_BUFF_LEN, fp)) > 0 ) {
		WRITE(to, buff, ReadCount);
	}

#else
	snprintf(head, 100, "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
	WRITE(to, head, strlen(head));	
	
	//   Body
	char buff[FILE_BUFF_LEN + 53];
	char chunk_size[50];
	int ReadCount;
	while( (ReadCount = fread(buff+50, 1, FILE_BUFF_LEN, fp)) > 0 ) {
		// add chunk size(hex) to before the content
		snprintf(chunk_size, 50, "%x\r\n", ReadCount);
		int tmp = strlen(chunk_size);
		memcpy(buff+50-tmp, chunk_size, tmp);
		
		// add \r\n after the content
		char *end = "\r\n";
		memcpy(buff+50+ReadCount, end, 2);
		WRITE(to, buff+50-tmp, ReadCount+tmp+2);
	}
	// the last chunk, end transmission
	WRITE(to,"0\r\n\r\n",5);
#endif

	fclose(fp);
	return 0;
}