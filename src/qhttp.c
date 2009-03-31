#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "qhttp.h"


//TODO asyncronous download


void logger(enum LOGMETHOD logtype, char* msg)
{
    switch (logtype)
    {
        case LOGNONE:
            break;
        case LOGSINGLE:
            printf("qhttp : %s\r", msg);
            break;
        case LOGMULTI:
            printf("qhttp : %s\n", msg);
            break;
    }
}

void logger2(enum LOGMETHOD logtype, char* msg, char*msg2)
{
    switch (logtype)
    {
        case LOGNONE:
            break;
        case LOGSINGLE:
            printf("qhttp : %s%s\r", msg, msg2);
            break;
        case LOGMULTI:
            printf("qhttp : %s%s\n", msg, msg2);
            break;
    }
}

// FIXME should be able to take IP addresses
struct HttpRequest* buildreq(char* url, enum LOGMETHOD logtype)
{
    // ignore http:// if given
    if(strstr(url, "http://"))
        url+=7;
    // error if other protocol supplied
    else if(strstr(url,"://"))
        return NULL;

    struct HttpRequest *ret = (struct HttpRequest*)malloc(sizeof(struct HttpRequest));

    if(strchr(url,':')==NULL){
        ret->port = 80;
        ret->host = malloc(strchr(url,'/')-url+1);
          strncpy(ret->host, url, strchr(url,'/')-url);
    }else{
        ret->host = malloc(strchr(url,':')-url+1);
          strncpy(ret->host, url, strchr(url,':')-url);
        int tp;
        char* tmp = malloc(strchr(url,'/')-strchr(url,':')+1);
          strncpy(tmp, strchr(url,':'), strchr(url,'/')-strchr(url,':'));
        if (1 != sscanf(tmp, "%d", &tp)){
            logger2(logtype, "Error with specified port",tmp);               // flag something on error
            return NULL;
        }
        ret->port = tp;
    }
    ret->rawheader = "";
    ret->method = GET;
    ret->path = malloc(url+strlen(url)-strchr(url,'/')+1);
    strcpy(ret->path, strchr(url,'/'));
    return ret;
}


// Adds a header to a request
//   header : string with no new line in the format Content-Encoding: thing/blah
void addheader(struct HttpRequest *req, char* header)
{
    char* oldhead = req->rawheader;
    req->rawheader = malloc(strlen(oldhead)+strlen(header)+2);
    sprintf(req->rawheader, "%s%s\r\n", oldhead, header);
}

// Adds a key-value pair to the entity to be posted
void addpostpair(struct HttpRequest *req, char *key, char *val)
{
    char* newpost = malloc(strlen(key)+strlen(val)+2);
    sprintf(newpost, "%s=%s", key, val);
    //printf(" Added post pair : %s\n", newpost);
    
    if(req->rawpost){
        char* curpost = req->rawpost;
        req->rawpost = malloc(strlen(curpost)+strlen(newpost)+2);
        sprintf(req->rawpost, "%s&%s", curpost, newpost);
    }else{
        req->rawpost = malloc(strlen(newpost)+1);
        strcpy(req->rawpost, newpost);
    }
    if(strstr(req->rawheader, "Content-Type:")==NULL)
        addheader(req, "Content-Type: application/x-www-form-urlencoded");
}

// Resolves the address for the host named in the request
//   FIXME doesn't do IP addresses
struct sockaddr *getsockaddr(struct HttpRequest req)
{
    struct hostent *ent;
    struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));

    //printf(" resolving host by name: %s\n", req.host);
    ent = gethostbyname (req.host);
    if ( ent ) {
        printf("resolved: %s (%s)\n", req.host, inet_ntoa(*(struct in_addr*)(ent->h_addr_list[0])));
        memcpy (&addr->sin_addr, ent->h_addr_list[0], ent->h_length);
        addr->sin_family = ent->h_addrtype;
    } else {
        printf(" qhttp : failed to resolve locally.\n");
        return NULL;                          /* failed */
    }
    
    addr->sin_port = htons(req.port);

    return (struct sockaddr *)addr;
}

// connects to the host specified in the request
//   returns : handle to the socket
int sconnect(struct HttpRequest req)
{
    struct sockaddr *saddr = getsockaddr(req);
    if (!saddr)
        return 0;
    int handle;

    //printf("connecting to %s:%u\n", inet_ntoa(saddr.sin_addr), req.port);
    handle = socket( AF_INET, SOCK_STREAM, 0 );
    //printf("socket %d opened\n", handle);
    if ( connect( handle, saddr, sizeof(*saddr))
         <0) {
        printf(" qhttp : connect() failed.\n");
        return -1; 
    }
    //printf("connected on socket %d.\n", handle);
    return handle;
}


// Converts a HttpRequest into a raw string to be sent
// TODO can't send binarys
char* rawrequest(struct HttpRequest *req)
{
    char* method;
    int buffersize = strlen(req->host)+strlen(req->path)+50;
    char* buffer = malloc(buffersize);
    if (req->rawpost){
        req->method = POST;
        char* h = malloc(30);
        sprintf(h, "Content-Length: %d", strlen(req->rawpost));
        addheader(req, h);        
        buffersize += strlen(req->rawpost);
    }
    switch (req->method){
        case GET:
            method= "GET";
            break;
        case POST:
            method= "POST";
            break;
    }                
                //   TODO path & rawpost needs to be escapified
    sprintf(buffer, "%s %s HTTP/1.1\r\nHost: %s\r\n%s\r\n%s", method, req->path, req->host, req->rawheader, req->rawpost);
    return buffer;
}


struct HttpResponse buildresponsehead(const char* rawresp)
{
    struct HttpResponse resp;
    resp.rawheader=malloc(strlen(rawresp));
    strcpy(resp.rawheader, rawresp);

    printf(" building response :\n%s\n\n", "");//rawresp);

    resp.streason = malloc(10);
    sscanf(rawresp, "HTTP/1.1 %d %s", &(resp.stcode), resp.streason);
    if(strstr(rawresp, "Content-Length:"))
        sscanf(strstr(rawresp, "Content-Length:"),"Content-Length: %d",&(resp.clength));
    else
        resp.clength = -1;

    
    return resp;
}
    
// FIXME could have trouble sending large files
int httpsend(int socket, struct HttpRequest *req)
{
    char* message = rawrequest(req);
    int p=0;
    //printf("sending %s\n\n", message);
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
    //printf("sent\n");
    return 1;
}


struct HttpResponse httpreadresponse(int socket, enum LOGMETHOD logtype)
{
    int bufsize=4096;        /* a 4K buffer */
    char *buffer=malloc(bufsize);

    int bufferused = recv(socket, buffer, bufsize, MSG_PEEK);
    bufsize=strstr(buffer,"\r\n\r\n")+4-buffer;

    free(buffer);
    buffer=malloc(bufsize);
    bufferused = recv(socket, buffer, bufsize, 0);
    strstr(buffer, "\r\n\r\n")[0] = 0;


    printf("buffer : %s",buffer);
    struct HttpResponse ret = buildresponsehead(buffer);
    ret.stream = socket;
    return ret;
}

// connects, sends request and returns response
struct HttpResponse HttpGet(struct HttpRequest req, enum LOGMETHOD logtype)
{
    int socket=sconnect(req);
    if(socket>0){
        logger2(logtype, "connected to ", req.host);
        if (httpsend(socket, &req)){
            logger(logtype, "request sent, awaiting response");
            struct HttpResponse hr = httpreadresponse(socket, logtype);
            logger(logtype, "response retrieved");
            return hr;
        }else{
            logger(logtype, "not connected : error sending request");
            struct HttpResponse error;
            error.stcode = 0; error.streason="ErrorSendingRequest";
            return error;
        }
    }else{
        logger2(logtype, "not connected : error connecting to ", req.host);
        struct HttpResponse error;
        error.stcode = 0; error.streason="ErrorConnecting";
        return error;
    }
}

// TODO make wary of status codes, Transfer-Encoding, chunked transfers
int wget(char* url, char* dir, char* filename, enum LOGMETHOD logtype)
{
    struct HttpRequest hq = *buildreq(url, logtype);
    struct HttpResponse hr = HttpGet(hq, logtype);

    if (filename==NULL)
        filename = strrchr(url,'/');
    char* path = malloc(strlen(dir)+strlen(filename));
    strcpy(path,dir);
    strcat(path,filename);
    

    FILE* f = fopen(path,"w");    
    logger2(logtype, "saving to ", path);

    if(hr.clength>0){       // If content-length is specified, retrieve that many octets
        int bufsize = ( hr.clength < 65536 ) ? hr.clength : 65536;   // 64KB max buffer size
        void *buffer = malloc(bufsize);
        int transremain = hr.clength;
        int readlength;

        do{
            transremain -= readlength = read(hr.stream, buffer, bufsize);
            fwrite(buffer, 1, readlength, f);
        }while (transremain > 0);
        free(buffer);
    }



    shutdown(hr.stream, SHUT_RDWR);
    fclose(f);
    logger(logtype, "saved");
    
    return 1;
}

