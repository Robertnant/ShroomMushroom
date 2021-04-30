#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "../../networking/client.h"
#include "../../messages/messages.h"
#include "../../saved_users/users.h"
#include "../../security/elgamal.h"
#include "../../security/tools.h"
#include "interface_full.h"

#define MAX_BUFFER 10000

// TODO: "user->number" in "sendMessage" should be changed to "target_user->number" later.


//struct *user user = get_user_path(".files/.user"); //name of the user 
void on_row();

// Function to save message to chat log.
void saveMessage(char *msg, FILE *file)
{
    fprintf(file, "[ROBERT]%s", msg);
}

// Function to send message.
void sendMessage(char *buff)
{
    // Step 1: Get receiver's public key (HARDCODED FOR NOW).
    message->receiver = user->number;   
    char *key = requestKey(message, sockfd);
    receiver_keys = stringtoPub(key);
    printf("Received key: g->%s q->%s h->%s\n", receiver_keys->g,
            receiver_keys->q, receiver_keys->h);

    // Step 2: Encrypt message.
    printf("\nEncrypting typed message using received key\n");

    struct cyphers *cyphers= malloc(sizeof(struct cyphers));
    encrypt_gamal(buff, receiver_keys, cyphers);

    // Free receiver public key.
    free(key);
    free(receiver_keys);


    // Step 3: Generate JSON with cyphers.
    printf("\nConverting encryption into JSON\n");
    //sleep(0.5);

    char * time = malloc(sizeof(char) * 5);
    strcpy(time, "1010");
    char * sender = malloc(sizeof(char) * 11);
    strcpy(sender, "0776727908");


    message->type = TEXT;
    message->content = cyphers->en_msg;
    message->p = cyphers->p;
    message->size = cyphers->size;
    message->time = time; //"1010";
    message->sender = sender; //"077644562";
    message->receiver = NULL;
    message->filename = 0;

    int jsonSize;
    char *json = genMessage(message, &jsonSize);


    // Reset message structure for next incoming message.
    freeMessage(message);

    // Step 4: Send JSON to server.
    printf("Sending JSON to server\n");
    //sleep(2);

    int e = write(sockfd, json, jsonSize);
    if (e == -1)
        errx(1, "Write error");

    // Free memory.
    free(cyphers);
    free(json);
}

// Function to retrieve incoming message.
void retrieveMessage(FILE *contacts)
{
    // Step 5: Receive incoming message from other client.
    // (For now just itself).
    // bzero(json, jsonSize);

    int jsonSize = MAX_BUFFER;
    char json[MAX_BUFFER];

    if (read(sockfd, json, jsonSize) == -1)
        errx(1, "Error reading incoming messages");

    parseMessage(json, message);

    printf("\nReceived a message from %s\n", message->sender);
    //sleep(2);

    // free(cyphers);
    struct cyphers *cyphers = malloc(sizeof(struct message)); // WARNING
    cyphers->en_msg = message->content;
    cyphers->p = message->p;
    cyphers->size = message->size;

    // Step 6: Decrypt message and save to chat file.
    char *res = decrypt_gamal(cyphers, privkey);
    printf("Decrypting received message\n");
    printf("Received message: %s\n", res);
    saveMessage(res, contacts);

    // Free memory.
    free(res);
    freeMessage(message);
    free(cyphers);
    // free(json);
}

void on_send_text_button_activate()
{
    // int len = gtk_entry_get_text_length(TextEntry);
	char *tmp = (char*) gtk_entry_get_text(TextEntry);
    sendMessage(tmp);
    retrieveMessage(f_con);
    chat_bubbles();

	/*
    //app_widgets *widgets = (app_widgets*) data;
  	sprintf(tmp, "%s", gtk_entry_get_text(TextEntry));
  	gtk_label_set_text(textlabel, (const gchar*) tmp); 
	//add it on chat.txt 
    */

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
			char *user = "ROBERT";  

			token = strtok(tmp_chat, s); //[Username]

			if (strcmp(token, user) == 0) //User - right grid (grid3)
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
    char * username;
	f_con = fopen("contacts.txt", "r");   
	
	if (f_con == NULL) 
	{ 
		printf("File contacts.txt not found\n");
		return ;
	}
	else
	{
		int row = 0;  

		while (fgets(tmp, 28, f_con)) 
		{       
                    /*
			if () //reads a line 
			{ break; } 
			
			else 
			{
                        */

                    //const char s[2] = "-";
                    //tmp[strlen(tmp)-1] = 0; //remove newline byte 
                    //token = strtok(tmp, s); //just extract the contact name
                    
                    username = strtok(tmp, "-");
                    gtk_grid_insert_row(GTK_GRID(grid1), row);  
                    button[row] = gtk_button_new_with_label(username); 
                    gtk_grid_attach (GTK_GRID(grid1), button[row], 1, row, 1, 1);
                    g_signal_connect(button[row], "clicked", G_CALLBACK(chat_bubbles), NULL); 
                    row ++;
		
                    //} 
		} 
		fclose(f_con); 
	}
}

void show_interface(char *path)
{
    builder = gtk_builder_new_from_file(path);
    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
	
    sendTextButton = GTK_BUTTON(gtk_builder_get_object(builder, "sendTextButton"));
    TextEntry = GTK_ENTRY(gtk_builder_get_object(builder, "TextEntry"));
    //send_textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "send_textbuffer"));
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
	add_contact(); 
	contacts(); 

	// CHAT BUBBLES  
	chat_bubbles(); 

    //g_object_unref(builder);

    gtk_widget_show_all(main_window);                
}

/*
int main()
{

    return 0;
}
*/
