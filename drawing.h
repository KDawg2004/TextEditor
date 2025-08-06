#pragma once

#include <gtk/gtk.h>

//shares surafce with main.c
cairo_surface_t* surface = NULL;

//Clears the surface of the drawing board
static void clear_surface(void)
{
    cairo_t* cr;

    cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    cairo_destroy(cr);
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

//rendering stuff
static void draw_cb(GtkDrawingArea* drawing_area, cairo_t* cr, int width, int height, gpointer data)
{
    //render what ever is stored in surface on the screen
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);
}

//for lines
static double start_x;
static double start_y;
static double prev_x;
static double prev_y;

//no negitive
static unsigned int radius = 3;

static void setRadius(int r)
{
    radius = r;
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
    cairo_set_source_rgb(cr, 0, 0, 0);
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
