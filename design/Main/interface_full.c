#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <pthread.h>

#include "../../networking/client.h"
#include "../../messages/messages.h"
#include "../../saved_users/users.h"
#include "../../security/elgamal.h"
#include "../../security/huffman.h"
#include "../../security/tools.h"
#include "../../design/addcontact/add_contact.h"
#include "interface_full.h"

#define UNUSED(x) (void)(x)
//#define G_SOURCE_FUNC(f) ((GSourceFunc) (void (*)(void)) (f))
#define MAX_BUFFER 10000

int row = 0;    //grid row counter (contact)
int row2 = 1;   //grid row counter (contact, not really for now)
int row3 = 0;   //grid row counter (user)

//struct *user user = get_user_path(".files/.user"); //name of the user
void on_row();
void column(char path[]);

// pthread_t receiving_thread;



void on_window_main_destroy()
{
    gtk_main_quit();
}

void on_add_contact_button_selected()
{
    // Display New Contact page.
    show_addContact(builder);
}

void select_contact(GtkWidget * widget, gpointer arg)
{
    UNUSED(arg);

    if(target_user)
        free(target_user);

    char * path;
    asprintf(&path, ".files/contacts/%s", gtk_widget_get_name(widget));
    path[26] = 0;
    target_user = get_user_path(path);
    if(!target_user)
    {
        printf("FAILED TO FETCH CONTACT\n");
        return;
    }
    free(path);
    asprintf(&path, ".files/chats/%s", target_user->number);
    chat_bubbles(path);
    free(path);

    // Change the label
    gtk_label_set_text(current_user_label, target_user->username);

    // Change the avatar
    char *image_path;
    asprintf(&image_path, "./design/Avatars/%s.ico", target_user->avatar);
    gtk_image_set_from_file(current_user_avatar, image_path);
    free(image_path);
    
    printf("SELECTED USER:\n\
            username: %s,\n\
            number: %s\n\
            public key: %s-%s-%s\n",
            target_user->username, target_user->number, target_user->pub.g,
            target_user->pub.q, target_user->pub.h);
}

void contacts(char *contacts_path)
{
    char * username;
    char * number;
    f_con = fopen(contacts_path, "r");

    if (f_con == NULL)
    {
        printf("File contacts.txt not found\n");
        return ;
    }
    else
    {
        while (fgets(tmp, 28, f_con))
        {

            /*if () //reads a line
            { break; }

            else
            { */
            //const char s[2] = "-";
            //tmp[strlen(tmp)-1] = 0; //remove newline byte
            //token = strtok(tmp, s); //just extract the contact name
            username = strtok(tmp, "-");
            gtk_grid_insert_row(GTK_GRID(grid1), row);
            button[row] = gtk_button_new_with_label(username);
            number = strtok(NULL, "-");
            gtk_widget_set_name(button[row], number);
            gtk_grid_attach (GTK_GRID(grid1), button[row], 1, row, 1, 1);
            g_signal_connect(button[row], "clicked", G_CALLBACK(select_contact), NULL);
            row ++;
            //}
        }
    }
    //gtk_widget_show_all(main_window);
}


void clear_bubbles()
{
    for (int i = row2 ; i >= 1; i--)
    {
        printf("Clearing row 2 element %d\n", i);
            //Apparently useless since remove row does it
        //gtk_widget_hide(GTK_WIDGET(gtk_grid_get_child_at(GTK_GRID(grid2), 2, i)));
        //gtk_widget_hide(gtk_grid_get_child_at(GTK_GRID(grid2), 2, i));
        //gtk_widget_destroy(GTK_WIDGET(gtk_grid_get_child_at(GTK_GRID(grid2), 2, i)));
        //gtk_widget_destroy(gtk_grid_get_child_at(GTK_GRID(grid2), 2, i));
        gtk_grid_remove_row (GTK_GRID(grid2), i);
        // row2 -= 1;
    }

    // Reset row2 counter.
    row2 = 1;
}

//function to load chat bubbles from a text file
void chat_bubbles(char path[]) //Display chat bubbles
{
	//Chat Bubbles -> row-=2
	//[USER1] ---
	//[USER2] ---

    // Clear previous bubbles.
    if (row2 > 1)
        clear_bubbles();

    //f_chat = fopen("design/Main/chat.txt", "a+");
    f_chat = fopen(path, "a+");

    if (f_chat == NULL)
    {
            printf("File not found");
            return;
    }

    while(1)
    {
        printf("Got into the loop\n");
            if ( fgets(tmp_chat, 1024, f_chat) == NULL)
            {
        // Decrement row2 counter to remove extra row count.
        // row2--;
                break;
            }

            else
            {
                    // TODO Remove.
                    printf("%s\n", tmp_chat);

                    tmp_chat[strlen(tmp_chat)-1] = 0; //remove newline byte
                    const char s[2] = "|";
                    char *token;
                    //char *user = "ROBERT";

                    token = strtok(tmp_chat, s); //[Username]
                    int is_user = strcmp(token, user->username);

                    token = strtok(NULL, s);
                    printf("TEXT: %s\n", token);
                    gtk_grid_insert_row(GTK_GRID(grid2), row2);
                    bubble_chat[row2] = gtk_button_new_with_label(token);
                    gtk_widget_set_hexpand(bubble_chat[row2], TRUE);

                    if (is_user != 0) //User - column2
                    {
                        //gtk_grid_attach (GTK_GRID(grid2), GTK_WIDGET(button_chat[row2]), 1, row2, 1, 1);
                        gtk_grid_attach (GTK_GRID(grid2), bubble_chat[row2], 0, row2, 1, 1);
                    }
                    else // Contact - column1
                    {
                        //gtk_grid_attach (GTK_GRID(grid2), GTK_WIDGET(button_chat[row2]), 2, row2, 1, 1);
                        gtk_grid_attach (GTK_GRID(grid2), bubble_chat[row2], 1, row2, 1, 1);
                    }
                    row2+=1;
            }
    }

    gtk_widget_show_all(grid2);

    printf("ROW 2 count: %d\n", row2);

	fclose(f_chat);
}

/* char* string_linebreak(char *msg)
{
	int l = strlen(msg);
	int n = l/30;

	char string = malloc(sizeof(l+n+1));

	for(int i=0; i<strlen(string); i++)
	{
		if (i%n == 0)
		{ string[i] = msg[i]; }
		else
		{ string[i] = msg[i]; }
	}

	return string;
}
*/

void addBubble(char * sender, char* msg)
{
    UNUSED(sender);

    //gtk_grid_set_row_baseline_position ();


    gtk_grid_insert_row(GTK_GRID(grid2), row2);
    printf("Not bugging 1\n");
    bubble_chat[row2] = gtk_button_new_with_label(msg);
    printf("Not bugging 2\n");
    gtk_widget_set_hexpand(bubble_chat[row2], TRUE);
    printf("Not bugging 3\n");

    if (strcmp(sender, user->number) == 0) //user
    {
        printf("Not bugging 4\n");
        gtk_grid_attach (GTK_GRID(grid2), bubble_chat[row2], 1, row2, 1, 1);
        printf("Not bugging 5\n");
    }
    else //not user
    {
        printf("Not bugging 6\n");
        gtk_grid_attach (GTK_GRID(grid2), bubble_chat[row2], 0, row2, 1, 1);
        printf("Not bugging 7\n");
    }

    row2+=1;

    // gtk_widget_show_all(bubble_chat[row2]);
    gtk_widget_show_all(grid2);

    printf("Not bugging FINAL\n");
}

// function to append msg on column
/*void addColumn(char *msg, int which_user)
{

}*/

// Function to save message to chat log.
void saveMessage(char * sender, char * chat, char *msg)
{
    if(msg)
    {
        char * path = NULL;
        asprintf(&path, ".files/chats/%s", chat);
        f_chat = fopen(path, "a+");
        free(path);
        fprintf(f_chat, "%s|%s\n", sender, msg);
        fclose(f_chat);
    }
}

// Function to compress content field of message.
char *compressContent(struct cyphers *cyphers, int *jsonSize)
{
    printf("\nConverting encryption (compressed) into JSON\n");

    // Compress data.
    size_t compressedLen;
    char *compData = compress(cyphers->en_msg, &compressedLen);

    char * time = malloc(sizeof(char) * 5);
    strcpy(time, "1010");

    char * sender = malloc(sizeof(char) * 11);
    strcpy(sender, user->number);

    char * receiver = malloc(sizeof(char) * 11);
    strcpy(receiver, target_user->number);

    // size_t plen = strlen(dataCyphers->p);
    // char * p = malloc(sizeof(char) * plen+1);
    // strcpy(p, dataCyphers->p);

    // char *content = malloc(sizeof(char) * compressedLen);
    // memcpy(content, compData, compressedLen);

    message->type = TEXT;
    message->content = compData;
    message->p = cyphers->p;
    message->size = cyphers->size;
    message->compSize = compressedLen;
    message->time = time; //"1010";
    message->sender = sender; //"077644562";
    message->receiver = receiver;
    message->filename = 0;

    char *json = genMessage(message, jsonSize);

    // Reset message structure for next incoming message.
    freeMessage(message);

    return json;
}

// Function to send message.
void sendMessage(char *buff)
{
    saveMessage(user->number, target_user->number, buff);
    addBubble(user->number, buff);

    // Step 1: Get receiver's public key (HARDCODED FOR NOW).
    message->receiver = user->number;   // To modify to target_user.
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


    // Ste3: Generate JSON with cyphers (after compression).
    int jsonSize;
    char *json = compressContent(cyphers, &jsonSize);

    // Step 4: Send JSON to server.
    printf("Sending JSON to server\n");

    ssize_t e = rewrite(sockfd, json, jsonSize);
    if (e == -1)
        errx(1, "Write error");

    // Free memory.
    free(cyphers);
    free(json);

}

struct bubble_content
{
    char *sender;
    char *message;
};


void addBubble2(gpointer arg)
{
    struct bubble_content* bub = arg; 

    gtk_grid_insert_row(GTK_GRID(grid2), row2);
    bubble_chat[row2] = gtk_button_new_with_label(bub->message);
    gtk_widget_set_hexpand(bubble_chat[row2], TRUE);

    if (strcmp(bub->sender, user->number) == 0) //user
      gtk_grid_attach (GTK_GRID(grid2), bubble_chat[row2], 1, row2, 1, 1);
    else //not user
      gtk_grid_attach (GTK_GRID(grid2), bubble_chat[row2], 0, row2, 1, 1);

    row2+=1;

    gtk_widget_show_all(grid2);
    free(bub);
}



// Function to retrieve incoming message.
void retrieveMessage()
{
    // Step 5: Receive incoming message from other client.

    char json[MAX_BUFFER];
    int er;

    printf("Waiting for message..\n");

    bzero(json, MAX_BUFFER);
    GString *json_string = g_string_new(NULL);

    int found = 0;
    while((er = read(sockfd, json, MAX_BUFFER - 1)) > 0)
    {
        if (found)
            json_string = g_string_append(json_string, json);

        if(g_str_has_prefix(json, "{"))
        {
            found++;
            json_string = g_string_append(json_string,  json);
        }
        if(g_str_has_suffix(json, "}"))
        {
            found++;
            printf("COMPLETED JSON!!!\n");
            break;
        }
        bzero(json, MAX_BUFFER);
    }
    if (er <= 0)
    {
        pthread_cancel(receiving_thread);
        return;
    }
    if (found != 2)
        return;

    gchar * final = g_string_free(json_string, FALSE);
    
    if (!message)
    {
        printf("MESSAGE ADDRESS: %p\n", message);
    }
    //printf("BUFFER -> %s\n", json);
    parseMessage(final, message);

    printf("\nReceived a message from %s\n", message->sender);
    //sleep(2);

    //printf("MESSAGE CONTENT: %s\n", message->content);

    // free(cyphers);
    
    // Step 6: Retrieve cyphers and decompress data.
    
    struct cyphers *cyphers = malloc(sizeof(struct cyphers)); // WARNING
    cyphers->en_msg = decompress(message->content);
    cyphers->p = message->p;
    cyphers->size = message->size;

    printf("Private key: %p\n", privkey);

    // Step 7: Decrypt message and save to chat file.
    char *res = decrypt_gamal(cyphers, privkey);
    printf("Decrypting received message\n");
    printf("Received message: %s\n", res);

    if (target_user != NULL && target_user->number && 
            message->sender && strcmp(message->sender, target_user->number) == 0)
    {
        struct bubble_content* bub = malloc(sizeof(struct bubble_content));
        bub->message = res;
        bub->sender = target_user->number;
        g_idle_add(G_SOURCE_FUNC(addBubble2), (gpointer) bub);
    }
    printf("Attempting to save message\n");
    saveMessage(message->sender, message->sender, res);
    printf("Message saved\n");

    // Free memory.
    free(res);
    freeMessage(message);
    free(cyphers);
    // free(json);
}

void * start_message_receiver(void * arg)
{
    UNUSED(arg);
    while(1)
    {
        retrieveMessage();
    }
}


void on_send_text_button_activate()
{
    // int len = gtk_entry_get_text_length(TextEntry);
    if (target_user)
    {
        char *tmp = (char*) gtk_entry_get_text(TextEntry);
        sendMessage(tmp);
        gtk_entry_set_text(TextEntry, "");
    }
}

void on_TextEntry_changed()
{
	char tmp[128];
	//app_widgets *widgets = (app_widgets*) data;
  	sprintf(tmp, "%s", gtk_entry_get_text(TextEntry));
  	gtk_label_set_text(textlabel, (const gchar*) tmp);
}

void show_interface(char *interface_path, char *contacts_path, char *chat_path)
{
    UNUSED(chat_path);
    // Get Widgets.
    
    //GtkCssProvider *cssProvider = gtk_css_provider_new();
    //gtk_css_provider_load_from_path(cssProvider, "design/themes/gtk-contained-dark.css", NULL);
    //gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
    //                            GTK_STYLE_PROVIDER(cssProvider),
    //                            GTK_STYLE_PROVIDER_PRIORITY_USER);


    builder = gtk_builder_new_from_file(interface_path);
    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

    close_window_button = GTK_BUTTON(gtk_builder_get_object(builder, "close_window_button"));

    addContactButton = GTK_BUTTON(gtk_builder_get_object(builder, "addContactButton"));

    sendTextButton = GTK_BUTTON(gtk_builder_get_object(builder, "sendTextButton"));
    TextEntry = GTK_ENTRY(gtk_builder_get_object(builder, "TextEntry"));
    //send_textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "send_textbuffer"));
    textlabel = GTK_LABEL(gtk_builder_get_object(builder, "textlabel"));
    current_user_label = GTK_LABEL(gtk_builder_get_object(builder, "current_user_label"));
    current_user_avatar = GTK_IMAGE(gtk_builder_get_object(builder, "current_user_avatar"));

    fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));
    grid1 = GTK_WIDGET(gtk_builder_get_object(builder, "grid1"));
    fixed2 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed2"));
    grid2 = GTK_WIDGET(gtk_builder_get_object(builder, "grid2"));

    // Connect signals.
    g_signal_connect(sendTextButton, "clicked", G_CALLBACK(on_send_text_button_activate), NULL);
    g_signal_connect(addContactButton, "clicked", G_CALLBACK(on_add_contact_button_selected), NULL);
    g_signal_connect(close_window_button, "clicked", G_CALLBACK(on_window_main_destroy), NULL);
    

    // Set Widget sensitivity.
    gtk_widget_set_sensitive(GTK_WIDGET(TextEntry), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(sendTextButton), TRUE);
	// CONTACTS
	// add_contact();
    contacts(contacts_path);

	// CHAT BUBBLES
	//chat_bubbles();

    //g_object_unref(builder);

    gtk_widget_show_all(main_window);

    pthread_create(&receiving_thread, NULL, start_message_receiver, NULL);

    // Close.
    // fclose(f_con);
    // fclose(f_chat);
}


/*
int main()
{

    return 0;
}
*/
