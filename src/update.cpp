
#include "qhttp.h"
#include "update.hpp"

// This is not done
void update()
{
    struct HttpRequest *hq = buildreq("www.bpack.co.uk/repo/update.php", LOGMULTI);
    struct HttpResponse hr = HttpGet(*hq, LOGMULTI);

    addpostpair(hq, "gmp", "3.44");
    addpostpair(hq, "gcc", "9.3");
    addpostpair(hq, "bpack", "5");
    addpostpair(hq, "~all", "+");

    printf("%s\n\n", HttpGet(*hq, LOGSINGLE).rawbody);* /
}
