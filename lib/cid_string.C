//cid_string.C

#include "cid_string.h"

void 
cid_string::chomp()
{
  // remove newlines and carraige returns from end of string
  string::size_type idx;

  idx = this->find('\r');
  if( idx != string::npos){
    this->erase(idx, this->length());
  }
  
  idx = this->find('\n');
  if( idx != string::npos){
    this->erase(idx, this->length());
  }

}

void 
cid_string::getData()
{
  // seperate 'LABEL=' from data (deleting LABEL=)

  string::size_type idx;
  
  idx = this->find('=');

  if( idx != string::npos){
    this->erase(0, idx+1);
  }
}
