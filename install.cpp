
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;
#include "version.hpp"
#include "packinst.hpp"
#include "package.hpp"
#include "installscript.hpp"
#include "storage.hpp"
#include "config.hpp"

//Separates the foo-0.2.2 to foo and 0.2.2
void depVersion(string &dep, version &ver){
        int pos;
        pos=dep.find("-",0);
        if(pos>0){
                  ver=dep.substr(pos+1,dep.size());
                  dep=dep.substr(0,pos);
                  }
        
}


//Recursively checks for dependencies and installs files (passes them to install script)
void install(string packname,const string configp="",const string makep="",const string makeinstp="")
{
     string *packdir=new string;
     version *ver=new version;
     depVersion(packname,*ver);
     *packdir=packname;
     
     //IMPORTANT
     *packdir=Config::getPackInstDir()+*packdir;
     packinst *packinsty=new packinst;
     ifstream textfile;
     textfile.open(packdir->c_str());
     delete packdir;
     if(!textfile)
     {
          cerr<<"\n\nPackage "<<packname<<" not found. Check the package name or try again after   bpack update \n\n";            
          exit(1);
     }
     if(*ver=="0.0.0")
     cout<<"\nPackage "<<packname<<" to be installed.\n";
     else
     cout<<"\nPackage "<<packname<<"-"<<ver<<" to be installed.\n";
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
     


     vector<string> *instructs=new vector<string>;
     instructs->push_back("");
     instructs->resize(6);
     int start=0;
     while((start+1<text.size())&&(!(*ver=="0.0.0"))){
     start=separate(text,*instructs,start);
     if(*ver==(*instructs)[0])
     break;
     
     }
     if(! (*ver==(*instructs)[0])){
          cout<<"\nI couldn't find "<<packname<<"-"<<(*ver)<<".\n\nI will install the latest version I have!\n";
          install(*packdir);
          exit(0);
          }
     
     //Construct package installation instructions
     packinsty->setName(packname);
     packinsty->setVersion((*instructs)[0]);
     packinsty->setGit((*instructs)[1]);
     
     //Check for passed parameters, if none, use default!
     if(strcmp(configp.c_str(),""))
     packinsty->setConfig(configp);
     else
     packinsty->setConfig((*instructs)[2]);
     
     if(strcmp(makep.c_str(),""))
     packinsty->setMake(makep);
     else
     packinsty->setMake((*instructs)[3]);
     
     if(strcmp(makeinstp.c_str(),""))
     packinsty->setMakeInst(makeinstp);
     else
     packinsty->setMakeInst((*instructs)[4]);
     
     packinsty->setDeps(loadLocation((*instructs)[5]));
     
     vector<package> *installed=new vector<package>;
     *installed=getInstalledPackages(Config::getPacklistPath().c_str());
     string y;
        bool gotit;
        version depver;
     while(packinsty->getNextDep(y)){
        for(int i=0;i<installed->size();i++){
                gotit=0;
                depVersion(y,depver);
            if(!strcmp(  ((*installed)[i].getName()).c_str(),y.c_str()))
            {
             if( (((*installed)[i].Version())>=depver)  || ( ((*installed)[i].Version())=="0.0.0") ){
                 cout<<"\nPackage "<<y<<" is installed, removing from dependencies...";
                 cout.flush();

                 packinsty->removeDep(packinsty->getLoc());
                gotit=1;
                break;
                }
                else{
                     cout<<"\nPackage "<<y<<" version "<<(*installed)[i].getVersion()<<" found but version "<<depver<<" required."<<endl;
                     }
            }
            
        }
        if (!gotit)
            {
                cout<<"\nDependency "<<y<<" needs to be installed.";
                install(y); 
            }
     }
        //delete gotit;
     cout<<"\nInstalling "<<packname<<"...\n";
     //Have a packinst!
     installScript(*packinsty);
     delete ver;
     delete installed;
     delete packinsty;
     delete instructs;
     //delete text;
}



//Catching parameters passed...
void preinstall(char* argv[],const int argc){
          vector<int> files;
          string configp,makep,makeinstp,name;
          version ver;
          if(argc==2){
                      cerr<<"\nNo packages passed to install!"<<endl;
                      exit(1);
                      }
          for(int i=2;i<argc;i++){
                  if(strncmp(argv[i],"--",2))
                  files.push_back(i);
          }
          for(int i=0;i<files.size();i++){
          configp="";
          makep="";
          makeinstp="";
          for(int j=files[i]+1;j<files[i+1];j++){
                  
                  if(!strncmp(argv[j],"--config=",9)){
                        configp=argv[j];
                        configp=configp.substr(10,configp.size()-10);
                        //cout<<configp;
                        }
                  if(!strncmp(argv[j],"--make=",8)){
                        makep=argv[j];
                        makep=makep.substr(9,makep.size()-9);
                        //cout<<makep;
                        }
                  if(!strncmp(argv[j],"--makeinst=",12)){
                        makeinstp=argv[j];
                        makeinstp=makeinstp.substr(13,makeinstp.size()-13);
                        //cout<<makeinstp;
                        }
                  }
                  name=argv[files[i]];
                  install(name,configp,makep,makeinstp);
                  }
}
