//popup

#include <pthread.h>

#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <gtk/gtk.h>

#include "modem.h"		// the modem class
#include "cid_string.h"		// manages caller id type strings

using namespace std;

void loop();
//void loop_cleanup( pthread_mutex_t *mutex);
void fatal_error(int err_num, char *function);

gint modem_ack(gpointer data);

gint destroyapp(GtkWidget *widget, gpointer gdata);
gint popup(gpointer data);

//int newCall = 0;		// track a new cal
modem m("/dev/ttyS0");

//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t tid;		// pthread ID for cancelation


int main(int argc, char *argv[])
{
  int ret=0;			// reusable return value
  GtkWidget *window;		// main window
  GtkWidget *close_button;	// close button in main window
  int ptimer;			// timer return value
  int ptimer2;			// timer return value

  //modem m("/dev/ttyS0");

  //initialize gtk
  gtk_init(&argc, &argv);



  /////////////////////////////////// BEGIN WINDOW INITIALIZATION

  //create a window
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);


  //signals
  gtk_signal_connect(GTK_OBJECT(window), "delete_event", 
		     GTK_SIGNAL_FUNC(destroyapp), NULL);
  
  
  //////////////////// WIDGETS:

  close_button = gtk_button_new_with_label("Close CID");
  gtk_signal_connect(GTK_OBJECT(close_button), "clicked",
		     GTK_SIGNAL_FUNC(destroyapp), NULL);
  //GTK_SIGNAL_FUNC(popup), NULL);

  //ptimer = gtk_timeout_add(1000, popup, (void *)&m);
  //ptimer = gtk_timeout_add(1000, popup, NULL);

  //ptimer2 = gtk_timeout_add(10000, modem_ack, NULL);

  //////////////////// PLACEMENT:
  gtk_container_add(GTK_CONTAINER(window), close_button);
  gtk_widget_show(close_button);
  



  //make main window visible
  gtk_widget_show(window);

  /////////////////////////////////// END WINDOW INITIALIZATION

  ///////////////////////////////////// MODEM INITIALIZATION 

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


  //////////////////////////////////// END MODEM INITIALIZATION



  /////////////////////////////////// BEGIN PTHREAD THING
  //start modem loop thread
  ret = pthread_create(&tid, (pthread_attr_t *)NULL, 
		       (void *(*)(void*))loop, (void *)NULL);

  if(ret != 0)
    fatal_error(ret, "pthread_create()");



  /////////////////////////////////// END PTHREAD THING

  /////////////////////////////////// BEGIN GTK LOOP
  //main event loop
  gtk_main();

  //program ending, join thread
  ret = pthread_join(tid, (void **)NULL);

  if(ret != 0)
    fatal_error(ret, "pthread_join()");

  return 0;
}

//////////////////////////// EVENT HANDLERS

gint destroyapp(GtkWidget *widget, gpointer gdata)
{
  //cancel the thread
  pthread_cancel(tid);

  //kill the app
  gtk_main_quit();

  //kill window too (FALSE)
  return(FALSE);
}


//////////////////////////// WIDGETS AND WINDOWS
gint popup(gpointer data)
{
  GtkWidget *dialog_widget, *label, *exit_btn;
  
  //if there isn't an new call, do nothing
  //if(!newCall) {
    //return 1;
  //}

  //pthread_mutex_lock(&mutex);
  //newCall=0;
  //pthread_mutex_unlock(&mutex);

  dialog_widget = gtk_dialog_new();
  label = gtk_label_new((gchar *)data);
  exit_btn=gtk_button_new_with_label("Close");

  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog_widget)->vbox), 
		     label, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog_widget)->action_area), 
		     exit_btn, FALSE, FALSE, 0);
			     

  gtk_widget_show(label);
  gtk_widget_show(exit_btn);

  gtk_signal_connect_object(GTK_OBJECT(exit_btn), "clicked",
			    GTK_SIGNAL_FUNC(gtk_widget_destroy), 
			    GTK_OBJECT(dialog_widget));

  gtk_widget_show(dialog_widget);

  return true;
}

gint modem_ack(gpointer data)
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
  cid_string name, number, date, time;
  char strCalls[20];

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

  while ((nbytes = m.read_modem()) > 0){

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

      memset(strCalls, '\0', sizeof(strCalls));
      sprintf(strCalls, "CALL #%d\n", numCalls);

      //print the strings and erase them
      //pthread_mutex_lock(&mutex);

      CID_output.erase();
      CID_output = strCalls;
      CID_output += "NAME: " + name + "\n";
      CID_output += "NMBR: " + number + "\n";
      CID_output += "TIME: " + time + "\n";
      CID_output += "DATE: " + date;

      //newCall=1;
      //pthread_mutex_unlock(&mutex);
	
      name.erase();
      number.erase();
      date.erase();
      time.erase();


      popup((void *)CID_output.c_str());
    }
  }

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
