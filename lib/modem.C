//modem.C

#include <modem.h>

#include <unistd.h>
#include <fcntl.h>
//#include <termios.h>
#include <sys/types.h>



modem::modem(char *dev, int canon)
{
  // use cannonical mode
  if(canon)
    cannonical = 1;
  else
    cannonical = 0;

  test=1;		       // test harnes variable
  device = dev;
  initString = "ATZ\r";
  resetString = "AT&F0\r";
}

modem::modem(char *dev, char *init, char *reset, int canon)
{
  /* instatiate the modem and set it up
   */

  // use cannonical mode
  if(canon)
    cannonical = 1;
  else
    cannonical = 0;

  device = dev;			// the terminal device name
  initString = init;		// the initialization string
  resetString = reset;		// the reset string (usially 'ATZ')
}

modem::~modem()
{
  // shutdown the modem
  close_modem();
}

int
modem::open_modem()
{

  /* open the modem and configure the modem for raw mode
   */
  
  struct termios options;	//termios structure for modem config 
  int ret=0;			// return values


  // open the port
  if( (fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY)) <= 1) {
    return fd; }

  /* get the current descriptor options */
  if( ret = tcgetattr(fd, &options) == -1) 
    return ret;  

  //save the old options internally
  oldOptions = options;

  /* Don't use memset on the entire structure -it's not entirely portable
     memset(&options, 0, sizeof(options)); 
  */

  //clear out the termios flags 
  options.c_iflag=0;		// input modes
  options.c_oflag=0;		// output modes
  options.c_cflag=0;		// control modes
  options.c_lflag=0;		// local modes
  memset(&options.c_cc, 0, sizeof(cc_t)); // control charecters


  if(!cannonical ){
    /*  line disciplin (ok to use tcgetattr data here usually)
	options.c_line=;		// (not used)
    */
    
    // change settings
    
    /* 
       options.c_iflag |= (IGNPAR); // (production programs don't use this) 
    */
    
    options.c_iflag |= (INLCR | IGNBRK);
    options.c_oflag |= (ONOCR| OCRNL | HUPCL);
    
    
    if(ismodem())	
      options.c_cflag |= (CS8 | CREAD); // modem line
    else
      options.c_cflag |= (CS8 | CLOCAL | CREAD); // non-modem line 
    
    //setup for blocking at a read (non-cannonical mode)
    options.c_cc[VMIN] = 1;	// at least 1 char
    options.c_cc[VTIME] = 0;
  }
    
    //setup baudrate (think... compatibility)
    cfsetospeed(&options, B9600);
    cfsetispeed(&options, B9600);
  
   //save settings
  if( (ret = tcsetattr(fd, TCSANOW, &options)) < 0) 
    return ret;

  //flush the line -for good measure
  tcsetattr(fd,TCSAFLUSH,&options);

  //turn off nonblock mode (to block at reads)
  if( (ret = this->change_fl(fd, O_NONBLOCK, 0) < 0))
    return ret;

  //now turn off O_NDELAY (antiquated but still necessary)
  if( (ret = this->change_fl(fd, O_NDELAY, 0) < 0))
    return ret;

  // initialize modem
  ret = write_command(initString);

  return ret;
}
 
int
modem::close_modem()
{
  //reset the modem
  write_command(resetString);

  //resotore options
  tcsetattr(fd, TCSANOW, &oldOptions);

  //flush the line
  tcsetattr(fd,TCSAFLUSH,&oldOptions);

  //close the modem descripter
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
  /* writes a string and waits for 'OK'.
     Function assumes that echo is ON.

     returns -1 on error.
  */

  char buffer[255];		// modem output buffer 
  int nbytes;			// num bytes read 
  int ret=0;			// func. call ret. val. 

  if(strlen(command) <= 0){
    return -1; }

  
  memset(buffer, '\0', sizeof(buffer));
  
  //turn on nonblock
  change_fl(fd, O_NONBLOCK, 1);

  //write the string
  if( (ret = write(fd, command, strlen(command)) ) < 0)
    return ret;

  //turn off nonblock
  change_fl(fd, O_NONBLOCK, 0);
  
  //set buffer with null chars
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

    //clear out buffer (interum)
    memset(buffer, '\0', sizeof(buffer));
  }
      
  return 0;
}

int
modem::read_modem()
{
  char buffer[255];
  int ret;

  //zero out buffer
  memset(buffer, '\0', sizeof(buffer));

  //read from modem (blockig expected)
  ret = read(fd, buffer, sizeof(buffer));
  
  //set response variable
  response.erase();
  response = buffer;


  if(cannonical){
    /* sleep(1): let data get to the modem
       This is here because we're using non-cannonical mode
       for a cannonical mode version of this program see cid-console2 of
       this project.
    */
    sleep(1);
  }

  //return number of bytes read
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

  //get descripter flags
  if( (val = fcntl(fd, F_GETFL, 0) < 0) ) {
      return val;
  }
  
  if(setclear)
    val |= flags;		// trun on flags
  else
    val &= ~flags;		// turn off flags 

  //set flags
  if( (val = fcntl(fd, F_SETFL, val)) < 0) {
    return val;
  }

  return -1;
}

int
modem::ismodem()
{
  /* This class expects that this is a modem.
     maybe something more technical for the future.
  */
  return 1;
}
