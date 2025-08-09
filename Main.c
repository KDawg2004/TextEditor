//Author: kaevin Barta
//Date: 5/20/25
//File: Main.c

/*
This file holds the boiler plate for the whole program, opens the program and closes it. But also includes start up to all features like drawing pad and text editor             
*/

//libs
#include <gtk/gtk.h>
#include <stddef.h>

//headers
#include "drawing.h"
#include "text.h"

static GtkWidget* color_preview = NULL;

//struct OpenData for passing multiple forms of data through pointers
typedef struct {
    //class that holds alot of functinailty of window management 
    GtkApplication* app;
    //Base class for all widgets
    GtkWidget* start_window;
} OpenData;

/*
  Closes window to text editor/drawing pad and frees up application mem
 */
static void close_window(void)
{
    close_surface();
}

/**
 * Opens window to text editor and sets up application
 *
 * @param button     The GTK button that triggered the callback.
 * @param user_data  Pointer to the data we need (often cast from gpointer).
 */
void open_text_editor(GtkButton* button, gpointer user_data)
{
    OpenData* data = (OpenData*)user_data;
    GtkApplication* app = data->app;
    GtkWidget* start_window = data->start_window;

    g_free(data); // no longer needed
    gtk_window_destroy(GTK_WINDOW(start_window));

    // Create editor window
    GtkWidget* editor_window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(editor_window), "Text Editor");
    gtk_window_set_default_size(GTK_WINDOW(editor_window), 500, 400);

    // Header bar with clear button
    GtkWidget* header = gtk_header_bar_new();
    gtk_header_bar_set_show_title_buttons(GTK_HEADER_BAR(header), TRUE);
    gtk_window_set_titlebar(GTK_WINDOW(editor_window), header);

    GtkWidget* clear_button = gtk_button_new_with_label("Clear");
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header), clear_button);

    // Scrolled text view
    GtkWidget* scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    GtkWidget* textview = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD_CHAR);

    // Add internal margins
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(textview), 12);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(textview), 12);

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), textview);
    gtk_window_set_child(GTK_WINDOW(editor_window), scrolled);

    // Connect clear button to clear the buffer
    g_signal_connect(clear_button, "clicked", G_CALLBACK(clear_text_cb), textview);

    // Present
    gtk_window_present(GTK_WINDOW(editor_window));
}


//--- DRAWING PAD CODE ---

/**
 * Opens up drawing application
 *
 * @param button     The GTK button that triggered the callback.
 * @param user_data  Pointer to the data we need (often cast from gpointer).
 * 
 * @see drawing.h For function calls
 */
static void openDrawingPad(GtkButton* button, gpointer user_data)
{
    OpenData* data = (OpenData*)user_data;//create new struct to hold info
    GtkApplication* app = data->app;//take app data from data struct
    GtkWidget* start_window = data->start_window;

    g_free(data);  // free it here since no longer needed

    gtk_window_destroy(GTK_WINDOW(start_window));

    GtkWidget* window;
    GtkWidget* frame;
    GtkWidget* drawing_area;
    GtkGesture* drag;
    GtkGesture* press;

    //create main window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Drawing Area");

    // Create header bar
    GtkWidget* header = gtk_header_bar_new();
    gtk_window_set_titlebar(GTK_WINDOW(window), header);
    gtk_header_bar_set_show_title_buttons(GTK_HEADER_BAR(header), TRUE);

    // Create hamburger menu button
    GtkWidget* menu_button = gtk_menu_button_new();
    GtkWidget* icon = gtk_image_new_from_icon_name("open-menu-symbolic");
    gtk_menu_button_set_child(GTK_MENU_BUTTON(menu_button), icon);

    // Create popover menu
    GtkWidget* popover = gtk_popover_new();
    GtkWidget* menu_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);

    GtkWidget* options_button = gtk_button_new_with_label("Options");
    GtkWidget* help_button = gtk_button_new_with_label("Help");

    gtk_box_append(GTK_BOX(menu_box), options_button);
    gtk_box_append(GTK_BOX(menu_box), help_button);

    gtk_popover_set_child(GTK_POPOVER(popover), menu_box);
    gtk_menu_button_set_popover(GTK_MENU_BUTTON(menu_button), popover);

    // Add menu button to header bar
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), menu_button);

    //add frame into window
    frame = gtk_frame_new(NULL);
    gtk_window_set_child(GTK_WINDOW(window), frame);

    //drawing area
    drawing_area = gtk_drawing_area_new();
    
    gtk_widget_set_size_request(drawing_area, 400, 400);

    gtk_frame_set_child(GTK_FRAME(frame), drawing_area);

    //hamburger menu signal
    g_signal_connect(help_button, "clicked", G_CALLBACK(on_help_clicked), window);

    //create options button for hamburger menu
    GtkWidget* options_popover = gtk_popover_new();
    gtk_widget_set_parent(options_popover, options_button); // anchor to the button
    

    //options widget style
    GtkWidget* options_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_margin_top(options_box, 10);
    gtk_widget_set_margin_bottom(options_box, 10);
    gtk_widget_set_margin_start(options_box, 10);
    gtk_widget_set_margin_end(options_box, 10);
    
    
    // Toggle button for brush color
    GtkWidget* brush_color = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 256, 1);
    gtk_scale_set_value_pos(GTK_SCALE(brush_color), GTK_POS_RIGHT);

    GtkWidget* color_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);  // vertical box
    GtkWidget* color_label = gtk_label_new("Brush Color");
    gtk_widget_set_halign(color_label, GTK_ALIGN_START);  // left-align label

    gtk_box_append(GTK_BOX(color_box), color_label);
    gtk_box_append(GTK_BOX(color_box), brush_color);
    gtk_box_append(GTK_BOX(options_box), color_box);  // add labeled group to menu

    g_signal_connect(brush_color, "value-changed", G_CALLBACK(on_brush_color_changed), NULL);

    
    // Brush size scale with label VERTICAL 
    GtkWidget* brush_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1, 20, 1);
    gtk_scale_set_value_pos(GTK_SCALE(brush_scale), GTK_POS_RIGHT);

    GtkWidget* size_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);//vertical box
    GtkWidget* size_label = gtk_label_new("Brush Size");
    gtk_widget_set_halign(size_label, GTK_ALIGN_START);//left align label

    gtk_box_append(GTK_BOX(size_box), size_label);
    gtk_box_append(GTK_BOX(size_box), brush_scale);
    gtk_box_append(GTK_BOX(options_box), size_box);//add labeled group to options popover

    g_signal_connect(brush_scale, "value-changed", G_CALLBACK(on_brush_size_changed), NULL);



    // Clear canvas button
    GtkWidget* clear_button = gtk_button_new_with_label("Clear Canvas");
    gtk_box_append(GTK_BOX(options_box), clear_button);

    //Late update, updates after first click once menu closed, please fix later 7/24/25
    g_signal_connect(clear_button, "clicked", G_CALLBACK(clear_surface), drawing_area);

    // Set popover content
    gtk_popover_set_child(GTK_POPOVER(options_popover), options_box);

    //connect drawing and resize
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), draw_cb, NULL, NULL);

    g_signal_connect_after(drawing_area, "resize", G_CALLBACK(resize_cb), NULL);

    g_signal_connect(options_button, "clicked", G_CALLBACK(show_options_popover), options_popover);

    // Create preview widget somewhere in your options_box
    color_preview = gtk_frame_new(NULL);
    gtk_widget_set_size_request(color_preview, 50, 50);  // square preview box
    gtk_box_append(GTK_BOX(options_box), color_preview);


    //left click
    drag = gtk_gesture_drag_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(drag), GDK_BUTTON_PRIMARY);
    gtk_widget_add_controller(drawing_area, GTK_EVENT_CONTROLLER(drag));
    g_signal_connect(drag, "drag-begin", G_CALLBACK(drag_begin), drawing_area);
    g_signal_connect(drag, "drag-update", G_CALLBACK(drag_update), drawing_area);
    g_signal_connect(drag, "drag-end", G_CALLBACK(drag_end), drawing_area);

    //right click, calls clear surface
    press = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(press), GDK_BUTTON_SECONDARY);
    gtk_widget_add_controller(drawing_area, GTK_EVENT_CONTROLLER(press));
    g_signal_connect(press, "pressed", G_CALLBACK(pressed), drawing_area);

    //show window
    gtk_window_present(GTK_WINDOW(window));
    //destroy
    g_signal_connect(window, "destroy", G_CALLBACK(close_window), NULL);
}



/**
 * Activates the appliaction, also serves as the main menu. 
 *
 * @param app     New gtk application to be passed, manages life cycle
 * @param user_data  Pointer to the data we need (often cast from gpointer).
 */
static void activate(GtkApplication* app, gpointer user_data)
{
    //widgets
    GtkWidget* start_window;
    GtkWidget* main_box;
    GtkWidget* title_label;
    GtkWidget* button_text_editor;
    GtkWidget* button_drawing_pad;
    GtkWidget* quit_button;

    // Create the start window
    start_window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(start_window), "Caveman's Toolbox");
    gtk_window_set_default_size(GTK_WINDOW(start_window), 500, 300);
    gtk_window_set_resizable(GTK_WINDOW(start_window), FALSE);

    // Give it a name for CSS
    gtk_widget_set_name(start_window, "start-window");

    // Vertical box to stack title & buttons
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_valign(main_box, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(main_box, GTK_ALIGN_CENTER);
    gtk_window_set_child(GTK_WINDOW(start_window), main_box);

    // Big title
    title_label = gtk_label_new(" Caveman's Toolbox");
    gtk_widget_set_name(title_label, "title-label");
    gtk_box_append(GTK_BOX(main_box), title_label);

    // Buttons
    button_drawing_pad = gtk_button_new_with_label(" Drawing Pad");
    button_text_editor = gtk_button_new_with_label(" Text Editor");

    gtk_box_append(GTK_BOX(main_box), button_drawing_pad);
    gtk_box_append(GTK_BOX(main_box), button_text_editor);

    // Quit button
    quit_button = gtk_button_new_with_label("Quit");
    gtk_widget_set_name(quit_button, "quit-button");
    gtk_box_append(GTK_BOX(main_box), quit_button);

    // Connect signals
    OpenData* data_editor = g_new(OpenData, 1);
    data_editor->app = app;
    data_editor->start_window = start_window;

    OpenData* data_drawing = g_new(OpenData, 1);
    data_drawing->app = app;
    data_drawing->start_window = start_window;

    g_signal_connect(button_text_editor, "clicked", G_CALLBACK(open_text_editor), data_editor);
    g_signal_connect(button_drawing_pad, "clicked", G_CALLBACK(openDrawingPad), data_drawing);
    g_signal_connect_swapped(quit_button, "clicked", G_CALLBACK(close_window), start_window);

    //CSS rule container
    GtkCssProvider* provider = gtk_css_provider_new();

    //set rules
    gtk_css_provider_load_from_string(provider,
        "#start-window { background: linear-gradient(135deg, #1e1e2f, #2e2e3f); }"//background color
        "#title-label { font-size: 24px; font-weight: bold; color: white; }" //title
        "button { font-size: 16px; padding: 8px 20px; border-radius: 8px; }" //text and drawing pad
        "#quit-button { background: #cc3333; color: white; }" //quit
        "#quit-button:hover { background: #ff5555; }" //quit hover
    );

    //apply css
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    //free after use
    g_object_unref(provider);

    gtk_window_present(GTK_WINDOW(start_window));
    

}


//runs the program
int main(int argc, char** argv)
{
    //new application
    GtkApplication* app;
    //flag
    int status;

    //No app Id and flag = 0
    app = gtk_application_new(NULL, G_APPLICATION_DEFAULT_FLAGS);

    //call to the activate function
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    //throws into main loop to hit the g signal above
    status = g_application_run(G_APPLICATION(app), argc, argv);

    //free mem
    g_object_unref(app);

    return status;
}




