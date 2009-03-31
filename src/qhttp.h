

enum HTTPMETHOD{GET,POST};

struct HttpRequest
{
    enum HTTPMETHOD method;
    unsigned short port;
    int socket;
    char *path;     // eg. /foo/bar.htm
    char *host;     // eg. www.bpack.co.uk
    char *rawheader;// eachline terminated with /r/n, Host & Content-Length added automatically
    char *rawpost;  // can only post text
};


struct HttpResponse
{
    int stcode;     // Http response code
    char *streason; // Http reason code
    char *rawheader;// Raw http response header
    char *rawbody;
    char *raw;      // Entire response raw
    int length;     // Length of raw response
    int clength;    // Content-Length
};

// downloads the specified file to the directory, use filename = NULL to use the remote filename
int wget(char* url, char* dir, char* filename);

// creates the default request to get the url
struct HttpRequest* buildreq(char* url);

// adds a key-value pair to be posted
void addpostpair(struct HttpRequest *req, char *key, char *val);

// Adds a header to a request
//   header : string with no new line in the format Content-Encoding: thing/blah
void addheader(struct HttpRequest *req, char* header);

// connects, sends request and return response
struct HttpResponse HttpGet(struct HttpRequest req);


