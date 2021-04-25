#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//ADD a text label which will display an error if the user puts in something wrong

GtkLabel *textlabel;

typedef struct 
{
    GtkButton *register_now_button;
    //GtkEntry *TextEntry;
    //GtkTextBuffer *name_textbuffer;
    //GtkLabel *textlabel;
    GtkEntry *username;
    GtkEntry *phonenumber;
    
} app_widgets;

app_widgets *curr;

void on_register_now_button_clicked(GtkWidget *widget, gpointer data)
{
	printf("register_now_button pressed\n");
	//char buffer[16];

	//make sure the name and number are words and numbers 
	//username max of 16 characters , at least 5 characters 
	//number 10 characters long and only digits
}

static void destroy(GtkWidget *widget, gpointer data){
	app_widgets *dest = data;
	free(data);
	//gtk_widget_destroy(widget);
	gtk_window_close((GTK_WINDOW(widget)));
	gtk_main_quit();
}

int main()
{
    GtkBuilder      *builder_registration;
    GtkWidget       *window_registration;
    app_widgets     *widgets_registration = malloc(sizeof(app_widgets));
    
    gtk_init(NULL, NULL);

    builder_registration = gtk_builder_new_from_file("./reg_page.glade");

    window_registration = GTK_WIDGET(gtk_builder_get_object(builder_registration, "reg_page"));

    widgets_registration->register_now_button= GTK_BUTTON(gtk_builder_get_object(builder_registration, "register_now_button"));

    //widgets->TextEntry = GTK_ENTRY(gtk_builder_get_object(builder, "TextEntry"));
    //widgets_registration->name_textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder_registration, "name_textbuffer"));
    //widgets->textlabel = GTK_LABEL(gtk_builder_get_object(builder, "textlabel"));

    g_signal_connect(widgets_registration->register_now_button, "clicked", G_CALLBACK(on_register_now_button_clicked), (gpointer) widgets_registration);
    g_signal_connect(window_registration, "destroy", G_CALLBACK(destroy), (gpointer) widgets_registration);

    //gtk_widget_set_sensitive(GTK_WIDGET(widgets->TextEntry), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(widgets_registration->register_now_button), TRUE);
    g_object_unref(builder_registration);

    gtk_widget_show(window_registration);                
    gtk_main();
    //free(widgets_registration);

    return 0;
}

