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
#include <fstream>
#include <cstring>
#include <iostream>
#include <cstdlib>
using namespace std;
#include "version.hpp"
#include "package.hpp"
#include "storage.hpp"
#include "scan.hpp"
#include "config.hpp"
#include "storage.hpp"
#include "search.hpp"


//The locations
vector<string> locations;

//TODO
void versionSet(package &pack)
{
     /*vector<string> *locations=new vector<string>;
     string *name=new string;
     cout << "\n\nTrying to guess version...";
     *locations=pack.getLocations();
     *name=pack.getName();
	version *ver=new version;
	for(int i=0;i<locations->size();i++){
		(*locations).[i]
	}
	
	delete ver;
	delete name;
	delete locations;*/
}

//Seraches and makes a package from each name!
package getPack(const string pack)
{
	vector<string> *location=new vector<string>;
	string *current=new string;
      	package curpack;
      	curpack.setName(pack);
	for(int i=0;i<locations.size();i++){
		*current=search(locations[i],pack+"*");
		//If it is there add it to the list
		if(strcmp(current->c_str(),""))
			location->push_back(locations[i]+"/"+*current);
	}
	curpack.setLocations(*location);
      	versionSet(curpack);
	delete location;
	delete current;
      	return curpack;
}

//Scans the filesystem for packages
void scan()
{
	vector<string> packs;
	vector<package> found_pack;
	package temp;
	string path =Config::getPackInstDir();
	cout<<"\n\nScanning filesystem for packages, note that this may return obscure directories."<<endl;
	cout<<"Please manually edit before removing or updating any of these!!"<<endl;
	cout<<"Getting package names..."<<endl;
	packs=loadLocation(search(path,""));
	cout<<"Building a directory tree (this could take a while)..."<<endl;
	locations=loadLocation(dirTree("/usr"));
	cout<<"Searching for the packages..."<<endl;
	for(int i=0;i<packs.size();i++){
		temp=getPack(packs[i]);
		if((temp.getLocations()).size()>0)
			found_pack.push_back(temp);
	}
	printPackages(found_pack);
}

