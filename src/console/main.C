//main.C

#include "modem.h"
#include "cid_string.h"

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include <iostream>

using namespace std;

void loop(modem &m);		     // loop and handle modem
static void sigHandler(); 		     // handle signals

int main()
{
  int ret=0;	/* func. call. ret. val */
  modem m("/dev/ttyS0");

  //signal handler stuff
  signal(SIGINT , (void (*)(int))sigHandler);
  signal(SIGHUP , (void (*)(int))sigHandler);
  signal(SIGABRT, (void (*)(int))sigHandler);
  signal(SIGQUIT, (void (*)(int))sigHandler);
  signal(SIGTERM, (void (*)(int))sigHandler);

  //open the modem
  if (m.open_modem() == -1) {
    cout << "open_modem() FAILED\n" << endl;
    exit(1);
  }

  //set CID commands
  cout << "Initializing CID: ";
  if((ret = m.write_command("AT+VCID=1\r")) == -1) {
    cout << "FAILED" << endl;
    exit(1);
  }
  else { 
    cout << m.getResponse() << endl; }
    
  
  //enter loop
  cout << "---------------------LOOP-------------------\n";
  loop(m);

   return ret;
}

void loop(modem &m)
{
  char buffer[255];	/* modem output buffer */
  int nbytes;		/* num bytes read */
  int numCalls=0;
  cid_string name, number, date, time;

  while ((nbytes = m.read_modem()) > 0){

    sprintf(buffer, "%s", m.getResponse().c_str());
    cout << "just read modem...\n" << "[" << buffer << "]" << endl;

    if(m.getResponse().find("NAME=") != string::npos){
      //just save the name
      name = m.getResponse();
      name.chomp();
      name.getData();
      //cout << "got name " << endl;
    }
    else if(m.getResponse().find("NMBR=") != string::npos){
      //format and save the number
      number = m.getResponse();
      number.chomp();
      number.getData();
      number.insert(3,"-");
      number.insert(7,"-");
      //cout << "got number " << endl;
    }
    else if(m.getResponse().find("DATE=") != string::npos){
      //format and save the date
      date = m.getResponse();
      date.chomp();
      date.getData();
      date.insert(2, "/");
      //cout << "got date " << endl;
    }
    else if(m.getResponse().find("TIME=") != string::npos){
      //format the time
      time = m.getResponse();
      time.chomp();
      time.getData();
      time.insert(2, ":");
      //cout << "got time " << endl;
    }
    else if(m.getResponse().find("RING") != string::npos){
      //just a new ring
      //cout << "got ring " << endl;
    }

    if( !name.empty() && !number.empty() && !date.empty()
	&& !time.empty()){

      numCalls++;

      //print the strings and erase them
      cout << "------------------------------" << endl;
      cout << "CALL #" << numCalls << endl;
      cout << "NAME=" << name << endl;
      cout << "NUMBER=" << number << endl;
      cout << "TIME=" << time << endl;
      cout << "DATE=" << date << endl;
      cout << "------------------------------" << endl;
	
      name.erase();
      number.erase();
      date.erase();
      time.erase();
    }
  }
  
    sprintf(buffer, "exiting: response = %s, nbytes=%d", m.getResponse().c_str(), nbytes);
  cout << "[buffer]" << endl;
  return;
}

static void sigHandler()
{
  //exit program (everything gets killed)
  cout << "Program terminated" << endl;
  exit(0);
}
