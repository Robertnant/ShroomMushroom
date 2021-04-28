#ifndef REG_PAGE_H
#define REG_PAGE_H

#include <gtk/gtk.h>

struct registration_data
{
    int success;
    char username[17];
    char number[11];
};

typedef struct 
{
    int fd; //For communication with server
    GtkWidget *window;
    GtkButton *register_now_button;
    GtkEntry *username;
    GtkEntry *phonenumber;
    GtkLabel *error_label;
    struct registration_data * data;
    
} app_widgets;


void show_registration(struct registration_data * data);


#endif
