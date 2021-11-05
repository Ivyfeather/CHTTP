#include "common.hh"
#include "log.hh"

#define LEN 4000
#define TEST 0
 
int main(int argc, const char *argv[])
{
    int s, cs;
    struct sockaddr_in server, client;
    char msg[LEN], filename[LEN];
     
    // create socket
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("create socket failed");
		return -1;
    }
    printf("socket created");
     
    // prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(80);
     
    // bind
    if (bind(s,(struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed");
        return -1;
    }
    printf("bind done");
     
    // listen
    listen(s, 3);
    printf("waiting for incoming connections...");
     
    // accept connection from an incoming client
    int c = sizeof(struct sockaddr_in);
    if ((cs = accept(s, (struct sockaddr *)&client, (socklen_t *)&c)) < 0) {
        perror("accept failed");
        return -1;
    }
    printf("connection accepted");
    
	// ========== HTTP SERVER ================
	int msg_len = 0;
    // receive a message from client
    while ((msg_len = recv(cs, msg, sizeof(msg), 0)) > 0) {
		printf("\n\"%s\"", msg);

		send_file("server.cc", cs);
    }
	// =======================================

    if (msg_len == 0) {
        printf("client disconnected");
    }
    else { // msg_len < 0
    	perror("recv failed");
		return -1;
    }
	
    return 0;
}

// HTTP/1.0 404 File not found\r\n\r\n
// HTTP/1.1 404 File not found\r\n\r\n

// get method and filename from HTTP message
// int server_decode(char *str, char *filename){
// 	char method[10], name[LEN], rest[LEN];
	
// #if TEST 
// 	printf("recv: %s",str);
// #endif
// 	// get method, severing at " "
// 	sever(str, method, rest, ' ');

// 	// GET method
// 	if( strcmp(method, "GET") == 0){
// 		sever(rest, name, rest, ' ');
// #if TEST 
// 		printf("name: %s\n",name + 1);
// #endif
// 		strcpy(filename, name + 1);// skip the first /
// 		return 0;
// 	}
	
// 	printf(" - Unknown Method\n");
// 	return -1;
// }
/*
int data_transmission(char *name, int cs){
	FILE *fp;
	if( (fp = fopen(name, "rb")) == NULL ){
		// 404 File Not Found

		printf(" - 404 File Not Found\n");

		char *N404 = "HTTP/1.1 404 File not found\r\n\r\n";
		write(cs, N404, strlen(N404));
		return -1;
	}

	printf(" - 200 OK\n");

	// send message
	//   Host
	char *OK = "HTTP/1.1 200 OK\r\n";
	write(cs, OK, strlen(OK));
	
	//   Head
	char head[100];
	snprintf(head, 100, "Content-Length: %d\r\n\r\n", file_size(fp));
	write(cs, head, strlen(head));

	//   Body
	char buff[LEN + 1];
	int ReadCount;
	while( (ReadCount = fread(buff, 1, LEN, fp)) > 0 ) {
		write(cs, buff, ReadCount);
#if TEST
		printf("ReadCount: %d\n",ReadCount);
		printf("%s", buff);
#endif
	}
#if TEST
	printf("Send Complete\n");
#endif
	fclose(fp);

	return 0;
}
*/
