//modem.C

#include "modem.h"

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <stdio.h>

#include <poll.h>

modem::modem(char *dev)
{
  
  device = dev;
  initString = "ATZ\r";
  resetString = "AT&F0\r";
}

modem::modem(char *dev, char *init, char *reset)
{
  //instatiate the modem and set it up

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
  //if( (fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY)) <= 1) {
    if( (fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK)) <= 1) {
  //if( (fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY)) <= 1) {
    return fd; }

  //set ndelay
  if( (ret = fcntl(fd, F_GETFL)) == -1) {
    return ret; } 

  //if( ret = fcntl(fd, F_SETFL, (ret & ~O_NDELAY)) == -1) {
    //return ret; } 
    

  /* get the current options */
  if( ret = tcgetattr(fd, &options) == -1) {
    return ret; } 

  memset(&options, 0, sizeof(options));
  /* set raw input, 1 second timeout */
  //options.c_cflag     |= (CLOCAL | CREAD);
  //options.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
  //options.c_oflag     &= ~OPOST;
  options.c_cc[VMIN]  = 0;
  options.c_cc[VTIME] = 10;

  options.c_iflag = IGNPAR | ICRNL;
  options.c_oflag = 0;
  options.c_cflag = CS8 | CREAD;
  options.c_lflag = ICANON;

  if( ret = tcflush(fd, TCIOFLUSH) == -1) {
    return ret; }

  //cfsetospeed(&options, B0);
  //cfsetispeed(&newtio, B0);

  //set the attributes -could fail with a return of success
  if( tcsetattr(fd,TCSANOW,&options) == -1) {
    return ret; }

  sleep(1);

  //cfsetospeed(&newtio, B38400);
  //fsetispeed(&newtio, B38400);

  tcsetattr(fd,TCSAFLUSH,&options);


  /* set the options */
  ret=tcsetattr(fd, TCSANOW, &options);

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

  //write the init string
  if(ret = write(fd, command, strlen(command)) == -1)
    return ret;
  
  //try to read the response
  while ((nbytes = read(fd, buffer, sizeof(buffer))) > 0){

    response = buffer;

    if(response.find("OK") != string::npos ||
       response.find("CONNECT") != string::npos ||
       response.find("ERROR") != string::npos)
      { break; }

    memset(buffer, '\0', sizeof(buffer));
  }
      
    //read the balance (newlines);
    nbytes = read(fd, bufptr, 2);

  return 0;
}

int
modem::read_modem()
{
  char buffer[255];
  int ret;

  memset(buffer, '\0', sizeof(buffer));
  ret = read(fd, buffer, sizeof(buffer));
  response = buffer;
  
  return ret;
}
