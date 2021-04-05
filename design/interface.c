#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GtkLabel *textlabel;

typedef struct 
{
    GtkButton *sendTextButton;
    GtkEntry *TextEntry;
    GtkTextBuffer *send_textbuffer;
    GtkLabel *textlabel;
    
} app_widgets;

app_widgets *curr;

void on_send_text_button_activate(GtkWidget *widget, gpointer data)
{
	char tmp[128];
	app_widgets *widgets = (app_widgets*) data;
  	sprintf(tmp, "%s", gtk_entry_get_text(widgets->TextEntry));
  	gtk_label_set_text(widgets->textlabel, (const gchar*) tmp);

  	if (widget){
  		printf(" ");
  	}
}


void on_TextEntry_changed(GtkWidget *widget, gpointer data)
{
	char tmp[128];
	app_widgets *widgets = (app_widgets*) data;
  	sprintf(tmp, "%s", gtk_entry_get_text(widgets->TextEntry));
  	gtk_label_set_text(widgets->textlabel, (const gchar*) tmp);

  	if (widget){
  		printf(" ");
  	}
}

int main()
{
    GtkBuilder      *builder;
    GtkWidget       *window;
    app_widgets     *widgets = malloc(sizeof(app_widgets));
    
    gtk_init(NULL, NULL);

    builder = gtk_builder_new_from_file("./interface.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

    widgets->sendTextButton = GTK_BUTTON(gtk_builder_get_object(builder, "sendTextButton"));
    widgets->TextEntry = GTK_ENTRY(gtk_builder_get_object(builder, "TextEntry"));
    widgets->send_textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "send_textbuffer"));
    widgets->textlabel = GTK_LABEL(gtk_builder_get_object(builder, "textlabel"));

    g_signal_connect(widgets->sendTextButton, "clicked", G_CALLBACK(on_send_text_button_activate), (gpointer) widgets);

    gtk_widget_set_sensitive(GTK_WIDGET(widgets->TextEntry), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(widgets->sendTextButton), TRUE);
    g_object_unref(builder);

    gtk_widget_show(window);                
    gtk_main();
    free(widgets);

    return 0;
}

