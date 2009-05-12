#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "qhttp.h"


//TODO asyncronous download

void logger(enum LOGMETHOD logtype, const char* msg)
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

void logger2(enum LOGMETHOD logtype, const char* msg, const char*msg2)
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


// creates the default request to get the url
// on error the returned structure's errormsg field is set, otherwise NULL

//TODO: I fixed the string issues here but it is very very very ugly!
struct HttpRequest* buildreq(char* url)
{
    struct HttpRequest *ret = (struct HttpRequest*)malloc(sizeof(struct HttpRequest));
	ret->errormsg = NULL;
	int pos;
	// Sort out protocol
	if(strstr(url,"://")){
		char* dd;
		dd=url+(strstr(url,"://")-url+3);
		pos=strlen(url)-strlen(dd)-3;
		char temp[pos];
		strncpy(temp,url,pos);
		ret->protocol = (char*)malloc(strlen(temp));
		strcpy(ret->protocol,temp);
		url=dd;
		
	} else
		ret->protocol = "http";	// assume http as default
		
	if(strcasecmp(ret->protocol, "http")!=0){
		ret->errormsg = "Unsupported protocol, cannot build request";
		return ret;
	}
	//printf(" : %\n", );
	
	// sort out host
	if(strchr(url,':')){
        ret->host = malloc(strchr(url,':')-url+1);
          strncpy(ret->host, url, strchr(url,':')-url);
    }else if(strchr(url,'/')){
	char* gg;
	gg=url+(strstr(url,"/")-url+1);
	pos=strlen(url)-strlen(gg)-1;
	char temp2[pos];
	strncpy(temp2,url,pos);
	temp2[pos]='\0';
        ret->host = (char*)malloc(strlen(temp2));
        strcpy(ret->host, temp2);
	}else{
        ret->host = malloc(strlen(url));
          strcpy(ret->host, url);
	}
	//printf(" : %\n", );
	

	// Sort out port
    if(strchr(url,':')==NULL){
        ret->port = 80;
    }else{
        int tp;	// FIXME work without '/'
        char* tmp;
		if(strchr(url, '/')){
			tmp = malloc(strchr(url,'/')-strchr(url,':')+1);
			  strncpy(tmp, strchr(url,':')+1, strchr(url,'/')-strchr(url,':')-1);
		}else
			tmp = strchr(url,':');
        if (1 != sscanf(tmp, "%d", &tp)){
            char *msg = (char*)malloc(strlen(tmp)+27);
            strcpy(msg, "Error with specified port: ");
            strcat(msg, tmp);
            ret->errormsg=msg;
            return ret;
        }
        ret->port = tp;
    }
	//printf(" : %\n", );
	
	
	// Sort out path
	
	if(strchr(url,'/')){
		ret->path = malloc(url+strlen(url)-strchr(url,'/')+1);
		strcpy(ret->path, strchr(url,'/'));
	}else
		ret->path = "/";
	
	//printf(" : %\n", );
	
	
	// Sort out general
    ret->rawheader = "";
    ret->method = GET;
    return ret;
}


// Adds a header to a request
//   header : string with no new line in the format Content-Encoding: thing/blah
void addheader(struct HttpRequest *req, const char* header)
{
    char* oldhead = req->rawheader;
    req->rawheader = malloc(strlen(oldhead)+strlen(header)+2);
    sprintf(req->rawheader, "%s%s\r\n", oldhead, header);
}

// Adds a key-value pair to the entity to be posted
void addpostpair(struct HttpRequest *req, const char *key, const char *val)
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
// returns NULL if it fails to find the host
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
        return NULL;                          /* failed */
    }
    
    addr->sin_port = htons(req.port);

    return (struct sockaddr *)addr;
}

// connects to the host specified in the request
//   returns : handle to the socket or 0 on failure
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
// TODO can't send binarys, generally only just conforms to http
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

// Turns the raw header text into a structure
struct HttpResponse buildresponsehead(const char* rawresp)
{
    struct HttpResponse resp;
	resp.errormsg = 0;
    resp.rawheader=malloc(strlen(rawresp));
    strcpy(resp.rawheader, rawresp);

    //printf(" building response :\n%s\n\n", "");//rawresp);

    resp.streason = malloc(10);
    sscanf(rawresp, "HTTP/1.1 %d %s", &(resp.stcode), resp.streason);
    if(strstr(rawresp, "Content-Length:"))
        sscanf(strstr(rawresp, "Content-Length:"),"Content-Length: %d",&(resp.clength));
    else
        resp.clength = -1;

    
    return resp;
}
    
// FIXME could have trouble sending large files
// returns 1 on success
int httpsend(int socket, struct HttpRequest *req)
{
    char* message = rawrequest(req);
    int p=0;
    if(send(socket,message,strlen(message),0) < 0){
            printf(" error writing to socket\n");
            return 0;
        }
    return 1;
}

// Reads the header from an arriving response
// TODO : error handling, is this valid http?, pretty much afaik
struct HttpResponse httpreadresponse(int socket)
{
    int bufsize=4096;        /* a 4K buffer */
    char *buffer=malloc(bufsize);

    int bufferused = recv(socket, buffer, bufsize, MSG_PEEK);
    bufsize=strstr(buffer,"\r\n\r\n")+4-buffer;

    free(buffer);
    buffer=malloc(bufsize);
    bufferused = recv(socket, buffer, bufsize, 0);
    strstr(buffer, "\r\n\r\n")[0] = 0;


    //printf("buffer : %s",buffer);
    struct HttpResponse ret = buildresponsehead(buffer);
    ret.stream = socket;
    return ret;
}

// connects, sends request and returns response
// TODO : follow redirects
struct HttpResponse HttpGet(struct HttpRequest req, enum LOGMETHOD logtype)
{
    int socket=sconnect(req);
    if(socket>0){
        logger2(logtype, "connected to ", req.host);
        if (httpsend(socket, &req)){
            logger(logtype, "request sent, awaiting response");
            struct HttpResponse hr = httpreadresponse(socket);
            logger(logtype, "response retrieved");
            return hr;
        }else{
            logger(LOGMULTI, "not connected : error sending request");
            struct HttpResponse error;
            error.stcode = 0; error.streason=error.errormsg ="ErrorSendingRequest";
            return error;
        }
    }else{
        logger2(LOGMULTI, "not connected : error connecting to ", req.host);
        struct HttpResponse error;
        error.stcode = 0; error.streason =error.errormsg ="ErrorConnecting";
        return error;
    }
}


// TODO make wary of status codes, Transfer-Encoding, chunked transfers, errors
// TODO should probably follow redirects

// Simple function to download and save a file like wget basic usage
// use filename = NULL to use the remote filename, see LOGMETHOD above
// returns 0 if download was succesful, else an error code or HTTP error 4xx / 5xx
// TODO : wont come up with a name for files if url has no filename
int wget(const char* url, const char* dir, const char* filename, enum LOGMETHOD logtype)
{
    if (filename==0)
        filename = strrchr(url,'/');
    char* path = malloc(strlen(dir)+strlen(filename)+2);
    strcpy(path,dir);
	strcat(path,"/");
    strcat(path,filename);
	path[strlen(dir)+strlen(filename)+1]=0;
    
    FILE* f = fopen(path,"w");
	if(!f){
		logger2(LOGMULTI, "Error opening file ", path);
		return 3;
	}
	
    struct HttpRequest hq = *buildreq(url);
	if(hq.errormsg){
		logger2(LOGMULTI, "Error building request : ", hq.errormsg);
		return 2;
	}
    struct HttpResponse hr = HttpGet(hq, logtype);
	if(hr.errormsg){
		logger2(LOGMULTI, "Error getting : ", hr.errormsg);
		return 3;
	}

    logger2(logtype, "saving to ", path);

	// TODO error handling and make nicer
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
    
    return 0; // success
}

