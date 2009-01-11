#ifndef _STORAGE_H
#define _STORAGE_H

vector<package> getInstalledPackages(const char* location);
vector<string> loadLocation(const string locations);
int separate(const string text,vector<string> &par,const int start);
void printPackages(const vector<package> packagelist);

#endif
