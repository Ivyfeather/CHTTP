#include "common.hh"
#include "log.hh"

char *method_str[] = {"UNKNOWN", "GET", "POST"};

/* 
    -- split stringp at delim p, 
    -- return the first part and change stringp to the second part

    #include <string.h>
        char *strsep(char **stringp, const char *delim);
    
    Be cautious when using this function.  If you do use it, note that:
       * Delim should be a single char, not a string
       * This function modifies its first argument.     
       * This function cannot be used on constant strings.
       * The identity of the delimiting character is lost.

    For example, s="123_456_789", a=strsep(&s,"_"), 
        then a="123" and s="456_789"
*/
/*
    strcmp(s1,s2) returns 0 means s1==s2
*/

// return method from http header
header *http_decoder(char *str){
    header *hd = MALLOC(header);
    memset((void *)hd, 0, sizeof(header));
    
    // parse method
    char *method = strsep(&str, " ");
    if( strcmp(method, "GET") == 0){
        hd->method = GET;
    }else if( strcmp(method, "POST") == 0){
        hd->method = POST;
    }else{
        hd->method = UNKNOWN;
        LOG(WARNING, "Unsupported method %s", method);
        return NULL;
    }

    // parse url
    hd->url = strsep(&str, " ");

    // parse version
    if(strcmp(strsep(&str, "\n"), "HTTP/1.1\r") != 0){
        LOG(WARNING, "Not HTTP/1.1");
    }

    // parse options
    char *op_str;
    int found_content_length = 0;
    int found_keep_alive = 0;

    while((op_str = strsep(&str, "\n"))!=NULL && op_str[0]!='\r'){
        char *key = strsep(&op_str, ": ");
        char *val = op_str;
        val[strlen(val)-1]='\0';

        if(!found_content_length && strcmp(key, "Content-Length")==0){
            hd->content_length = atoi(val);
            found_content_length = 1;
        }
        if(!found_keep_alive && strcmp(key, "keep-alive")==0){
            hd->keep_alive = 1;
            found_keep_alive = 1;
        }

        if(found_keep_alive && found_content_length){
            break;
        }
    }

    // the rest is content
    hd->content = str;

    return hd;
}
