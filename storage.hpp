#ifndef _STORAGE_H
#define _STORAGE_H

vector<package> getInstalledPackages(const char* location);
//int loadPackage(const string text,string &name,string &version,string &locations,const int start);
vector<string> loadLocation(const string locations);
int separate(const string text,vector<string> &par,const int start);
void printPackages(const vector<package> packagelist);

#endif
