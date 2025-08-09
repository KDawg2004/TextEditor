#pragma once

#include <gtk/gtk.h>

//shares surafce with main.c
cairo_surface_t* surface = NULL;
extern GtkWidget* color_preview;


//Clears the surface of the drawing board
static void clear_surface(void)
{
    cairo_t* cr;

    cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    cairo_destroy(cr);
}

/**
 * Function definition for help screen, displays useful tips
 *
 * @param button     The GTK button that triggered the callback.
 * @param user_data  Pointer to the data we need (often cast from gpointer).
 */
static void on_help_clicked(GtkButton* button, gpointer user_data) {
    GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(user_data), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "This is a simple drawing pad.\nLeft click to draw.\nRight click to clear drawing.");
    gtk_window_set_title(GTK_WINDOW(dialog), "Help");
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
    gtk_window_present(GTK_WINDOW(dialog));
}

/**
 * Function definition for options menu.
 *
 * @param button     The GTK button that triggered the callback.
 * @param user_data  Pointer to the data we need (often cast from gpointer).
 */
static void show_options_popover(GtkButton* button, gpointer user_data) {
    GtkWidget* popover = GTK_WIDGET(user_data);
    gtk_widget_set_visible(popover, TRUE);
}

//Create a new surface to store our drawing 
static void resize_cb(GtkWidget* widget, int width, int height, gpointer   data)
{
    if (surface)
    {
        cairo_surface_destroy(surface);
        surface = NULL;
    }

    if (gtk_native_get_surface(gtk_widget_get_native(widget)))
    {
        surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
            gtk_widget_get_width(widget),
            gtk_widget_get_height(widget));

        //Initialize the surface to white 
        clear_surface();
    }
}


//to clario decimal
static double red_f, green_f, blue_f;

//How this works, pretty interesting: https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB
static void on_brush_color_changed(GtkRange* range, gpointer user_data) 
{   
    //rgb
    static unsigned int red = 0, blue = 0, green = 0;
    //--PREVIEW STUFFS--
    int value = (int)gtk_range_get_value(range);

    // ...HSV to RGB conversion here, filling r,g,b ...
    //---Complicated stuff---

    

    // Clamp value to [0, 255] if needed
    if (value < 0) value = 0;
    if (value > 255) value = 255;

    int region = value / 43;      // 6 regions (0-5) of 43 values each
    int remainder = (value - (region * 43)) * 6;

    int p = 0;
    int q = 255 - remainder;
    int t = remainder;

    switch (region) {
    case 0: red = 255; green = t; blue = p; break;       // Red to Yellow
    case 1: red = q; green = 255; blue = p; break;       // Yellow to Green
    case 2: red = p; green = 255; blue = t; break;       // Green to Cyan
    case 3: red = p; green = q; blue = 255; break;       // Cyan to Blue
    case 4: red = t; green = p; blue = 255; break;       // Blue to Magenta
    case 5: red = 255; green = p; blue = q; break;       // Magenta to Red
    }
    //for draw color
    red_f = red / 255.0;
    green_f = green / 255.0;
    blue_f = blue / 255.0;

    // Update preview widget's background color using CSS
    char css[100];
    snprintf(css, sizeof(css),
        "#color_preview { background-color: rgb(%d, %d, %d); }", red, green, blue);
    
    GtkStyleContext* context = gtk_widget_get_style_context(color_preview);
    
    //set id
    gtk_widget_set_name(color_preview, "color_preview");

    // Apply inline CSS
    GtkCssProvider* provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, css);
    gtk_style_context_add_provider_for_display(gtk_widget_get_display(color_preview), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_object_unref(provider);

    // Redraw preview widget
    gtk_widget_queue_draw(color_preview);
}

//rendering stuff
static void draw_cb(GtkDrawingArea* drawing_area, cairo_t* cr, int width, int height, gpointer data)
{
    //render what ever is stored in surface on the screen
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);
}

//for lines
static double start_x, start_y, prev_x, prev_y;

//no negitive
static unsigned int radius = 3;

/**
 * Function definition for chaging the brush radius size, this increases the width of the stroke
 *
 * @param range     input for radius
 * @param user_data  Pointer to the data we need (often cast from gpointer).
 */
static void on_brush_size_changed(GtkRange* range, gpointer user_data)
{
    int value = (int)gtk_range_get_value(range);
    radius = value;
}

//start line
static void drag_begin(GtkGestureDrag* gesture, double x, double y, GtkWidget* area)
{
    //set same to start
    prev_x = start_x = x;
    prev_y = start_y = y;

    cairo_t* cr = cairo_create(surface);
    cairo_arc(cr, prev_x, prev_y, 1.5, 0, 2 * 3.1415);//4 is enough
    cairo_fill(cr);
    cairo_destroy(cr);

    gtk_widget_queue_draw(area);
}

//mini lines
static void drag_update(GtkGestureDrag* gesture, double x, double y, GtkWidget* area)
{
    double new_x = start_x + x;
    double new_y = start_y + y;

    cairo_t* cr = cairo_create(surface);
    cairo_set_source_rgb(cr, red_f, green_f, blue_f);
    cairo_set_line_width(cr, radius);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

    cairo_move_to(cr, prev_x, prev_y);
    cairo_line_to(cr, new_x, new_y);
    cairo_stroke(cr);

    cairo_destroy(cr);

    prev_x = new_x;
    prev_y = new_y;

    gtk_widget_queue_draw(area);
}

static void drag_end(GtkGestureDrag* gesture, double x, double y, GtkWidget* area)
{
    drag_update(gesture, x, y, area);
}

//clear surface function 
static void pressed(GtkGestureClick* gesture, int n_press, double x, double y, GtkWidget* area)
{
    clear_surface();
    gtk_widget_queue_draw(area);
}

void close_surface(void)
{
    if (surface)
        cairo_surface_destroy(surface);
}

