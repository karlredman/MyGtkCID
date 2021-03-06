//cid_string.C

#include <cid_string.h>

void 
cid_string::chomp()
{
  /*  remove newlines and carraige returns from end of string
   */

  string::size_type idx;	// element position from find

  //check for and kill \r at end of sting
  idx = this->find('\r');
  if( idx != string::npos){
    this->erase(idx, this->length());
  }
  
  //check for and kill \n at end of sting
  idx = this->find('\n');
  if( idx != string::npos){
    this->erase(idx, this->length());
  }
}

void 
cid_string::getData()
{
  /* seperate 'LABEL=' from data (deleting LABEL=)

  sets cid_stirng::response to chars right of the '=' sign
   */

  string::size_type idx;
  
  idx = this->find('=');

  if( idx != string::npos){
    this->erase(0, idx+1);
  }
}
