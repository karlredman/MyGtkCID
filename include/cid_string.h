//cid_string

/* String derived utility class for caller id*/

#ifndef MYCID_CID_STRING_H
#define MYCID_CID_STRING_H

#include <string>

class cid_string : public string
{
 public:

  /* constructor */
  cid_string(char *s):string(s){}
  cid_string():string(){}

  /* make equal to a string object */
  cid_string &operator=(const string &str){
    this->assign(str); 
    return *this;}
  
  /* trim \n and \r from end of string */
  void chomp();

  /* seperate 'xxx=' from data */
  void getData();
};
#endif MYCID_CID_STRING_H
