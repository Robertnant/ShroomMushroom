#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "client.c"

GtkLabel *textlabel;

GtkBuilder      *builder;
GtkWidget       *window;
GtkWidget		*fixed1;
GtkWidget		*fixed2;
GtkWidget		*grid1; 
GtkWidget		*grid2; 
GtkWidget		*grid3; 

char			tmp[1024];
char			tmp_chat[1024]; 
GtkWidget		*button[1000];
GtkWidget		*button_chat[1000]; 

GtkButton *sendTextButton;
GtkEntry *TextEntry;
GtkTextBuffer *send_textbuffer;
GtkLabel *textlabel;

FILE *f_con = NULL; //ccontacts.txt 
FILE *f_chat = NULL; //chat2 (contact)

//struct *user user = get_user_path(".files/.user"); //name of the user 

void on_row();

void on_send_text_button_activate()
{
	char tmp[128];
	//app_widgets *widgets = (app_widgets*) data;
  	sprintf(tmp, "%s", gtk_entry_get_text(TextEntry));
  	gtk_label_set_text(textlabel, (const gchar*) tmp); 
	//add it on chat.txt 

}

void on_TextEntry_changed()
{
	char tmp[128];
	//app_widgets *widgets = (app_widgets*) data;
  	sprintf(tmp, "%s", gtk_entry_get_text(TextEntry));
  	gtk_label_set_text(textlabel, (const gchar*) tmp);
}


/*
char* get_name(char* tmp) //determine user or contact 
{
	char *name = malloc(sizeof(char)*17);

	name[0] = '0'; 

	for(size_t i=1; i<strlen(tmp)-1; i++)
	{
		if (tmp[i] != ']')
		{ name[i] = tmp[i]; } 
	}
	name++; //pop the first element (space)

	return name; 	
}
*/ 

void chat_bubbles() //Display chat bubbles 
{
	//Chat Bubbles -> row-=2 
	//[USER1] --- 
	//[USER2] --- 

	f_chat = fopen("chat.txt", "r"); 

	if (f_chat == NULL)
	{ 
		printf("File not found");
		return; 
	}

	int row2 = 0; 
	int row3 = 0; 

	while(1) 
	{
		if ( fgets(tmp_chat, 1024, f_chat) == NULL) 
		{ break; } 
		
		else 
		{
			tmp_chat[strlen(tmp_chat)-1] = 0; //remove newline byte 
			const char s[2] = "]";
			char *token; 
			
			token = strtok(tmp_chat, s); //[Username]

			if (token == user) //User - right grid (grid3)
			{
				token = strtok(NULL, s); 
				gtk_grid_insert_row(GTK_GRID(grid3), row3);  
				button_chat[row3] = gtk_button_new_with_label(token); 
				gtk_grid_attach (GTK_GRID(grid3), button_chat[row3], 1, row3, 1, 1);
				row3+=2; 
			}
			else //contact - left grid (grid2)
			{
				token = strtok(NULL, s); 
				gtk_grid_insert_row(GTK_GRID(grid2), row2);  
				button_chat[row2] = gtk_button_new_with_label(token); 
				gtk_grid_attach (GTK_GRID(grid2), button_chat[row2], 1, row2, 1, 1);
				row2+=2; 
			} 
		} 
	}
	fclose(f_chat); 
}

void add_contact() //on_add_contact_button_clicked() 
{
	printf("clicked"); 
}



void contacts()
{
	f_con = fopen("contacts.txt", "r");   
	
	if (f_con == NULL) 
	{ 
		printf("File contacts.txt not found\n");
		return ;
	}
	else
	{
		int row = 0; 
		char *token; 

		while (1) 
		{ 
			if (fgets(tmp, 1024, f_con) == NULL) //reads a line 
			{ break; } 
			
			else 
			{
				const char s[2] = "-"; 
				tmp[strlen(tmp)-1] = 0; //remove newline byte 
				token = strtok(tmp, s); //just extract the contact name

				gtk_grid_insert_row(GTK_GRID(grid1), row);  
				button[row] = gtk_button_new_with_label(token); 
				gtk_grid_attach (GTK_GRID(grid1), button[row], 1, row, 1, 1);
				g_signal_connect(button[row], "clicked", G_CALLBACK(chat_bubbles), NULL); 
				row ++;
			} 
		} 
		fclose(f_con); 
	}
}

int main()
{
    gtk_init(NULL, NULL);

    builder = gtk_builder_new_from_file("./interface_dual_grid.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
	
    sendTextButton = GTK_BUTTON(gtk_builder_get_object(builder, "sendTextButton"));
    TextEntry = GTK_ENTRY(gtk_builder_get_object(builder, "TextEntry"));
    send_textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "send_textbuffer"));
    textlabel = GTK_LABEL(gtk_builder_get_object(builder, "textlabel"));

	fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));
	grid1 = GTK_WIDGET(gtk_builder_get_object(builder, "grid1"));
	fixed2 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed2"));
	grid2 = GTK_WIDGET(gtk_builder_get_object(builder, "grid2"));
	grid3 = GTK_WIDGET(gtk_builder_get_object(builder, "grid3"));

    g_signal_connect(sendTextButton, "clicked", G_CALLBACK(on_send_text_button_activate), NULL); 

    gtk_widget_set_sensitive(GTK_WIDGET(TextEntry), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(sendTextButton), TRUE);

	// CONTACTS 
	add_contacts(); 
	contacts(); 

	// CHAT BUBBLES  
	chat_bubbles(); 

    g_object_unref(builder);

    gtk_widget_show(window);                
    gtk_main(); 

    return 0;
} 

