#ifndef REG_PAGE_H
#define REG_PAGE_H

#include <gtk/gtk.h>

struct registration_data
{
    int success;
    char username[17];
    char number[11];
    char avatar[10];
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

    GtkButton *lion_avatar_button;
    GtkButton *elephant_avatar_button;
    GtkButton *dog_avatar_button;
    GtkButton *koala_avatar_button;
    GtkButton *cat_avatar_button;
    
} app_widgets;


void show_registration(struct registration_data * data);


#endif
