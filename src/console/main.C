//popup

#include <pthread.h>

#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "modem.h"		// the modem class
#include "cid_string.h"		// manages caller id type strings

using namespace std;

void loop();
//void loop_cleanup( pthread_mutex_t *mutex);
void fatal_error(int err_num, char *function);
int modem_ack(void *data);
//int newCall = 0;		// track a new cal

modem m("/dev/ttyS0");

//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t tid;		// pthread ID for cancelation


int main(int argc, char *argv[])
{
  int ret=0;			// reusable return value
  int ptimer;			// timer return value
  int ptimer2;			// timer return value

  //modem m("/dev/ttyS0");

  //ptimer = gtk_timeout_add(1000, popup, (void *)&m);
  //ptimer = gtk_timeout_add(1000, popup, NULL);

  //ptimer2 = gtk_timeout_add(10000, modem_ack, NULL);


  ///////////////////////////////////// MODEM INITIALIZATION 

  //open the modem
  cout << "opening modem" << endl;
  if ( (ret = m.open_modem()) == -1) {
    cout << "open_modem() FAILED\n" << endl;
    exit(1);
  }

  //set CID commands
  cout << "Initializing CID: ";
  if((ret = m.write_command("AT+VCID=1\r")) == -1) {
    cout << "FAILED" << endl;
    exit(1);
  }
  //else { 
    //cout << m.getResponse() << endl; }


  //////////////////////////////////// END MODEM INITIALIZATION



  /////////////////////////////////// BEGIN PTHREAD THING
  //start modem loop thread
  //ret = pthread_create(&tid, (pthread_attr_t *)NULL, 
		       //(void *(*)(void*))loop, (void *)NULL);

  //if(ret != 0)
    //fatal_error(ret, "pthread_create()");



  /////////////////////////////////// END PTHREAD THING

  loop();
  
  //program ending, join thread
  //ret = pthread_join(tid, (void **)NULL);

  //if(ret != 0)
    //fatal_error(ret, "pthread_join()");

  return 0;
}


int modem_ack(void *data)
{
  int ret;

  //"ping" the modem so it doesn't reset
  cout << "Checking modem responsiveness: ";
  if((ret = m.write_command("AT\r")) == -1) {
    cout << "FAILED" << endl;
    exit(1);
  }
  else { 
    cout << m.getResponse() << endl; }
  
  return true;
}

void loop()
{
  int nbytes;		/* num bytes read */
  int numCalls=0;
  string CID_output;
  cid_string name, number, date, time, tmp;
  char strCalls[20];

  //modem_ack(NULL);

  cout << "------------ Loop ------------------" << endl;

  //install a thread cancelation routine
  //pthread_cleanup_push((void (*)(void *))loop_cleanup, (void *)&mutex);

  /////////////////////////// debug
  //CID_output.erase();
//   CID_output = "Call #3\n";
//   CID_output += "NAME: Redman, Karl\n";
//   CID_output += "NMBR: 708-771-0517\n";
//   CID_output += "TIME: 10:22\n";
//   CID_output += "DATE: 10/03";
  
//   //newCall=1;
//   while(1)
//     {
//       popup((void *)CID_output.c_str());
//       cout << CID_output << endl;
//       sleep(10);
//     }
  /////////////////////////// end debug

  string::size_type idx = 0;

  while ((nbytes = m.read_modem()) > 0){
    //while (( nbytes = m.test_read_modem()) > 0){
    //cout<< "loop" << endl;

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

      numCalls++;

      memset(strCalls, '\0', sizeof(strCalls));
      sprintf(strCalls, "CALL #%d\n", numCalls);

      CID_output.erase();
      CID_output = strCalls;
      CID_output += "NAME: " + name + "\n";
      CID_output += "NMBR: " + number + "\n";
      CID_output += "TIME: " + time + "\n";
      CID_output += "DATE: " + date;

      name.erase();
      number.erase();
      date.erase();
      time.erase();

      //popup((void *)CID_output.c_str());
      //cout<< CID_output.c_str() << endl;
    }
    else {
      if(m.getResponse().find("RING") != string::npos){
	//just a new ring
	//cout << "got ring " << endl;
      }
      else{
	tmp = m.getResponse();
	tmp.chomp();
	tmp.getData();
	
	//cout << tmp.c_str() << endl;
	
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

  //cleanup and exit
  //pthread_cleanup_pop(0);
  pthread_exit(0);
}


void
fatal_error(int err_num, char *function)
{
        char    *err_string;

        err_string = strerror(err_num);
        fprintf(stderr, "%s error: %s\n", function, err_string);
        exit(-1);
}

//void loop_cleanup( pthread_mutex_t *mutex)
//{
//  pthread_mutex_unlock(mutex);
//}
