#include "common.hh"
#include "log.hh"

#define LEN 4000
#define TEST 0
 
int main(int argc, const char *argv[])
{
    int s, cs;
    struct sockaddr_in server, client;
    char msg[LEN];
     
    // create socket
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("create socket failed");
		return -1;
    }
    LOG(INFO, "socket created");
     
    // prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(80);
     
    // bind
    if (bind(s,(struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed");
        return -1;
    }
    LOG(INFO, "bind done");
     
    // listen
    listen(s, 3);
    LOG(INFO, "waiting for incoming connections...");
     
    // accept connection from an incoming client
    int c = sizeof(struct sockaddr_in);
    if ((cs = accept(s, (struct sockaddr *)&client, (socklen_t *)&c)) < 0) {
        perror("accept failed");
        return -1;
    }
    LOG(INFO, "connection accepted");
    
	// ========== HTTP SERVER ================
	int msg_len = 0;
    // receive a message from client
    while ((msg_len = recv(cs, msg, sizeof(msg), 0)) > 0) {
		LOG(DEBUG, "\n\"%s\"", msg);
		
        //!! CAUTION: FOR NOW ALL HTTP REQUEST MUST BE IN ONE PACKET
		header *hd = http_decoder(msg);
		int method = hd->method;

        // print_options(hd);

		if(method == GET){
			//!! temporarily removing the first '/' at url
			hd->url=hd->url+1;
			send_file(hd->url, cs);
		}else if(method == POST){
			echo_post(hd, cs);
		}else{
			LOG(WARNING, "Unknown HTTP Method.");
		}
		

        // free hd and options

		LOG(INFO, "Reciving HTTP request: %s:%d - %s",\
			inet_ntoa(client.sin_addr),ntohs(client.sin_port),\
            method_str[hd->method]);
    }
	// =======================================

    if (msg_len == 0) {
        LOG(INFO, "client disconnected");
    }
    else { // msg_len < 0
    	perror("recv failed");
		return -1;
    }
	
    return 0;
}

// HTTP/1.0 404 File not found\r\n\r\n
// HTTP/1.1 404 File not found\r\n\r\nx