

enum HTTPMETHOD{GET,POST};

struct HttpRequest
{
    enum HTTPMETHOD method;
    unsigned short port;
    int socket;
    char *path;     // eg. /foo/bar.htm
    char *host;     // eg. www.bpack.co.uk
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

// connects, sends request and return response
struct HttpResponse HttpGet(struct HttpRequest req);

// creates the default request to get the url
struct HttpRequest buildreq(char* url);

// adds a key-value pair to be posted
void addpostpair(struct HttpRequest req, char *key, char *val);

// connects, sends request and returns response
struct HttpResponse HttpGet(struct HttpRequest req);

