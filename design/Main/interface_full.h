#ifndef INTERFACE_H
#define INTERFACE_H

// Global variables.
GtkLabel *textlabel;

GtkBuilder      *builder;
GtkWidget       *main_window;
GtkWidget		*fixed1;
GtkWidget		*fixed2;
GtkWidget		*grid1; 
GtkWidget		*grid2; 
GtkWidget		*grid3; 

char			tmp[1024];
char			tmp_chat[1024]; 
GtkWidget		*button[1000];
GtkWidget		*button_chat[1000]; 

GtkButton *sendTextButton;
GtkEntry *TextEntry;
GtkTextBuffer *send_textbuffer;
GtkLabel *textlabel;

FILE *f_con; //contacts.txt 
FILE *f_chat; //chat2 (contact)

// Functions.
void show_interface();
void chat_bubbles();

#endif
