#include <gtk/gtk.h>

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    
    GtkWidget *window;
    GtkWidget *button1;
    GtkWidget *button2;
    GtkWidget *box;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    button1 = gtk_button_new_with_label("Button 1");
    button2 = gtk_button_new_with_label("Button 2");

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_container_add(GTK_CONTAINER (window), box);

    gtk_box_pack_start(GTK_BOX (box), button1, FALSE, FALSE, 50);
    gtk_box_pack_start(GTK_BOX (box), button2, FALSE, FALSE, 50);

    gtk_window_set_default_size(GTK_WINDOW (window), 1280, 720);

    gtk_window_set_title(GTK_WINDOW (window), "Chat App");

    gtk_window_set_resizable(GTK_WINDOW (window), FALSE);

    g_signal_connect(window, "destroy", gtk_main_quit, NULL);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}