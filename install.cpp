
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;
#include "packinst.hpp"
#include "package.hpp"
#include "installscript.hpp"
#include "storage.hpp"
#include "config.hpp"


void install(const char* packname)
{
     string *packdir=new string;
     *packdir=packname;
     //IMPORTANT
     *packdir=Config::getPackInstDir()+*packdir;
     packinst *packinsty=new packinst;
     ifstream textfile;
     cout<<*packdir;
     textfile.open(packdir->c_str());
     delete packdir;
     if(!textfile)
     {
          cerr<<"\n\nPackage "<<packname<<" not found. Check the package name or try again after   bpack update \n\n";            
          exit(1);
     }
     cout<<"\nPackage "<<packname<<" to be installed.\n";
     string text;
     string x;
     while(textfile.good())
     {
          textfile >> x;
                if(!textfile.eof())          
                text +=x;
          
     }
     textfile.close();

     cout<<"\nChecking for dependencies....";
        cout.flush();


     vector<string> *instructs=new vector<string>;
     instructs->push_back("");
     instructs->resize(6);
     separate(text,*instructs,0);
     packinsty->setName(packname);
     packinsty->setVersion((*instructs)[0]);
     packinsty->setGit((*instructs)[1]);
     packinsty->setConfig((*instructs)[2]);
     packinsty->setMake((*instructs)[3]);
     packinsty->setMakeInst((*instructs)[4]);
     packinsty->setDeps(loadLocation((*instructs)[5]));
     
     vector<package> *installed=new vector<package>;
     *installed=getInstalledPackages(Config::getPacklistPath().c_str());
     string y;
        bool gotit;

     while(packinsty->getNextDep(y)){
        for(int i=0;i<installed->size();i++){
                gotit=0;
                //cout<<"\n"<<(*installed)[i].getName()<<endl;
            if(!strcmp(  ((*installed)[i].getName()).c_str(),y.c_str()))
            {
                 cout<<"\nPackage "<<y<<" is installed, removing from dependencies...";
                 cout.flush();

                 packinsty->removeDep(packinsty->getLoc());
                gotit=1;
                break;
                 
            }
            
        }
        if (!gotit)
            {
                cout<<"\nDependency "<<y<<" needs to be installed.";
                install(y.c_str()); 
            }
     }
        //delete gotit;
     cout<<"\nInstalling "<<packname<<"...\n";
     //Have a packinst!
     installScript(*packinsty);
     delete installed;
     delete packinsty;
     delete instructs;
     //delete text;
}



