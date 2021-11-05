#include "common.hh"
#include "log.hh"

/* 
    -- split stringp at delim p, 
    -- return the first part and change stringp to the second part

    #include <string.h>
        char *strsep(char **stringp, const char *delim);
    
    Be cautious when using this function.  If you do use it, note that:
       * This function modifies its first argument.     
       * This function cannot be used on constant strings.
       * The identity of the delimiting character is lost.
*/

// return method from http header
header *http_decoder(char *str){
    header *hd = (header *)malloc(sizeof(header));
    
    char *method = strsep(&str, " ");
    if( strcmp(method, "GET") == 0){
        hd->method = GET;
    }else if( strcmp(method, "POST") == 0){
        hd->method = POST;
    }else{
        hd->method = UNKNOWN;
        LOG(WARNING, "Unsupported method %s", method);
    }

    hd->url = strsep(&str, " ");

    return hd;
}
