#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../networking/client.h"
#include "add_contact.h"
#include "../Main/interface_full.h"
#define UNUSED(x) (void)(x)

GtkBuilder *main_builder;   // Builder of main interface.
//app_widgets_add_contacts     *widgets_add_contact;



// TODO: Implement mechanism to save new contact to txt file.

void on_add_contact_button_clicked(GtkWidget *widget, gpointer data)
{
    UNUSED(widget);
    printf("register_now_button pressed\n");
    app_widgets_add_contacts *arg = data;

    arg->data->success = 0;

    gtk_label_set_text(arg->error_label, (const gchar*) "NO ERRORS");

    // Getting number into buffer
	// char buffer_for_phone[11]; // changed to structure so no need for that
    strcpy(arg->data->number, gtk_entry_get_text(arg->phonenumber)); //TODO: might need to free it 
    printf("GOT PHONE NUM\n");
    // Verifying buffer length
    if (strlen(arg->data->number) != 10)
    {
        printf("Wrong length\n");
        gtk_label_set_text(arg->error_label, (const gchar*) "Phone number should be 10 characters long!");
        return;
    }
    printf("Len check\n");

    // Verifying format for number
    for (int i = 0; i < 10; i++)
    {
        if (!(arg->data->number[i] >= '0' && arg->data->number[i] <= '9'))
        {
            gtk_label_set_text(arg->error_label, (const gchar*) "Phone number should only contain digits!");
            printf("Wrong Format : Detected character %c\n", arg->data->number[i]);
            return;
        }
    }
    printf("Char check\n");
    if (!addContact(sockfd, arg->data->number))
    {
        gtk_label_set_text(arg->error_label, (const gchar*) "Woops! Who's that?");
        return;
    }
    printf("finised adding contact\n");

    struct user * new_user = get_contact(arg->data->number);
    
    if(!new_user)
    {
        gtk_label_set_text(arg->error_label, (const gchar*) "Woops! Who's that?");
        return;
    }

    // Add phone number to string for main interface.
    //char *newContact;
    //asprintf(&newContact, "NewUser-%s", arg->data->number);

    arg->data->success = 1;

    // Get main interface grid.
    GtkWidget *grid1 = GTK_WIDGET(gtk_builder_get_object(main_builder, "grid1"));
    int row = 0;
    // Display new conctact on list.
    gtk_grid_insert_row(GTK_GRID(grid1), row);
    GtkWidget *button = gtk_button_new_with_label(new_user->username);
    gtk_widget_set_name(button, new_user->number);
    g_signal_connect(button, "clicked", G_CALLBACK(select_contact), NULL);
    gtk_grid_attach (GTK_GRID(grid1), button, 1, row, 1, 1);
    gtk_widget_show_all(grid1);

    // Free memory.
    //free(newContact);

    //user = init_procedure(sockfd, arg->data->username, arg->data->number);
    gtk_window_close(GTK_WINDOW(arg->window));
    // gtk_widget_destroy(
}

static void destroy(GtkWidget *widget, gpointer data)
{
    UNUSED(widget);

    app_widgets_add_contacts *dest = data;
    UNUSED(dest);
    free(data);
    //gtk_widget_destroy(widget);
    // gtk_window_close((GTK_WINDOW(widget)));
    // gtk_main_quit();
}


void show_addContact(GtkBuilder *data)
{
    main_builder = data;

    // gtk_init(NULL,NULL);     // TODO: Uncomment if fails.

    GtkBuilder      *builder_add_contact;
    GtkWidget       *window_add_contact;
    widgets_add_contact = malloc(sizeof(app_widgets_add_contacts));

    struct add_contact_data *data1 = malloc(sizeof(struct add_contact_data));
    widgets_add_contact->data = data1;
    

    // builder_add_contact = gtk_builder_new_from_file("./add_contact.glade");
    builder_add_contact = gtk_builder_new_from_file("design/addcontact/add_contact.glade");
    window_add_contact = GTK_WIDGET(gtk_builder_get_object(builder_add_contact, "add_contact_page"));
    widgets_add_contact->add_contact_button= GTK_BUTTON(gtk_builder_get_object(builder_add_contact, "add_contact_button"));

    widgets_add_contact->window = window_add_contact;
    widgets_add_contact->phonenumber = GTK_ENTRY(gtk_builder_get_object(builder_add_contact, "number_textentry"));
    // widgets_registration->username = GTK_ENTRY(gtk_builder_get_object(builder_registration, "username_textentry"));
    widgets_add_contact->error_label = GTK_LABEL(gtk_builder_get_object(builder_add_contact, "error_label"));
    //widgets_registration->data = data;
    
    //widgets_registration->name_textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder_registration, "name_textbuffer"));
    //widgets->error_label = GTK_LABEL(gtk_builder_get_object(builder, "textlabel"));

    g_signal_connect(widgets_add_contact->add_contact_button, "clicked", G_CALLBACK(on_add_contact_button_clicked), (gpointer) widgets_add_contact);
    g_signal_connect(window_add_contact, "destroy", G_CALLBACK(destroy), (gpointer) widgets_add_contact);

    //gtk_widget_set_sensitive(GTK_WIDGET(widgets->TextEntry), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(widgets_add_contact->add_contact_button), TRUE);
    g_object_unref(builder_add_contact);

    gtk_widget_show(window_add_contact);
    // gtk_main();  // TODO: Uncomment if fails.
}

/*
int main()
{
    gtk_init(NULL,NULL);

    GtkBuilder      *builder_add_contact;
    GtkWidget       *window_add_contact;
    app_widgets_add_contacts     *widgets_add_contact = malloc(sizeof(app_widgets));

    struct add_contact_data *data1 = malloc(sizeof(struct add_contact_data));
    widgets_add_contact->data = data1;
    

    builder_add_contact = gtk_builder_new_from_file("./add_contact.glade");
    window_add_contact = GTK_WIDGET(gtk_builder_get_object(builder_add_contact, "add_contact_page"));
    widgets_add_contact->add_contact_button= GTK_BUTTON(gtk_builder_get_object(builder_add_contact, "add_contact_button"));

    widgets_add_contact->window = window_add_contact;
    widgets_add_contact->phonenumber = GTK_ENTRY(gtk_builder_get_object(builder_add_contact, "number_textentry"));
    // widgets_registration->username = GTK_ENTRY(gtk_builder_get_object(builder_registration, "username_textentry"));
    widgets_add_contact->error_label = GTK_LABEL(gtk_builder_get_object(builder_add_contact, "error_label"));
    //widgets_registration->data = data;
    
    //widgets_registration->name_textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder_registration, "name_textbuffer"));
    //widgets->error_label = GTK_LABEL(gtk_builder_get_object(builder, "textlabel"));

    g_signal_connect(widgets_add_contact->add_contact_button, "clicked", G_CALLBACK(on_add_contact_button_clicked), (gpointer) widgets_add_contact);
    g_signal_connect(window_add_contact, "destroy", G_CALLBACK(destroy), (gpointer) widgets_add_contact);

    //gtk_widget_set_sensitive(GTK_WIDGET(widgets->TextEntry), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(widgets_add_contact->add_contact_button), TRUE);
    g_object_unref(builder_add_contact);

    gtk_widget_show(window_add_contact);
    gtk_main();

    return 0;

}
*/
