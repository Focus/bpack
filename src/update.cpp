
#include <vector>
#include <cstring>
#include "version.hpp"
#include "update.hpp"
extern "C"{
#include "qhttp.h"
}

// This is not done
void update()
{
    // Get all packs
    vector<string> packs=loadLocation(search(Config::getPackInstDir(),""));
    version vers[packs.size()];
    for (int i=0;i++;i<packs.size())
        depVersion(packs[i],vers[i]);

    struct HttpRequest *hq = buildreq("www.bpack.co.uk/repo/update.php", LOGMULTI);
    struct HttpResponse hr = HttpGet(*hq, LOGMULTI);

    addpostpair(hq, "gmp", "3.44");
    addpostpair(hq, "gcc", "9.3");
    addpostpair(hq, "bpack", "5");
    addpostpair(hq, "~all", "+");

    printf("%s\n\n", HttpGet(*hq, LOGSINGLE).rawbody);
}
