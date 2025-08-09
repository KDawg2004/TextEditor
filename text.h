#pragma once

#include <gtk/gtk.h>

/**
 * Clears text in text editor
 *
 * @param button     The GTK button that triggered the callback.
 * @param user_data  Pointer to the data we need (often cast from gpointer).
 */
static void clear_text_cb(GtkButton* button, gpointer user_data) {
    GtkTextView* textview = GTK_TEXT_VIEW(user_data);
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(textview);
    gtk_text_buffer_set_text(buffer, "", -1);//set back
}
