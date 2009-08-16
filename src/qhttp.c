/*
 * 		Copyright 2009 (c) Bpack- Bati Sengul <batieatworld@gmail.com>
 * 							      Mike Bush <mpbush@gmail.com>
 * 
 * 					http://www.bpack.co.uk
 * 
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */


#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include "qhttp.h"

#define MIN(x,y) x < y ? x : y

//pass an integer %
process(const char* name,float per){
	per=per/4;
	printf("%s : [",name);
	int i;
	for(i=0;i<per;i++)
		printf("#");
	for(i=per;i<25;i++)
		printf("-");

	printf("] %d%%\r",(int)per*4);
	fflush(stdout);
}

void freereq(struct HttpRequest *req){
	free(req->host);
	if(strcmp(req->path,"/"))
		free(req->path);
	free(req);
}

void freeresp(struct HttpResponse hr){
	if(hr.rawheader!=NULL)
		free(hr.rawheader);
	if(hr.streason!=NULL)
		free(hr.streason);
}

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
		if(!strncmp(url,"http",4))
			ret->protocol=HTTP;
		else if(!strncmp(url,"ftp",3))
			ret->protocol=FTP;
		else{
			ret->errormsg = "Unsupported protocol, cannot build request";
			return ret;
		}

		url+=strstr(url,"://")-url+3;
	} else
		ret->protocol = HTTP;	// assume http as default

	//printf(" : %\n", );

	// sort out host
	if(strchr(url,':')){
		ret->host = malloc(strchr(url,':')-url+1);
		strncpy(ret->host, url, strchr(url,':')-url);
		ret->host[strchr(url,'/')-url]='\0';
	}else if(strchr(url,'/')){
		ret->host = malloc(strchr(url,'/')-url+1);
		strncpy(ret->host, url, strchr(url,'/')-url);
		ret->host[strchr(url,'/')-url]='\0';
	}else{
		ret->host = malloc(strlen(url));
		strcpy(ret->host, url);
	}
	//printf(" : %\n", );


	// Sort out port
	if(strchr(url,':')==NULL){
		if(ret->protocol==HTTP)
			ret->port = 80;
		else
			ret->port=21;
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
	int sd;
	struct sockaddr_in pin;
	struct hostent *hp;
	if ((hp = gethostbyname(req.host)) == 0) {
		perror("gethostbyname");
		return -1;
	}

	memset(&pin, 0, sizeof(pin));
	pin.sin_family = AF_INET;
	pin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
	pin.sin_port = htons(req.port);
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return -1;
	}
	if (connect(sd,(struct sockaddr *)  &pin, sizeof(pin)) == -1) {
		perror("connect");
		return -1;
	}
	return sd;
}

//Simple connect function
int fconnect(char* ip,int port){
	int sd;
	struct sockaddr_in pin;
	struct hostent *hp;
	long addr=inet_addr(ip);
	if ((hp = gethostbyaddr((char*) &addr,sizeof(addr),AF_INET)) == NULL) {
		perror("gethostbyaddr");
		return -1;
	}

	memset(&pin, 0, sizeof(pin));
	pin.sin_family = AF_INET;
	pin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
	pin.sin_port = htons(port);
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return -1;
	}
	if (connect(sd,(struct sockaddr *)  &pin, sizeof(pin)) == -1) {
		perror("connect");
		return -1;
	}
	return sd;
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
	resp.rawheader=(char*)malloc(strlen(rawresp));
	strcpy(resp.rawheader, rawresp);

	//printf(" building response :\n%s\n\n", "");//rawresp);
	resp.streason = malloc(10);
	sscanf(rawresp, "HTTP/1.1 %d %s", &(resp.stcode), resp.streason);
	if(strstr(rawresp, "Content-Length:"))
		sscanf(strstr(rawresp, "Content-Length:"),"Content-Length: %d",&(resp.clength));
	else if(strstr(rawresp, "Content-length:"))
		sscanf(strstr(rawresp, "Content-length:"),"Content-length: %d",&(resp.clength));
	else if(strstr(rawresp, "content-length:"))
		sscanf(strstr(rawresp, "content-length:"),"content-length: %d",&(resp.clength));
	else{
		resp.clength = -1;
		resp.errormsg=malloc(strlen(("Cannot find content length")));
		resp.errormsg="Cannot find content length";
	}
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

//Simple function to send a message to the server
int ftpsend(int socket, char* command){

	char* message=(char*)malloc(strlen(command)+strlen(" \r\n"));
	sprintf(message,"%s\r\n",command);
	if(send(socket,message,strlen(message),0) < 0){
		printf("%s \n",strerror(errno));
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

	char* newbuffer=(char*)malloc(bufsize);
	bufferused = recv(socket, newbuffer, bufsize, 0);
	int i;


	free(buffer);
	struct HttpResponse ret = buildresponsehead(newbuffer);
	if(ret.clength<=0){
		if(strstr(newbuffer,"Location:")){
			char* redirect=strstr(newbuffer,"Location: ")+10;
			redirect[strlen(redirect)-strlen(strchr(redirect,'\r'))]='\0';
			ret.errormsg=malloc(strlen(redirect));
			strcpy(ret.errormsg,redirect);
			ret.clength=-10; //Return code for redirect
		}
	}
	ret.stream = socket;
	free(newbuffer);
	return ret;
}

//Pass the socket for profit
char* ftpreadresponse(int socket)
{
	int bufsize=5120;
	int i,n=-1;

	char* buffer=malloc(bufsize);
	if( recv(socket, buffer, bufsize, 0)==-1){
		perror("recv");
		exit(0);
	}
	if(buffer==NULL)
		return NULL;
	buffer[bufsize-1]='\n';
	//The server gives shit loads of stuff I don't want. Get rid of them all
	//if it is of the form 223-something, then it is bullshit, we want 223 something!
	for(i=0;i<strlen(buffer);i++){
		if(buffer[i]=='\n')
			n=i;
		if(buffer[i]==' ' && i-n==4)
			break;
	}
	char* ret;
	if(n!=bufsize-1){
		ret=malloc(bufsize-n-1);
		for(i=n+1;i<bufsize;i++){
			ret[i]=buffer[i];
			if(ret[i]=='\r'){
				ret[i]='\0';
				break;
			}
		}
	}else
		ret=strdup(buffer);
	free(buffer);
	return ret;
}

int ftpConvertAddy(char * buf, char * hostname, int * port) {
	unsigned int i,t=0;
	int flag=0,decCtr=0,tport1,tport2;
	char tmpPort[6];
	//example data in quotes below:
	//"227 Listening on (149,122,52,162,4,20)"
	//4 * 256 + 20 = 1044
	for(i=0;i<strlen(buf);i++) {
		if(buf[i]=='(') {
			flag = 1;
			i++;
		}
		if(buf[i]==')') {
			hostname[t]='\0';
			break;
		}
		if(flag==1) {
			if(buf[i] != ',') {
				hostname[t]=buf[i];
				t++;
			} else {
				hostname[t]='.';
				t++;
			}
		}
	}
	t=0;
	for(i=0;i<strlen(hostname);i++) {
		if(hostname[i]=='.')
			decCtr++;
		if(decCtr==4 && hostname[i]!='.') {
			tmpPort[t]=hostname[i];
			t++;
			if(hostname[i+1]=='.') {
				tmpPort[t]='\0';
				tport1=atoi(tmpPort);
				t=0;
			}
		}
		if(decCtr==5 && hostname[i]!='.') { 
			tmpPort[t]=hostname[i];
			t++;
			if(hostname[i+1]=='\0') {
				tmpPort[t]='\0';
				tport2=atoi(tmpPort);
				t=0;
			}
		}
	}
	*port = tport1 * 256 + tport2;
	decCtr=0;
	for(i=0;i<strlen(hostname);i++) {
		if(hostname[i]=='.') {
			decCtr++;
		}
		if(decCtr>3)
			hostname[i]='\0';
	}
	return 0;
}

struct HttpResponse ftpLogin(int socket){
	//char* greeting,user,pass,type;
	struct HttpResponse hr;
	char* greeting=ftpreadresponse(socket);
	if(strncmp(greeting,"2",1)){
		hr.errormsg=malloc(strlen(greeting));
		strcpy(hr.errormsg,greeting);
		free(greeting);
		return hr;
	}
	free(greeting);


	ftpsend(socket,"USER anonymous");
	sleep(1);
	char* user=ftpreadresponse(socket);
	if(user==NULL){
		//Do something...
	}
	else if(strncmp(user,"2",1)){
		hr.errormsg=malloc(strlen(user));
		strcpy(hr.errormsg,user);
		free(user);
		return hr;
	}
	if(user!=NULL)
		free(user);


	ftpsend(socket,"PASS anonymous");
	char* pass=ftpreadresponse(socket);
	if(pass!=NULL) //Don't think checking the pass is worth it
		free(pass);


	ftpsend(socket,"TYPE I");
	char* type=ftpreadresponse(socket);
	if(type==NULL){
		//Do something...
	}
	else if(strncmp(type,"2",1)){
		hr.errormsg=strdup(type);
		free(type);
		return hr;
	}
	if(type!=NULL)
		free(type);
	return hr;

}

int ftpGetFileSize(char* buffer){
	int size,code;
	sscanf(buffer,"%d %d",&code,&size);
	if(code!=213)
		return -1;
	return size;
}

// connects, sends request and returns response
// TODO : follow redirects
struct HttpResponse HttpGet(struct HttpRequest req, enum LOGMETHOD logtype)
{
	int socket=sconnect(req);
	if(req.protocol==HTTP){
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
	else{
		if(socket>0){
			char* ip;
			logger2(logtype, "connected to ", req.host);
			char* buffer;
			struct HttpResponse hr=ftpLogin(socket); //Does the Login to the ftp server
			if(hr.errormsg)
				return hr;

			//Grabs the size of the file we want
			char* retrmsg=malloc(strlen(req.path)+6);
			sprintf(retrmsg,"SIZE %s",req.path);
			ftpsend(socket,retrmsg);
			char* size=ftpreadresponse(socket);
			if( (hr.clength=ftpGetFileSize(size))==-1){
				hr.errormsg=malloc(strlen("Cannot obtain size!"));
				hr.errormsg="Cannot obtain size!";
				if(size!=NULL)
					free(size);
				return hr;
			}
			if(size!=NULL)
				free(size);


			/* Tell the server to be passive
			 * We will connect to the server as this is much better bet with firewalls and stuff
			 * Server sends back ip+port of the form (x1,x2,x3,x4,x5,x6)
			 * ip=x1.x2.x3.x4	port=x5*249+x6
			 */
			ftpsend(socket,"PASV");
			buffer=ftpreadresponse(socket);
			int port;
			char *host=buffer;
			ftpConvertAddy(buffer,host,&port);
			if(buffer!=NULL)
				free(buffer);

			//Connect to that ip, this is where the server will send the data through
			int filesocket=fconnect(host,port);
			if(filesocket==0){
				printf("Connecting to %s at %d failed\n",host,port);
				exit(0);
			}

			//Tell the server we want the file
			sprintf(retrmsg,"RETR %s",req.path);
			ftpsend(socket,retrmsg);
			free(retrmsg);
			//sleep(1);
			ftpreadresponse(socket);
			hr.stream=filesocket;
			return hr;

		}else{
			logger2(LOGMULTI, "not connected : error connecting to ", req.host);
			struct HttpResponse error;
			error.stcode = 0; error.streason =error.errormsg ="ErrorConnecting";
			return error;
		}


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

// Simple function to download and save a file like wget basic usage
// use filename = NULL to use the remote filename, see LOGMETHOD above
// returns 0 if download was succesful, else an error code or HTTP error 4xx / 5xx
// TODO : wont come up with a name for files if url has no filename
int wget(const char* url, const char* dir, const char* filename, enum LOGMETHOD logtype, int overwrite)
{
	// build request
	struct HttpRequest *hq = buildreq(url);
	if(hq->errormsg){
		logger2(LOGMULTI, "Error building request : ", hq->errormsg);
		return 2;
	}
	// get
	struct HttpResponse hr = HttpGet(*hq, logtype);
	if(hr.clength==-10){
		int ret=wget(hr.errormsg,dir,filename,logtype,overwrite);
		freereq(hq);
		freeresp(hr);
		return ret;
	}
	if(hr.errormsg){
		logger2(LOGMULTI, "Error getting : ", hr.errormsg);
		return 3;
	}


	// choose filename
	char tempname[100];
	if (filename==0){	// use given filename first
		if(hq->protocol==HTTP){
			if(getHeader(&hr, "Content-Disposition", tempname, 99)){
				// Get filename from response header
				filename = strchr(strstr(tempname, "filename"), '"')+1;
				*strchr(filename, '"') = '\0';
			}else 	// use filename from url
				filename = strrchr(url,'/')+1;
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
	FILE* f;
	if(overwrite==-1){
		f = fopen("/tmp/.bpack_pack","w");
		if(!f){
			logger(LOGMULTI, "Error opening /tmp/.bpack_pack");
			return 3;
		}
		remove("/tmp/.bpack_pack");//Tell the kernel we want this disposed when we are done
	}
	else{ 
		f = fopen(path,"w");
		if(!f){
			logger2(LOGMULTI, "Error opening file ", path);
			return 3;
		}
	}


	logger2(logtype, "saving to ", path);
	struct stat sta;
	int exists=stat(path, &sta);

	// TODO error handling and make nicer
	//If the content length is specified and overwrite is allowed or the file isn't there we just write to path
	if(hr.clength>0 && (overwrite==1 || exists!=0 ) ){
		int bufsize = ( hr.clength < 65536 ) ? hr.clength : 65536;   // 64KB max buffer size
		void *buffer = malloc(bufsize);
		int transremain = hr.clength;
		int readlength;
		float per;
		per=0;
		do{
			transremain -= (readlength = recv(hr.stream, buffer, bufsize,0));
			if(100-(float)transremain*100/(float)hr.clength-per>=4|| 100-(float)transremain*100/(float)hr.clength==100){
				process(filename,100-transremain*100/hr.clength);
				per=100-(float)transremain*100/(float)hr.clength;
			}
			fwrite(buffer, 1, readlength, f);

		}while (transremain > 0);
		printf("\n");
		free(buffer);
	}
	else if(hr.clength>0 && overwrite==-1){//TODO: finish this!
		int bufsize = ( hr.clength < 65536 ) ? hr.clength : 65536;   // 64KB max buffer size
		void *buffer = malloc(bufsize);
		int transremain = hr.clength;
		int readlength;
		float per;
		per=0;
		do{
			transremain -= (readlength = recv(hr.stream, buffer, bufsize,0));
			if(100-(float)transremain*100/(float)hr.clength-per>=4|| 100-(float)transremain*100/(float)hr.clength==100){
				process(filename,100-transremain*100/hr.clength);
				per=100-(float)transremain*100/(float)hr.clength;
			}
			fwrite(buffer, 1, readlength, f);

		}while (transremain > 0);
		printf("\n");
		free(buffer);
	}
	else if(hr.clength<=0){
		logger(LOGMULTI,"Error no content length!");
		remove(path);
		free(path);
		shutdown(hr.stream, SHUT_RDWR);
		fclose(f);
		logger(logtype, "saved");
		freeresp(hr);
		freereq(hq);
		return 3;
	}
	else{
		logger(LOGMULTI,"Error file already exists!");
		free(path);
		shutdown(hr.stream, SHUT_RDWR);
		fclose(f);
		logger(logtype, "saved");
		freeresp(hr);
		freereq(hq);
		return 3;
	}
	free(path);
	shutdown(hr.stream, SHUT_RDWR);
	fclose(f);
	logger(logtype, "saved");
	freeresp(hr);
	freereq(hq);
	return 0; // success
	//No way!!
}

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
