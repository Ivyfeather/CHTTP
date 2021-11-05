#include "common.hh"
#include "log.hh"

// get file size
int file_size(FILE *fp) {
	fseek(fp, 0, SEEK_END);
	int filelen = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return filelen;
}

int send_file(char *name, int cs){
	FILE *fp = NULL;
	// file not found
	if( (fp = fopen(name, "rb")) == NULL ){
		log(INFO, "file %s does not exist.\n", name);
		char *N404 = "HTTP/1.1 404 File not found\r\n\r\n";
		write(cs, N404, strlen(N404));
		return -1;
	}

	// file found, start transmission
	char head[100];
	snprintf(head, 100, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", file_size(fp));
	write(cs, head, strlen(head));	
	
	//   Body
	char buff[FILE_BUFF_LEN + 1];
	int ReadCount;
	while( (ReadCount = fread(buff, 1, FILE_BUFF_LEN, fp)) > 0 ) {
		write(cs, buff, ReadCount);
	}

	fclose(fp);

}