#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "../../networking/client.h"
#include "add_contact.h"

void on_add_contact_button_clicked(GtkWidget *widget, gpointer data)
{
	printf("register_now_button pressed\n");
	app_widgets *arg = data;

    arg->data->success = 0;

    gtk_label_set_text(arg->error_label, (const gchar*) "NO ERRORS");

        // Getting number into buffer
	// char buffer_for_phone[11]; // changed to structure so no need for that
	strcpy(arg->data->number, gtk_entry_get_text(arg->phonenumber)); //might need to free it 

        // Verifying buffer length
	if (strlen(arg->data->number) != 10)
    {
		printf("Wrong length\n");
        gtk_label_set_text(arg->error_label, (const gchar*) "Phone number should be 10 characters long!");
        return;
	}

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

    arg->data->success = 1;
    //user = init_procedure(sockfd, arg->data->username, arg->data->number);
    gtk_window_close(GTK_WINDOW(arg->window));
}

static void destroy(GtkWidget *widget, gpointer data)
{
	app_widgets *dest = data;
	free(data);
	//gtk_widget_destroy(widget);
	gtk_window_close((GTK_WINDOW(widget)));
	gtk_main_quit();
}


void main()
{
    gtk_init(NULL,NULL);

    GtkBuilder      *builder_add_contact;
    GtkWidget       *window_add_contact;
    app_widgets     *widgets_add_contact = malloc(sizeof(app_widgets));

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

}

/*
int main()
{
    gtk_init(NULL, NULL);

    struct registration_data* data = malloc(sizeof(struct registration_data));
    show_registration(data);
    
    gtk_main();
    //free(widgets_registration);

    return 0;
}
*/
