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


#ifndef _SEARCH_H
#define _SEARCH_H

//Searches the directory given by the 1st input for a file given by the 2nd (use * wildcard)
//If the second argument is not passed, it retuns all the things in the directory
//in this case, the return format is file1,file2, ... and so on (use the com2vec in storage
//to get it into a vector format)
std::string search(std::string, std::string name="");

//Makes a directory tree. It's pretty useless now but it may be of use later.
std::string dirTree(const std::string);

#endif

