//console

#include <stdio.h>
#include <errno.h>

#include <modem.h>		// the modem class
#include <cid_string.h>		// manages caller id type strings

using namespace std;		// using STL

int loop(modem &m);
void fatal_error(int err_num, char *function);
int modem_ack(void *data);


int main(int argc, char *argv[])
{
  int ret=0;			// reusable return value

  modem m("/dev/ttyS0", 0);	// modem object, non-cannonical


  ///////////////////////////////////// MODEM INITIALIZATION 
  //open the modem
  cout << "Initializing modem: " ;
  if ( (ret = m.open_modem()) == -1) {
    cout << "open_modem() FAILED\n" << endl;
    exit(1);
  }
  else
    cout << "[OK]" << endl;

  //set CID commands
  cout << "Initializing CID: ";
  if((ret = m.write_command("AT+VCID=1\r")) == -1) {
    cout << "Initialization FAILED" << endl;
    exit(1);
  }
  else
    cout << "[OK]" << endl;
  //////////////////////////////////// END MODEM INITIALIZATION

  //run man loop
  ret = loop(m);
  
  return ret;
}


int modem_ack(modem &m)
{
  // "ping" the modem so it doesn't reset 

  int ret;

  cout << "Checking modem responsiveness: ";
  if((ret = m.write_command("AT\r")) == -1) {
    cout << "FAILED" << endl;
    return false;
  }
  else { 
    cout << "[OK]" << endl; 
  }
  
  return true;
}

int loop(modem &m)
{
  int nbytes;			// num bytes read
  int numCalls=0;		// number of calls
  string CID_output;		// formatted output string

  cid_string name, 		// data name
    number, 			// data number
    date, 			// data date
    time, 			// data time
    tmp;			// tmp string

  string::size_type idx = 0;

  //modem_ack(NULL);

  cout << "------------ MyCID ------------------" << endl;

  //while (( nbytes = m.test_read_modem()) > 0){
  while ((nbytes = m.read_modem()) > 0){

    if( (idx = m.getResponse().find("NAME=", 0)) != string::npos){
      //just save the name
      name = m.getResponse().substr(idx);
      name.chomp();
      name.getData();
      //cout << "got name [" << name << "]" << endl;
    }

    if( (idx = m.getResponse().find("NMBR=", 0)) != string::npos){
      //format and save the number
      number = m.getResponse().substr(idx);
      number.chomp();
      number.getData();
      number.insert(3,"-");
      number.insert(7,"-");
      //cout << "got number " << endl;
    }

    if( (idx = m.getResponse().find("DATE=", 0)) != string::npos){
      //format and save the date
      date = m.getResponse().substr(idx);
      date.chomp();
      date.getData();
      date.insert(2, "/");
      //cout << "got date " << endl;
    }
    
    if( (idx = m.getResponse().find("TIME=", 0)) != string::npos){
      //format the time
      time = m.getResponse().substr(idx);
      time.chomp();
      time.getData();
      time.insert(2, ":");
      //cout << "got time " << endl;
    }
    
    if( !name.empty() && !number.empty() && !date.empty()
	&& !time.empty()){

      //print data 

      numCalls++;

      CID_output.erase();
      CID_output += "NAME: " + name + "\n";
      CID_output += "NMBR: " + number + "\n";
      CID_output += "TIME: " + time + "\n";
      CID_output += "DATE: " + date;

      name.erase();
      number.erase();
      date.erase();
      time.erase();

      //print output
      cout << "Call #" << numCalls << endl;
      cout << CID_output.c_str() << endl;
    }
    else {
      if(m.getResponse().find("RING") != string::npos){
	//just a new ring
	cout << "RING" << endl;
      }
      else{
	//something else came accross the line (try to display it)

	tmp = m.getResponse();
	tmp.chomp();
	tmp.getData();
	
	cout << "looped: errno = " << errno 
	     << ", nbytes = " << nbytes << ", "
	     << "modem response = " << "[" << tmp.c_str() << "]" 
	     << endl;
      }
    }
  }

  cout << "SHUTTING DOWN: errno = " << errno 
       << ", nbytes = " << nbytes << ", "
       << "Last modem response = \n" << "\t[" << m.getResponse() << "]" 
       << endl;

  return 0;
}


void
fatal_error(int err_num, char *function)
{
        char    *err_string;

        err_string = strerror(err_num);
        fprintf(stderr, "%s error: %s\n", function, err_string);
        exit(-1);
}
