//modem.h

#ifndef MYCID_MODEM_H
#define MYCID_MODEM_H

#include <string>

class modem
{
  int fd;			/* device descriptor */
  string device;		/* name of the device */
  string initString;		/* string for opening*/
  string resetString;		/* string for closing */
  string response;		/* response from write */

 public:

  /* constructors */
  modem(char *dev);
  modem(char *dev, char *init, char *reset);

  /* destruct (calls close_modem) */
  virtual ~modem();

  int open_modem();        	/* open modem in raw mode */

  int close_modem();		/* close modem and reset */

  /* write a command to the modem and wait for 'OK' */
  int write_command(int fd, const char *command);
  int write_command(string &command);
  int write_command(char *command);

  int read_modem();		/* just read the modem */

  int ismodem();		/* always returns true (for now)*/
  
  /* return the last modem character output */
  const string &getResponse() {return response;}		

  /* change a fioctl flag */
  int change_fl(int fd, int flags, int setclear);
  
  /* test harnes for CID */
  int test_read_modem();
  int test;			/* testing variable */
};

#endif MYCID_MODEM_H
