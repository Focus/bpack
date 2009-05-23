
#include <vector>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>
#include "version.hpp"
#include "update.hpp"
#include "config.hpp"
#include "search.hpp"
#include "storage.hpp"
extern "C"{
#include "qhttp.h"
}
#include <map>

using namespace std;

class collection
{
	private:
		map<std::string, std::string> packs;
	public:
		collection(string url);
		void operator-=(vector<string> &rempacks);
		void operator+=(const collection &coll);
		void saveall(string path);
};

collection::collection(string url)
{
	// build request for collection
	struct HttpRequest req = *buildreq(url.c_str());
	// add headers such as User-Agent and Accept
	// get
	struct HttpResponse resp = HttpGet(req, LOGMULTI);
	// Error handling, on error none of the lower priority collections can be used, higher ones can however
	// Read list
	string respbody(getBody(&resp));
	// error handle
	// check respbody is a valid collection listing
	
	//cout << respbody << "\n";
	stringstream coll(respbody, stringstream::in);
	string packroot;
	coll >> packroot;
	while(!coll.eof()){
		string id, name;
		coll >> id >> name;
		packs.insert(pair<string,string>(name, packroot+id));
		//cout << name << packroot+id << "\n";
	}
}

// removes from this collection any packs in the vector
void collection::operator-=(vector<string> &rempacks)
{
	vector<string>::iterator it;
	for ( it=rempacks.begin() ; it != rempacks.end(); it++ )
		packs.erase(*it);
}

// add any packs from provided collection, discard duplicates
void collection::operator+=(const collection &coll)
{
	
}

void collection::saveall(string path)
{
	cout << "Downloading " << packs.size() << " packs\n";
	map<string,string>::iterator it;
	for ( it=packs.begin() ; it != packs.end(); it++ )
		wget(it->second.c_str(), path.c_str(), 0, LOGMULTI);
}


// This is not done
void update()
{
	// Get list of collections we are using
	string testurl("http://bpack.co.uk/repos.php?coll=test");
	
	// get current list of packs for each collection
	collection test(testurl);
	
	// Superimpose all packs
	collection all = test;
	
    // Get all local packs
	string packdir(Config::getPackInstDir());
    vector<string> packs=loadLocation(search(packdir,""));
	//cout << packs[0] << packs[1] << "\n";
	
	// remove all the packs we have from all, we dont need to download them
	all -= packs;
	
	// download and save all packs
	all.saveall(Config::getPackInstDir());

}
