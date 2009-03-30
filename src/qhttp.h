

enum HTTPMETHOD{GET,POST};

struct HttpRequest
{
    enum HTTPMETHOD method;
    unsigned short port;
    int socket;
    char *path;
    char *host;
};


struct HttpResponse
{
    int stcode;
    char *streason;
    char *rawheader;
    char *rawbody;
    char *raw;
    int length;
    int clength;
};

// downloads the specified file to the directory
int wget(char* url, char* dir, char* filename=NULL);

