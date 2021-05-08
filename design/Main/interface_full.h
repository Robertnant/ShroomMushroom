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
GtkWidget		*bubble_chat[1000]; 

//char *user = ; 

// int row2 = 0; //grid row counter (contact)  
// int row3 = 0; //grid row counter (user)

GtkButton *sendTextButton;
GtkButton *addContactButton;
GtkEntry *TextEntry;
GtkTextBuffer *send_textbuffer;
GtkLabel *textlabel;

FILE *f_con; //contacts.txt 
FILE *f_chat; //chat2 (contact)

// Functions.
void show_interface(char *interface_path, char *contacts_path, char *chat_path);
void chat_bubbles(char path[]);
void select_contact(GtkWidget * widget, gpointer arg);



#endif
