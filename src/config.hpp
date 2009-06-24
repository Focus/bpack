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


#ifndef _CONFIG_H
#define _CONFIG_H


enum OPTDEP{ALL,NONE,RECOMMENDED};

class Config{

	private:
		static std::string installdir, scriptdir, packinstdir, tarballdir, packmandir, packlistpath, logdir,website,lib,coll,cflags,cxxflags;
		static int quitlevel;
		static enum OPTDEP optionaldep;

	public:

		// gets the directory containing bpack
		static std::string getInstallDir() {return installdir;}
		// gets the install script directory
		static std::string getScriptDir() {return scriptdir;}
		// gets the pack instruction directory
		static std::string getPackInstDir() {return packinstdir;}
		// gets the tarball directory
		static std::string getTarballDir() {return tarballdir;}
		// gets the directory used for temporary installation
		static std::string getPackmanDir() {return packmandir;}
		// gets log directory
		static std::string getLogDir() {return logdir;}
		// gets the path of the packlist
		static std::string getPacklistPath() {return packlistpath;}
		//Gets the lib directory
		static std::string getLib() {return lib;}
		//Gets the quit level
		static int getQuit() {return quitlevel;}
		//Gets flags
		static std::string getCflags(){return cflags;}
		static std::string getCxxflags(){return cxxflags;}
		//Gets the optional dependency configuration
		static enum OPTDEP getOptionalDep(){return optionaldep;}
		//		
		static std::string getColl(){return coll;}
		static void initialise();
};

#endif

