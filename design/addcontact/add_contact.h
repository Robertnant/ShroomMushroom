#ifndef ADD_CONTACT_H
#define ADD_CONTACT_H

#include <gtk/gtk.h>

struct add_contact_data
{
    int success;
    char number[11];
};

typedef struct 
{
    int fd; //For communication with server
    GtkWidget *window;
    GtkButton *add_contact_button;
    GtkEntry *phonenumber;
    GtkLabel *error_label;
    struct add_contact_data * data;
    
} app_widgets_add_contacts;

void show_addContact();

app_widgets_add_contacts *widgets_add_contact;


//void show_registration(struct registration_data * data);


#endif
