#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include "qhttp.h"

#define MIN(x,y) x > y ? x : y


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

//Fixed again the strings, hehehe
struct HttpRequest* buildreq(const char* url)
{
    struct HttpRequest *ret = (struct HttpRequest*)malloc(sizeof(struct HttpRequest));
	ret->errormsg = NULL;
	ret->rawpost=NULL;
	int pos;
	// Sort out protocol
	if(strstr(url,"://")){
		ret->protocol = (char*)malloc(strstr(url,"://")-url);
		strncpy(ret->protocol, url, strstr(url,"://")-url);
		(ret->protocol)[strstr(url,"://")-url]='\0';
		url+=strstr(url,"://")-url+3;
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
		(ret->host)[strchr(url,'/')-url]='\0';
    	}else if(strchr(url,'/')){
		ret->host = malloc(strchr(url,'/')-url+1);
        	strncpy(ret->host, url, strchr(url,'/')-url);
		(ret->host)[strchr(url,'/')-url]='\0';
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
			free(tmp);
		}else
			tmp = strchr(url,':');
        if (1 != sscanf(tmp, "%d", &tp)){
            char *msg = (char*)malloc(strlen(tmp)+27);
            strcpy(msg, "Error with specified port: ");
            strcat(msg, tmp);
            ret->errormsg=msg;
            free(msg);
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
		req->rawpost[strlen(curpost)+strlen(newpost)+1]='\0';
    }else{
        req->rawpost = malloc(strlen(newpost)+1);
        strcpy(req->rawpost, newpost);
		req->rawpost[strlen(newpost)]='\0';
    }
    if(strstr(req->rawheader, "Content-Type:")==NULL)
        addheader(req, "Content-Type: application/x-www-form-urlencoded");
	free(newpost);
}

// connects to the host specified in the request
//   returns : handle to the socket or 0 on failure
int sconnect(struct HttpRequest req)
{
    //struct sockaddr *saddr = getsockaddr(req);
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(req.host, req.protocol, &hints, &res);

    int handle;

    //printf("connecting to %s:%u\n", inet_ntoa(saddr->sin_addr), req.port);
    handle = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    //printf("socket %d opened\n", handle);
    if ( connect(handle, res->ai_addr, res->ai_addrlen)
         <0) {
        logger2(LOGMULTI, "qhttp : connect() failed. Reason : ",strerror( errno ));
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
	free(h);
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
            logger(LOGMULTI, " error writing to socket\n");
            return 0;
        }
		free(message);
    return 1;
}

// Reads the header from an arriving response
// TODO : error handling, is this valid http?, pretty much afaik

struct HttpResponse httpreadresponse(int socket)
{
    int bufsize=4096;        /* a 4K buffer */
    char *buffer=(char*)malloc(bufsize);

    int bufferused = recv(socket, buffer, bufsize, MSG_PEEK);
    bufsize=strstr(buffer,"\r\n\r\n")+4-buffer;

    //free(buffer);
    char* newbuffer=(char*)malloc(bufsize);
    bufferused = recv(socket, newbuffer, bufsize, 0);
    strstr(newbuffer, "\r\n\r\n")[0] = 0;


    //printf("buffer : %s",buffer);
    struct HttpResponse ret = buildresponsehead(newbuffer);
    ret.stream = socket;
    free(buffer);
    free(newbuffer);
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
            logger2(logtype, "response retrieved : ", hr.streason);
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

int getHeader(struct HttpResponse *resp, char* key, char* value, int size)
{
	if(!strstr(resp->rawheader, key)) // does not contain this key
		return 0;
	char *start = strchr(strstr(resp->rawheader, key), ':')+1;
	return strncpy(value, start, MIN(size,strchr(start,'\r')-start) ) != 0;
}

// TODO make wary of status codes, Transfer-Encoding, chunked transfers, errors
// TODO should probably follow redirects

// Simple function to download and save a file like wget basic usage
// use filename = NULL to use the remote filename, see LOGMETHOD above
// returns 0 if download was succesful, else an error code or HTTP error 4xx / 5xx
// TODO : wont come up with a name for files if url has no filename
int wget(const char* url, const char* dir, const char* filename, enum LOGMETHOD logtype)
{
	// build request
    struct HttpRequest *hq = buildreq(url);
	if(hq->errormsg){
		logger2(LOGMULTI, "Error building request : ", hq->errormsg);
		return 2;
	}
	// get
    struct HttpResponse hr = HttpGet(*hq, logtype);
	if(hr.errormsg){
		logger2(LOGMULTI, "Error getting : ", hr.errormsg);
		return 3;
	}
	
	// choose filename
	char tempname[100];
	if (filename==0){	// use given filename first
		if(getHeader(&hr, "Content-Disposition", tempname, 99)){
			// Get filename from response header
			filename = strchr(strstr(tempname, "filename"), '"')+1;
			*strchr(filename, '"') = '\0';
		}else 	// use filename from url
			filename = strrchr(url,'/')+1;
	}
		
	// open file to save
	//printf("\n %s \n %s \n %s",url,dir,filename);
    char* path =(char*)malloc(strlen(dir)+strlen(filename)+2);
    strcpy(path,dir);
	//strcat(path,"/");
    strcat(path,filename);
	path[strlen(dir)+strlen(filename)]='\0';
    
    FILE* f = fopen(path,"w");
	if(!f){
		logger2(LOGMULTI, "Error opening file ", path);
		return 3;
	}
	

    logger2(logtype, "saving to ", path);
    free(path);
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
    free(hr.rawheader);
    free(hr.streason);

    free(hq->path);
    free(hq->protocol);
    free(hq->host);
    free(hq);
    return 0; // success
}

// Another malloc without a free, hopefully will find a better way to do this
char* getBody(struct HttpResponse *resp)
{
	int clength = resp->clength;
	/*char slength[10]; sprintf(slength, "%d", clength);
	logger2(LOGMULTI, "Reading body, length : ", slength);*/
	char *respbody = (char *)malloc(resp->clength+1);
	respbody[resp->clength]='\0';
	read(resp->stream, respbody, resp->clength);
	return respbody;
}
