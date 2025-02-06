#include <gtk/gtk.h>
#include "server.h"

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv); // Init GTK
    
    GtkWidget *window; // Declaring Window
    GtkWidget *box; // Declaring Container Box

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL); // Init Window

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); // Init Container Box

    gtk_container_add(GTK_CONTAINER (window), box); // Adding Container Box to Window

    gtk_window_set_default_size(GTK_WINDOW (window), 1280, 720); // Setting Window Size

    gtk_window_set_title(GTK_WINDOW (window), "Chat App"); // Setting Window Title

    gtk_window_set_resizable(GTK_WINDOW (window), FALSE); // Setting Window Not Resizable

    g_signal_connect(window, "destroy", gtk_main_quit, NULL); // Enabling x Button

    gtk_widget_show_all(window); // Show Window

    gtk_main(); // Run GTK

    return 0;
}