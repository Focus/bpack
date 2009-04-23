#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;
#include "version.hpp"
#include "packinst.hpp"
#include "package.hpp"
#include "installscript.hpp"
#include "storage.hpp"
#include "config.hpp"
#include "remove.hpp"
#include "search.hpp"

//Separates the foo-0.2.2 to foo and 0.2.2
void depVersion(string &dep, version &ver){
        int pos,temp;
        pos=dep.find_last_of("-");
        stringstream *ss= new stringstream;
        (*ss) << dep.substr(pos+1,pos+2);
        //If we have a - and the - isnt like hello-me
        if(pos>0 && !((*ss) >> temp).fail()){
                  ver=dep.substr(pos+1,dep.size());
                  dep=dep.substr(0,pos);
                  }
        delete ss;
}

//Extracts locations from the copy log
vector<string> stripCp(string name){
	vector<string> locs;
	ifstream *textfile=new ifstream;
	textfile->open("/tmp/bpack_log.txt");
	if(!(*textfile)){
		cerr<<"\nI can't find the install log!"<<endl;
		exit(1);
	}
	string *x=new string;
	while(!textfile->eof())
	{
		*textfile >> *x;
        	if(!textfile->eof())
		  locs.push_back(*x);
	}
	erase("/tmp/bpack_log.txt");
	delete x;
	delete textfile;
	return locs;
	
}

//Installs and cleans up packages
void clean(packinst pack){
	cout<<"\nAdding install paths..."<<endl;
	package *installed=new package;
	installed->setLocations(stripCp(pack.getName()));
	installed->setVersion(pack.getVersion());
	installed->setName(pack.getName());
	installed->write();
	delete installed;
	cout<<"Clearing up..."<<endl;
	erase(Config::getTarballDir()+pack.getName()+"/");
}


//Grabs the latest version
string greatestVer(vector<string> bushsucks){

	if(bushsucks.size()==0)
		return "";
	version tempver,highver;
	highver="0.0.0";
	string temp,ret;
	for(int i=0; i< bushsucks.size();i++){
		temp=bushsucks[i];
		depVersion(temp,tempver);
		if(tempver>highver)
			ret=bushsucks[i];
	}
	return ret;

}
//Recursively checks for dependencies and installs files (passes them to install script)
void install(string packname, const string configp="", const string makep="", const string makeinstp=""){

	packinst *packageinst=new packinst;
	version *ver=new version;
	string *location=new string;
	depVersion(packname,*ver);
	if(*ver=="0.0.0"){
		*location=greatestVer(loadLocation(search(Config::getPackInstDir(),packname+"*")));
	}
	else
		*location=search(Config::getPackInstDir(),packname+"-"+ver->asString());
	
	
	if(*location==""){
		cerr<<"Package "<<packname<<" not found, try after doing  bpack update."<<endl;
		exit(1);
	}	
	
	*packageinst=getPackage(Config::getPackInstDir()+*location);
	packageinst->setName(packname);
	packageinst->setVersion(*ver);

	vector<package> *installed=new vector<package>;
     	*installed=getInstalledPackages(Config::getPacklistPath().c_str());
     	string y;
	bool gotit;
	version depver;
	//check for dependencies
     	while(packageinst->getNextDep(y)){
        
		for(int i=0;i<installed->size();i++){
                	gotit=0;
                	depVersion(y,depver);
            		if(!strcmp(  ((*installed)[i].getName()).c_str(),y.c_str()))
            		{
             			if( (((*installed)[i].Version())>=depver)  || ( ((*installed)[i].Version())=="0.0.0") ){
                 			cout<<"\nPackage "<<y<<" is installed, removing from dependencies...";
					packageinst->removeDep(packageinst->getLoc());
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
      
     	cout<<"\nInstalling "<<packname<<"...\n";
     	//Have a packinst!
    	if(installScript(*packageinst)){
		clean(*packageinst);
		cout<<"\n"<<packageinst->getName()<<"-"<<packageinst->getVersion()<<" is installed!"<<endl;
	}
	else{
		cerr<<"\nError package not installed correctly. Consult the logs and inform us.\nWhat did you expect from an alpha build?"<<endl;
		exit(1);
	}

	
	delete installed;
	delete packageinst;
	delete ver;
	delete location;
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

//Makes a package and then installs it
void makePack(string loc,const string configp,const string makep,const string makeinstp){

	cout<<"\n\nMaking a package..."<<endl;
	int name=loc.find_last_of("/");
	if(name<=0){
		cerr<<"\n\n This doesn't look like a internet file to me!"<<endl;
	exit(1);
	}
	
	int truename=(loc.substr(name+1)).find_last_of(".tar");

	if(truename<=0){
		cerr<<"\n\nThis is not a tarball!!"<<endl;
		exit(1);
	}
	
	string fil;
	version ver;
	fil=(loc.substr(name+1)).substr(0,truename-4);
	
	depVersion(fil,ver);
	

	//Lets make the end bit now
	string out=loc;
	out=out+";\n"+configp+";\n"+makep+";\n"+makeinstp+";\n";
	string deps;
	
	cout<<"\n\nWhat are the dependencies? [Leave empty if you don't know/care] \nSeperate with a , e.g \t gcc,glibc,binutils"<<endl;

	cin>>deps;

	out=out+deps+";]\n\n";
	ofstream textfile(  (Config::getPackInstDir()+fil).c_str() ,ios::app);

	if(!textfile){
		cerr<<"\n\nI don't have rights to the package directory!"<<endl;
	exit(1);
	}

	textfile<<"[";
	textfile<<ver;
	textfile<<";\n";
	textfile<<out;
	textfile.close();

	install(fil);
}

//Look familiar
void internetinstall(char* argv[],const int argc){
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
                  makePack(name,configp,makep,makeinstp);
                  }
}


