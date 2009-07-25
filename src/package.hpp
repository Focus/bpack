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


#ifndef _PACKAGE_H
#define _PACKAGE_H

#include <vector>
#include <cstring>

#include "packinst.hpp"

class package
{

	public:
		package(){scan=0;}
		package(const std::string);
		package(const std::string , const std::vector<std::string>);
		package(const packinst inst);
		~package(){}
		std::string getName() const {return name;}
		std::string getVersion() const {return ver.asString();}
		version Version() const {return ver;}
		std::vector<std::string> getLocations() const{return locations;} 
		bool getScan() const {return scan;}
		void setName(const std::string pname){name=pname;}
		void setVersion(const std::string pname){ver=pname;}
		void setVersion(const version pver){ver=pver;}
		void setLocations(const std::vector<std::string> plocations){locations=plocations;}
		void addLocation(const std::string plocation){locations.push_back(plocation);}
		void addLocations(const std::vector<std::string> plocations){
			for(int i=0;i<plocations.size();i++)
				locations.push_back(plocations[i]);
		}
		void setScan(const bool pscan){scan=pscan;}
		bool save();
		bool remove();

	private:
		std::string name;
		version ver;
		std::vector<std::string> locations;
		bool scan;
};

std::vector<package> getInstalledPackages(const std::string);
std::vector<package> getInstalledPackages();
void printPackages();
package getInstalledPackage(std::string);
std::vector<package> getInstalledPackages();
void depVersion(std::string&, version&);
std::string packSize(std::vector<std::string>);
#endif



