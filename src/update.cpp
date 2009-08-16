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


#include <vector>
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <cstdlib>
#include "version.hpp"
#include "update.hpp"
#include "config.hpp"
#include "search.hpp"
#include "storage.hpp"
extern "C"{
#include "qhttp.h"
}
#include <map>

//TODO: check the current packages on update

using namespace std;

class collection
{
	private:
		map<std::string, std::string> packs;//What is this? please explain
		string url;
	public:
		collection(string url);
		void operator-=(vector<string> &rempacks);
		void operator+=(const collection &coll);
		void saveall(string path);
		void add(string);
		int empty();
};

collection::collection(string purl)
{
	url=purl;
	// build request for collection
	struct HttpRequest *req = buildreq(url.c_str());
	// add headers such as User-Agent and Accept
	// get
	struct HttpResponse resp = HttpGet(*req, LOGMULTI);
	freereq(req);
	// Error handling, on error none of the lower priority collections can be used, higher ones can however
	// Read list
	char* body = getBody(&resp);
	string respbody=body;
	free(body);
	freeresp(resp);
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

//Is the collection empty?
int collection::empty(){
	if(packs.size()>0)
		return 0;
	return 1;
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

void collection::add(string pack){


}

//returns 1 on success
void collection::saveall(string path)
{
	cout << "Downloading " << packs.size() << " packs\n";
	map<string,string>::iterator it;
	for ( it=packs.begin() ; it != packs.end(); it++ )
		wget(it->second.c_str(), path.c_str(), 0, LOGMULTI,1);

}
// This is not done
void update()
{
	// Get list of collections we are using
	string testurl("http://bpack.co.uk/repos.php?coll="+Config::getColl());
	
	// get current list of packs for each collection
	collection test(testurl);
	
	// Superimpose all packs
	collection all = test;
	
	// Get all local packs
	string packdir(Config::getPackInstDir());
    	vector<string> packs=com2vec(search(packdir,""));
	//cout << packs[0] << packs[1] << "\n";
	
	// remove all the packs we have from all, we dont need to download them
	all -= packs;
	
	// download and save all packs
	all.saveall(Config::getPackInstDir());

}

//Downloads a single package
int dlPack(string package){

	collection coll("http://bpack.co.uk/repos.php?coll="+Config::getColl());
	if(coll.empty())
		return 0;
	coll.saveall(Config::getPackInstDir());
	return 1;

}
