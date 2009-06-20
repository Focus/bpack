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


#ifndef _VERSION_H
#define _VERSION_H

#include <vector>
#include <cstring>
#include <sstream>
//#include <ofstream>

class version{
      public:
             version();
             version(const std::string);
             ~version();
             //Basic operators for comparison
             bool operator== (const version) const;
             bool operator< (const version) const;
             bool operator> (const version) const ;
             bool operator<= (const version) const ;
             bool operator>= (const version) const;
             //As strings
             bool operator== (const std::string) const;
             bool operator< (const std::string) const;
             bool operator> (const std::string) const;
             bool operator<= (const std::string) const;
             bool operator>= (const std::string) const;
             //Getting stuff
             std::string asString() const;
             std::vector<int> asVector() const {return numbers;}
             //Setting stuff
             void operator= (const std::string);
             void setVersion(std::string);
             void setVersion(std::vector<int> vec){numbers=vec;}
             
            
      private:
             std::vector<int> numbers; 
};

//IO operators
std::ostream& operator<< (std::ostream&, version&);
std::ofstream& operator<< (std::ofstream&, version&);
#endif
