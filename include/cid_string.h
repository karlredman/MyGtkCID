//cid_string

/* String derived utility class */

#include <string>

class cid_string : public string
{
 public:

  cid_string(char *s):string(s){}
  cid_string():string(){}

  /*   make equal to a string */
  cid_string &operator=(const string &str){
    this->assign(str); 
    return *this;}
  

  //trim newlines from end of string
  void chomp();

  //seperate 'xxx=' from data
  void getData();
};
