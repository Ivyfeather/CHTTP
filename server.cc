#include "common.hh"
#include "log.hh"
 
int main(int argc, const char *argv[])
{
    int s, cs;
    struct sockaddr_in server, client;
    char msg[SOCK_BUFF_LEN];
     
    // create socket
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("create socket failed");return -1;
    }
    LOG(INFO, "socket created");
     
    // prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
#ifndef HTTPS
    server.sin_port = htons(80);
#else
    server.sin_port = htons(443);

#define CACERT "./sslca/misc/ca.crt"
#define SCERT "./sslca/misc/ecs-net.crt"
#define SKEY  "./sslca/misc/ecs-net.key"

    SSL_CTX *ctx;
    SSL *ssl;
    
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    if ((ctx = SSL_CTX_new(SSLv23_server_method())) == NULL){
        perror("create ctx failed");return -1;
    }
    SSL_CTX_set_verify (ctx, SSL_VERIFY_PEER, NULL);
    SSL_CTX_load_verify_locations (ctx, CACERT, NULL);

    // load ca
    if(SSL_CTX_use_certificate_file(ctx, SCERT, SSL_FILETYPE_PEM) <= 0){
        perror("use cerificate file faled");return -1;
    }
    // load private key
    if (SSL_CTX_use_PrivateKey_file (ctx, SKEY, SSL_FILETYPE_PEM) <= 0){
        perror("use private key faled");return -1;
    }
    if (SSL_CTX_check_private_key (ctx) <= 0)
    {
        perror("private key does not match the certificate public key");
        return -1;
    }
    SSL_CTX_set_cipher_list (ctx, "RC4-MD5");
    SSL_CTX_set_mode (ctx, SSL_MODE_AUTO_RETRY);
#endif

    // bind
    if (bind(s,(struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed");return -1;
    }
    LOG(INFO, "bind done");
     
    // listen
    listen(s, 3);
    LOG(INFO, "waiting for incoming connections...");
     
    // accept connection from an incoming client
    int c = sizeof(struct sockaddr_in);
    if ((cs = accept(s, (struct sockaddr *)&client, (socklen_t *)&c)) < 0) {
        perror("accept failed");return -1;
    }
    close(s);
    LOG(INFO, "connection accepted");
    
	// ========== HTTP SERVER ================
#ifdef HTTPS	
    if((ssl = SSL_new(ctx)) == NULL){
        perror("create ssl failed");return -1;
    };
    LOG(INFO, "create ssl");
    if(!SSL_set_fd(ssl, cs)){
        perror("attach to cs failed");return -1;
    }
    if(!SSL_accept(ssl)){
        perror("ssl connect failed");return -1;
    }
    // varify peer cert
    /*
    X509 *client_cert = SSL_get_peer_certificate(ssl);
    if(!client_cert){
        perror("client auth error");return -1;
    }
    LOG(INFO, "Client Cert:");
    char *subject = X509_NAME_oneline(X509_get_subject_name(client_cert), 0, 0);
    char *issuer = X509_NAME_oneline (X509_get_issuer_name(client_cert), 0, 0);
    LOG(INFO, "subject: %s", subject);
    LOG(INFO, "issuer: %s", issuer);
    X509_free(client_cert);
    OPENSSL_free(subject);
    OPENSSL_free(issuer);
    */
   /*
    char *nohead =  "HTTP/1.1 200 OK\r\nContent-Length: 26\r\n\r\n";
    SSL_write(ssl, nohead, strlen(nohead));

    char *welcome = "HTTPS Server Ivy connected\n";
    SSL_write(ssl, welcome, strlen(welcome));
*/
#endif

    int msg_len = 0;
    header *hd = NULL;

    int https;
    void *net;
#ifndef HTTPS
    int tmp = cs;
    net = (void *)&cs;
    https = 1;
#else
    SSL *tmp = ssl;
	net = (void *)ssl;
    https = 0;
#endif

    // receive a message from client
    while ((msg_len = READ(tmp, msg, sizeof(msg))) > 0) {
		LOG(DEBUG, "\n\"%s\"", msg);
		
        //!! CAUTION: FOR NOW ALL HTTP REQUEST MUST BE IN ONE PACKET
		hd = http_decoder(msg);
		int method = hd->method;
        
		LOG(INFO, "Reciving %s request: %s:%d - %s",\
			(https)?"HTTPS":"HTTP", inet_ntoa(client.sin_addr),\
            ntohs(client.sin_port), method_str[hd->method]);

        //!! temporarily removing the first '/' at url
        hd->url=hd->url+1;

		if(method == GET){
			send_file(hd->url, net);
		}else if(method == POST){
            receive_file(hd, net);
        }
        // free hd
        free(hd);

    }
	// =======================================

    if (msg_len == 0) {
        LOG(INFO, "client disconnected");
    }
    else { // msg_len < 0
    	perror("recv failed");return -1;
    }
	
    return 0;
}

// HTTP/1.0 404 File not found\r\n\r\n
// HTTP/1.1 404 File not found\r\n\r\nx