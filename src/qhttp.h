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


enum HTTPMETHOD{GET,POST};
// Chooses whether logging occurs (currently only to stdout) and whether it should be multiline
enum LOGMETHOD{LOGNONE,LOGMULTI,LOGSINGLE};
enum PROTOCOL{HTTP,FTP};
struct HttpRequest
{
    enum HTTPMETHOD method;
    unsigned short port;
    int socket;
    char *path;     // eg. /foo/bar.htm
    enum PROTOCOL protocol;// 
    char *host;     // eg. www.bpack.co.uk
    char *rawheader;// eachline terminated with /r/n, Host & Content-Length added automatically
    char *rawpost;  // FIXME: can only post text
    char *errormsg; // points to an error description if there is one
};


struct HttpResponse
{
    int stcode;     // Http response code
    char *streason; // Http reason code
    char *rawheader;// Raw http response header
    int clength;    // Content-Length
    int stream;
    char *errormsg; // points to an error description if there is one
};

	

// Simple function to download and save a file like wget basic usage
// use filename = NULL to use the remote filename, see LOGMETHOD above
// returns 0 if download was succesful, else an error code or HTTP error 4xx / 5xx
// Currently segfaults if no filename is provided in filename or url
//Overwrite values, 1 for overwriting, -1 for asking, 0 for no overwriting
// 0 will look at the differences of the two files and then tell the user
int wget(const char* sourceurl, const char* destdir, const char* filename, enum LOGMETHOD method, int overwrite );

// creates the default request to get the url
// on error the returned structure's errormsg field is set, otherwise NULL
struct HttpRequest* buildreq(const char* url);

// adds a key-value pair to be posted to a request
void addpostpair(struct HttpRequest *req, const char *key, const char *val);

// Adds a header to a request
//   header : string with no new line in the format "Content-Encoding: thing/blah"
void addheader(struct HttpRequest *req, const char* header);

// connects, sends request and return response
struct HttpResponse HttpGet(struct HttpRequest req, enum LOGMETHOD method);

// read the body of the response and return as string
char* getBody(struct HttpResponse*);

//Free all the mallocs using these
void freeresp(struct HttpResponse);
void freereq(struct HttpRequest*);
