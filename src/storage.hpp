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


#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <cstring>
#include <vector>

void macro(std::string&);
std::vector<std::string> com2vec(const std::string);
int separate(const std::string,std::vector<std::string> &,const int);
bool write (const std::string,const std::string,bool overwrite=0);
bool write (const std::vector<std::string>,const std::string,bool p3=0);
std::vector<std::string> read(const std::string);
void removeLine(std::string,std::string);
//If you want to track the cp call, pass it a vector of strings (note that it wont track the directories it makes)
int cp(std::vector<std::string>,std::string,std::vector<std::string> *arg=NULL);
int cp(std::string,std::string,std::vector<std::string> *arg=NULL);
#endif
