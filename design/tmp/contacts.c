#include <stdlib.h>
#include <stdio.h> 
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>

//Make them global

GtkWidget	*window;
GtkWidget	*fixed;
GtkWidget	*grid; 
//GtkWidget	*label[1000]; 
GtkWidget	*button[1000];
GtkBuilder	*builder; 

void		on_destroy(); 
void		on_row();

char	tmp[1024]; 
int		row; // row in contacts.txt 
FILE *f = NULL; 

int main() 
{
	gtk_init(NULL, NULL); 
 
	builder = gtk_builder_new_from_file ("contacts.glade");
 
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

	g_signal_connect(window, "destroy", G_CALLBACK(on_destroy), NULL);

    gtk_builder_connect_signals(builder, NULL);

	fixed = GTK_WIDGET(gtk_builder_get_object(builder, "fixed"));
	grid = GTK_WIDGET(gtk_builder_get_object(builder, "grid"));
	
	f = fopen("contacts.txt", "r");  
	
	if (f == NULL)
	{ 
		printf("File contacts.txt not found\n");
		return EXIT_FAILURE; 
	}

	else
	{
		row = 0; 
		while (1) 
		{ 
			if (fgets(tmp, 1000, f) == NULL) //reads a line 
			{ break; } 
			
			else 
			{
				tmp[strlen(tmp)-1] = 0; //remove newline byte 
				gtk_grid_insert_row(GTK_GRID(grid), row);  
				//label[row] = gtk_label_new(tmp); 
				button[row] = gtk_button_new_with_label(tmp); 
				gtk_grid_attach (GTK_GRID(grid), button[row], 1, row, 1, 1);
				g_signal_connect(button[row], "clicked", G_CALLBACK(on_row), NULL); 
				row ++;
			} 
		} 

		fclose(f); 
	}

	gtk_widget_show_all(window);

	gtk_main();

	return EXIT_SUCCESS; 
} 
//......................................................................................

void on_row() 
{
	//open window - window 2 
	printf("Button is clicked!\n"); 
}

void on_destroy() 
{ 
	gtk_main_quit();
}

