//main.C

#include "modem.h"
#include "cid_string.h"

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include <iostream>

#include <gtk/gtk.h>

using namespace std;

void loop(modem &m);		     // loop and handle modem

//popup box funcs
//void print_and_quit(GtkButton *was_clicked, gpointer user_data);
void print_and_quit(GtkWidget *was_clicked, gpointer user_data);
gboolean delete_event_handler(GtkWidget *widget, GdkEvent *event, gpointer user_data);
gboolean doWin();

int main(int argc, char *argv[])
{
  int ret=0;	/* func. call. ret. val */
  modem m("/dev/ttyS0");

  gtk_init(&argc, &argv);

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
  //char buffer[255];	/* modem output buffer */
  int nbytes;		/* num bytes read */
  int numCalls=0;
  cid_string name, number, date, time;

  doWin();
  cout << "while loop" << endl;

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
  return;
}

static void sigHandler()
{
  //exit program (everything gets killed)
  cout << "Program terminated" << endl;
  exit(0);
}

// Callbacks

//void print_and_quit(GtkButton *was_clicked, gpointer user_data)
  void print_and_quit(GtkWidget *was_clicked, gpointer user_data)
{
  g_print("Button clicked\n");
  gtk_widget_destroy(was_clicked->parent);
  gtk_main_quit();
}

gboolean delete_event_handler(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
  g_print("Closing Dialog\n");

  //gtk_widget_destroy(widget->parent->parent->parent);
  //gtk_widget_destroy(widget->parent);
  //gtk_widget_destroy(widget->parent);
  gtk_widget_destroy(widget);
  return(FALSE);
}

gboolean doWin()
{
  GtkWidget *dialog_widget, *label, *exit_btn;

  dialog_widget = gtk_dialog_new();
  label = gtk_label_new("GTK+ is fun!");
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

  g_print("Bye!\n");

  return true;

  //gtk_window_set_title(GTK_WINDOW(top_widget), "CID");
  //gtk_container_set_border_width(GTK_CONTAINER(top_widget), 15);
  //gtk_widget_set_name(top_widget, "CID");
  //gtk_widget_set_name(box, "vertical box");
  //gtk_widget_set_name(label, "label");
  //gtk_widget_set_name(separator, "separator");
  //gtk_widget_set_name(exit_btn, "exit");


  //gtk_signal_connect(GTK_OBJECT(exit_btn), "clicked", 
		     //GTK_SIGNAL_FUNC(delete_event_handler), NULL);
		     //GTK_SIGNAL_FUNC(print_and_quit), NULL);

  //gtk_signal_connect(GTK_OBJECT(top_widget), "delete-event", 
		     //GTK_SIGNAL_FUNC(delete_event_handler), NULL); 
  //gtk_signal_connect(GTK_OBJECT(top_widget), "destroy", 
		     //GTK_SIGNAL_FUNC(print_and_quit), NULL); 
  
  //gtk_container_add(GTK_CONTAINER(top_widget), box);
  //gtk_box_pack_start_defaults(GTK_BOX(box), label);
  //gtk_box_pack_start_defaults(GTK_BOX(box), separator);
  //gtk_box_pack_start_defaults(GTK_BOX(box), exit_btn);

  //gtk_widget_show_all(top_widget);

  //gtk_main();
}
