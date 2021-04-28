#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GtkLabel *textlabel;

typedef struct 
{
    GtkButton *register_now_button;
    GtkEntry *username;
    GtkEntry *phonenumber;
    GtkLabel *error_label;
    
} app_widgets;

app_widgets *curr;


void on_register_now_button_clicked(GtkWidget *widget, gpointer data)
{
	printf("register_now_button pressed\n");
	app_widgets *arg = data; 

	char buffer_for_phone[11];
	strcpy(buffer_for_phone, gtk_entry_get_text(arg->phonenumber)); //might need to free it 

	for (int i = 0; i < 10; i++){
		if (!(buffer_for_phone[i] >= '0' && buffer_for_phone[i] <= '9')){
			printf("Wrong Format : Detected character %c\n", buffer_for_phone[i]);
			break;
		}
	}

	if (strlen(buffer_for_phone) != 10){
		printf("Wrong length\n");
	}

	char buffer_for_username[17];
	strcpy(buffer_for_username, gtk_entry_get_text(arg->username));
	
	if (strlen(buffer_for_username) < 6){
		printf("Wrong length username\n");
	}

}

static void destroy(GtkWidget *widget, gpointer data){
	app_widgets *dest = data;
	free(data);
	//gtk_widget_destroy(widget);
	gtk_window_close((GTK_WINDOW(widget)));
	gtk_main_quit();
}


void show_registration()
{

    GtkBuilder      *builder_registration;
    GtkWidget       *window_registration;
    app_widgets     *widgets_registration = malloc(sizeof(app_widgets));
    

    builder_registration = gtk_builder_new_from_file("./reg_page.glade");
    window_registration = GTK_WIDGET(gtk_builder_get_object(builder_registration, "reg_page"));
    widgets_registration->register_now_button= GTK_BUTTON(gtk_builder_get_object(builder_registration, "register_now_button"));

    widgets_registration->phonenumber = GTK_ENTRY(gtk_builder_get_object(builder_registration, "number_textentry"));
    widgets_registration->username = GTK_ENTRY(gtk_builder_get_object(builder_registration, "username_textentry"));
    //widgets_registration->name_textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder_registration, "name_textbuffer"));
    //widgets->error_label = GTK_LABEL(gtk_builder_get_object(builder, "textlabel"));

    g_signal_connect(widgets_registration->register_now_button, "clicked", G_CALLBACK(on_register_now_button_clicked), (gpointer) widgets_registration);
    g_signal_connect(window_registration, "destroy", G_CALLBACK(destroy), (gpointer) widgets_registration);

    //gtk_widget_set_sensitive(GTK_WIDGET(widgets->TextEntry), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(widgets_registration->register_now_button), TRUE);
    g_object_unref(builder_registration);

    gtk_widget_show(window_registration);                
}

int main()
{
    gtk_init(NULL, NULL);

    show_registration();
    
    gtk_main();
    //free(widgets_registration);

    return 0;
}

