//modem.h

#include <string>

class modem
{
  int fd;			/* device descriptor */
  string device;		/* name of the device */
  string initString;		/* string for opening*/
  string resetString;		/* string for closing */
  string response;		/* response from write */

 public:

  modem(char *dev);
  modem(char *dev, char *init, char *reset);

  //destruct (calls close_modem)
  virtual ~modem();

  //open the modem for raw mode
  int open_modem();        

  //close modem
  int close_modem();

  //write a command to the modem and wait for 'OK'
  int write_command(int fd, const char *command);
  int write_command(string &command);
  int write_command(char *command);

  //just read the modem
  int read_modem();
  int ismodem();
  
  const string &getResponse() {return response;}		

  //helper functions
  int change_fl(int fd, int flags, int setclear);
  int test_read_modem();
  int test;
};
