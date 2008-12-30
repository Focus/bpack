#ifndef _VERSION_H
#define _VERSION_H

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


#endif
