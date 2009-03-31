#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "qhttp.h"


//TODO post, asyncronous download

// FIXME very picky about url format
struct HttpRequest buildreq(char* url)
{
    struct HttpRequest ret;

    if(strchr(url,':')==NULL){
        ret.port = 80;
        ret.host = malloc(strchr(url,'/')-url);
          strncpy(ret.host, url, strchr(url,'/')-url);
    }else{
        ret.host = malloc(strchr(url,':')-url);
          strncpy(ret.host, url, strchr(url,':')-url);
        int tp;
        char* tmp = malloc(strchr(url,'/')-strchr(url,':'));
          strncpy(tmp, strchr(url,':'), strchr(url,'/')-strchr(url,':'));
        if (1 != sscanf(tmp, "%d", &tp))
            printf( "qhttp : Error with specified port\n");               // flag something on error
        ret.port = tp;
    }
    ret.path = malloc(url+strlen(url)-strchr(url,'/'));
    strcpy(ret.path, strchr(url,'/'));
    return ret;
}

//FIXME both postdata's need to be escapified
//postdata can only be text
void addpostpair(struct HttpRequest req, char *key, char *val)
{
    char* newpost = malloc(strlen(key)+strlen(val)+1);
    sprintf(newpost, "%s=%s", key, val);
    printf(" Added post pair : %s\n", newpost);
    
    if(req.rawpost){
        char* curpost = req.rawpost;
        req.rawpost = malloc(strlen(curpost));
        strcpy(req.rawpost, curpost);
        req.rawpost[strlen(curpost)];
        strcpy(req.rawpost+strlen(curpost)+1, newpost);
    }else{
        req.rawpost = malloc(strlen(newpost));
        strcpy(req.rawpost, newpost);
    }
}

struct sockaddr_in *getsockaddr(struct HttpRequest req)
{
    struct hostent *ent;
    struct sockaddr_in *addr;

    printf("resolving host by name: %s\n", req.host);
    ent = gethostbyname (req.host);
    if ( ent ) {
        memcpy (&addr->sin_addr, ent->h_addr, ent->h_length);
        addr->sin_family = ent->h_addrtype;
        printf("resolved: %s (%s)\n", req.host, inet_ntoa(addr->sin_addr));
    } else {
        printf("failed to resolve locally.\n");
        return NULL;                          /* failed */
    }
    
    addr->sin_port = htons(req.port);

    return addr;
}

int sconnect(struct HttpRequest req)
{
    struct sockaddr_in saddr = *getsockaddr(req);
    int handle;

    printf("connecting to %s:%u\n", inet_ntoa(saddr.sin_addr), req.port);
    handle = socket( AF_INET, SOCK_STREAM, 0 );
    printf("socket %d opened\n", handle);
    if ( connect( handle, (struct sockaddr *)&saddr, sizeof(saddr))
         == -1) {
        printf("connect() failed.\n");
        return -1; 
    }
    printf("connected.\n");
    return handle;
}

char* rawrequest(struct HttpRequest req)
{
    char* method;
    char* buffer = malloc(strlen(req.host)+strlen(req.path)+50 + req.rawpost?strlen(req.rawpost):0);
    if (req.rawpost)
        req.method = POST;
    switch (req.method){
        case GET:
            method= "GET";
            break;
        case POST:
            method= "POST";
            break;
    }
    sprintf(buffer, "%s %s HTTP/1.1\r\nHost: %s\r\n\r\n", method, req.path, req.host);
    return buffer;
}

//TODO error handling and header extractions
struct HttpResponse buildresponse(char* rawresp,int length)
{
    struct HttpResponse resp;
    resp.raw=rawresp;

    printf("building response :\n%s\n\n", "");//rawresp);

    resp.streason = malloc(10);
    sscanf(rawresp, "HTTP/1.1 %d %s", &(resp.stcode), resp.streason);
    sscanf(strstr(rawresp, "Content-Length:"),"Content-Length: %d",&(resp.clength));
    

    resp.rawbody = malloc(resp.clength);
    strncpy(resp.rawbody, strstr(rawresp,"\r\n\r\n")+4, resp.clength);
    return resp;
}
    
// FIXME could have trouble sending large files
int httpsend(int socket, struct HttpRequest req)
{
    char* message = rawrequest(req);
    int p=0;
    printf("sending %s\n\n", message);
    /*while(p<strlen(message)){
        if(p+=send(socket,message+p,strlen(message)-p,0) < 0){
            printf(" error writing to socket\n");
            return 0;
        }
        //printf("p=%d, msglen=%d\n",p,strlen(message));
    }*/
    if(send(socket,message,strlen(message),0) < 0){
            printf(" error writing to socket\n");
            return 0;
        }
    printf("sent\n");
    return 1;
}

struct HttpResponse httpreadresponse(int socket, int close)
{//TODO should handle responses of arbitrary size
    int bufsize=1024;        /* a 1K buffer */
    char *buffer=malloc(bufsize);

    int bufferused = recv(socket, buffer, 1024, 0);     

    if (close)
        shutdown(socket, SHUT_RDWR);

    return buildresponse(buffer, bufferused);
}

// connects, sends request and returns response
struct HttpResponse HttpGet(struct HttpRequest req)
{
    int socket;
    if((socket=sconnect(req))>0 && httpsend(socket, req))
        return httpreadresponse(socket,1);
    else{
        struct HttpResponse error;
        error.stcode = 0; error.streason="ErrorSendingRequest";
        return error;
    }
}

// FIXME should accept dir or path and should use filename from response if provided
int wget(char* url, char* dir, char* filename)
{
    struct HttpRequest hq = buildreq(url);
    struct HttpResponse hr = HttpGet(hq);

    if (filename==NULL)
        filename = strrchr(url,'/');
    char* path = malloc(strlen(dir)+strlen(filename));
    strcpy(path,dir);
    strcat(path,filename);
    
    printf("saving to %s\n", path);
    FILE* f = fopen(path,"w");
    fwrite(hr.rawbody, 1, hr.clength, f);
    fclose(f);

    return 1;
}    
    



