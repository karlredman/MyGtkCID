//modem.C

#include "modem.h"

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <stdio.h>

#include <poll.h>

#include <sys/types.h>


modem::modem(char *dev)
{
  test=1;
  device = dev;
  initString = "ATZ\r";
  resetString = "AT&F0\r";
}

modem::modem(char *dev, char *init, char *reset)
{
  //instatiate the modem and set it up

  test=1;
  device = dev;
  initString = init;
  resetString = reset;
}

modem::~modem()
{
  close_modem();
}

int
modem::open_modem()
{

  /* open the modem
     configure the modem for raw mode
  */
  
  struct termios options;

  int ret=0;


  /* open the port */
  if( (fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY)) <= 1) {
    return fd; }

  /* get the current descriptor options */
  if( ret = tcgetattr(fd, &options) == -1) 
    return ret;  

  //don't use memset on the entire structure -it's not entirely portable
  //memset(&options, 0, sizeof(options)); 

  //clear out the flags 
  options.c_iflag=0;		// input modes
  options.c_oflag=0;		// output modes
  options.c_cflag=0;		// control modes
  options.c_lflag=0;		// local modes
  memset(&options.c_cc, 0, sizeof(cc_t)); // control charecters

  /*  line disciplin (ok to use tcgetattr data here)
      options.c_line=options.c_line; 
  */


  //change settings
  //options.c_iflag |= (ICRNL | IGNPAR);
  options.c_iflag |= (INLCR | IGNBRK);
  options.c_oflag |= (ONOCR| OCRNL | HUPCL);


  if(ismodem())			// use CLOCAL non-modem controled line
    options.c_cflag |= (CS8 | CREAD);
  else
    options.c_cflag |= (CS8 | CLOCAL | CREAD);

  //setup for blocking at a read (non-cannonical mode)
  options.c_cc[VMIN] = 1;
  options.c_cc[VTIME] = 0;

  //setup baudrate
   cfsetospeed(&options, B9600);
   cfsetispeed(&options, B9600);
  
   //save settings
  if( (ret = tcsetattr(fd, TCSANOW, &options)) < 0) 
    return ret;

  tcsetattr(fd,TCSAFLUSH,&options);

  //turn off nonblock
  if( (ret = this->change_fl(fd, O_NONBLOCK, 0) < 0))
    return ret;

  //now turn off O_NDELAY (used for open)
  if( (ret = this->change_fl(fd, O_NDELAY, 0) < 0))
    return ret;

  // initialize modem
  ret = write_command(initString);

  return ret;
}
 
int
modem::close_modem()
{
  write_command(resetString);
  return (close(fd));
}

int
modem::write_command(char *command)
{
  return write_command(fd, command);
}

int
modem::write_command(string &command)
{
  return write_command(fd, command.c_str());
}

int
modem::write_command(int fd, const char *command)
{
  /* Initialize the modem -writes a string and waits for 'OK'.
     Function assumes that echo is ON.
  */

  char buffer[255];	/* modem output buffer */
  char *bufptr=buffer;	/* pointer to buffer */
  int nbytes;		/* num bytes read */
  int ret=0;		/* func. call ret. val. */
  int reading = 0;		// loop control

  int tbytes=0;

  if(strlen(command) <= 0){
    return -1; }

  
  memset(buffer, '\0', sizeof(buffer));
  //memcpy(buffer, command, strlen(command));
  //buffer[strlen(command)] = '\r';

  
  change_fl(fd, O_NONBLOCK, 1);

  //write the init string
  if( (ret = write(fd, command, strlen(command)) ) < 0)
    return ret;
  else {
    memcpy(buffer, command, strlen(command));
    buffer[strlen(command)] = '\0';
    buffer[strlen(command)-1] = '\0';
    cout << "write: wrote " << ret << "bytes" 
      "[" << buffer << "]" << endl;
  }

  change_fl(fd, O_NONBLOCK, 0);
  
  memset(buffer, '\0', sizeof(buffer));

  //try to read the response
  while ((nbytes = read(fd, buffer, sizeof(buffer))) > 0){

    response.erase();
    response = buffer;

    if(response.find("OK") != string::npos ||
       response.find("CONNECT") != string::npos) {
      break; 
    }
    else
      if(response.find("ERROR") != string::npos) { 
	return -1;
      }

    memset(buffer, '\0', sizeof(buffer));
  }
      
    memset(buffer, '\0', sizeof(buffer));

    //read last newlines
    //    nbytes = read(fd, buffer, 2);

  return 0;
}

int
modem::read_modem()
{
  char buffer[255];
  int ret;

  memset(buffer, '\0', sizeof(buffer));
  ret = read(fd, buffer, sizeof(buffer));
  response.erase();
  response = buffer;
  sleep(1);
  
  return ret;
}

int
modem::test_read_modem()
{
  char buffer[255];
  int ret;

  memset(buffer, '\0', sizeof(buffer));

  if(0)
    {

  if(test == 1) {
    strcpy(buffer, "RING\n"); 
    test++;
    sleep(1);
  }
  else 
    if(test == 2) {
      strcpy(buffer, "DATE=0926\n"); 
      test++;
      sleep(1);
    }
    else 
      if(test == 3) {
	strcpy(buffer, "TIME=0354\n");
	test++;
      sleep(1);
      }
      else 
	if(test == 4) {
	  strcpy(buffer, "NMBR=7735496129\n");
	  test++;
	  sleep(1);
	}
	else {
	  strcpy(buffer, "NAME=WEXENTHALLER RE\n");
	  test = 1;
	  sleep(1);
	}
    }
  else
    {
      strcpy(buffer, "\r\rDATE=0926\r\rTIME=0515\r\rNMBR=7735496129\r\rNAME=WEXENTHALLER RE\r\r\r\rRING\r\r\r\rRING\r\r");
	  sleep(1);
    }
  
  response.erase();
  response = buffer;
  

  return strlen(buffer);
}

int 
modem::change_fl(int fd, int flags, int setclear)
{
  /* set flag in file descriptor, 
     setclear (bool) used for setting or clearing flag

     return => 0 (SUCCESS)
     return < 0 (FAIL)
  */

  int val;

  if( (val = fcntl(fd, F_GETFL, 0) < 0) ) {
      return val;
  }
  
  if(setclear)
    val |= flags;		// trun on flags
  else
    val &= ~flags;		// turn off flags 

  if( (val = fcntl(fd, F_SETFL, val)) < 0) {
    return val;
  }
  else {
    val = fcntl(fd, F_GETFL, 0);
  }

  return val;
}

int
modem::ismodem()
{
  return 1;
}
