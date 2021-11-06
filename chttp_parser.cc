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
    }

    // parse url
    hd->url = strsep(&str, " ");

    // parse version
    if(strcmp(strsep(&str, "\n"), "HTTP/1.1\r") != 0){
        LOG(WARNING, "Not HTTP/1.1");
    }

    // parse options
    hd->options = MALLOC(option);
    hd->options->next = NULL;
    option *prev_op = hd->options; 
    char *op_str;
    while((op_str = strsep(&str, "\n"))!=NULL && op_str[0]!='\r'){
        option *new_op = MALLOC(option);
        prev_op->next = new_op;    
    
        new_op->key = strsep(&op_str, ": ");
        new_op->value = op_str;
        new_op->next = NULL;
        prev_op = new_op;
    }

    hd->content = str;

    return hd;
}

void print_options(header *hd){
    for(option *it=hd->options->next; it!=NULL; it=it->next){
        printf("%s: %s\n", it->key, it->value);
    }
}