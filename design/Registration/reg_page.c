#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "../../networking/client.h"
#include "reg_page.h"



void on_register_now_button_clicked(GtkWidget *widget, gpointer data)
{
	printf("register_now_button pressed\n");
	app_widgets *arg = data;

    arg->data->success = 0;

    gtk_label_set_text(arg->error_label, (const gchar*) "NO ERRORS");
        // Getting username into buffer
	//char buffer_for_username[17]; // changed to structure so no need for that
	strcpy(arg->data->username, gtk_entry_get_text(arg->username));
	
        // Verifying buffer length
    if (strlen(arg->data->username) < 6)
    {
        printf("Wrong length username\n");
        gtk_label_set_text(arg->error_label, (const gchar*) "Username should be between 6 and 16 characters long!");
        return;
	}

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
    user = init_procedure(sockfd, arg->data->username, arg->data->number);
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


void show_registration(struct registration_data* data)
{

    GtkBuilder      *builder_registration;
    GtkWidget       *window_registration;
    app_widgets     *widgets_registration = malloc(sizeof(app_widgets));
    

    builder_registration = gtk_builder_new_from_file("./design/Registration/reg_page.glade");
    window_registration = GTK_WIDGET(gtk_builder_get_object(builder_registration, "reg_page"));
    widgets_registration->register_now_button= GTK_BUTTON(gtk_builder_get_object(builder_registration, "register_now_button"));

    widgets_registration->window = window_registration;
    widgets_registration->phonenumber = GTK_ENTRY(gtk_builder_get_object(builder_registration, "number_textentry"));
    widgets_registration->username = GTK_ENTRY(gtk_builder_get_object(builder_registration, "username_textentry"));
    widgets_registration->error_label = GTK_LABEL(gtk_builder_get_object(builder_registration, "error_label"));
    widgets_registration->data = data;
    
    //widgets_registration->name_textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder_registration, "name_textbuffer"));
    //widgets->error_label = GTK_LABEL(gtk_builder_get_object(builder, "textlabel"));

    g_signal_connect(widgets_registration->register_now_button, "clicked", G_CALLBACK(on_register_now_button_clicked), (gpointer) widgets_registration);
    g_signal_connect(window_registration, "destroy", G_CALLBACK(destroy), (gpointer) widgets_registration);

    //gtk_widget_set_sensitive(GTK_WIDGET(widgets->TextEntry), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(widgets_registration->register_now_button), TRUE);
    g_object_unref(builder_registration);

    gtk_widget_show(window_registration);

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
