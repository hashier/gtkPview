#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <gtk/gtk.h>
#include <glib.h>

#include <curl/curl.h>
#include <gdk/gdkkeysyms.h>
#include "main.h"

#if defined(TITS)
  #define URL "http://tissit.teurasporsaat.org/random.php"
#elif defined(APINAPORN)
  #define URL "http://stargazers.nor.fi/random_image/apinaporn.cgi"
#elif defined(APINA)
  #define URL "http://stargazers.nor.fi/random_image/apina.cgi"
#else
  #define URL "http://localhost/~hashier/relpub_2/schwerkraft.gif"
#endif
#define FILENAME "logo.jpg"

#define DEBUG

#ifndef DEBUG
  #define printDebugMsg(msg) ((void)0)
  #define printDebugMsgv(msg) ((void)0)
#else
  #define printDebugMsg(msg) (printDebug(msg))
  #define printDebugMsgv(msg) (printDebugv(msg, __FILE__, __LINE__))
#endif

void printDebugv( const char *msg, const char *file, const int line) {
	fprintf(stderr, "%s : line %d in %s\n", msg, line, file);
}
void printDebug( const char *msg) {
	fprintf(stderr, "%s\n", msg);
}

int main(int argc, char **argv) {
	struct _APP app;

	gtk_init(&argc, &argv);

	app.state = STATE_NORMAL;
	app.list = NULL;
	app.current = NULL;

	// This must be NULL if there is no loaded pixbuf at all.
	app.scaled = NULL;
	app.pixbuf = NULL;

	app.width = 0;
	app.height = 0;

	/* creating stuff */
	create(&app);

	/* putting stuff together */
	put(&app);

	/* Connect signals */
	connect_signals(&app);

	// Default timeout is 3 seconds
	app.slideshow_timeout = 3000;

	/* make visible */
	makevisable(&app);

	// This is used to tell if slideshow is stopped when
	// we download new image. If it is, then we do not want to
	// show downloaded image.
	// 2 = Not set yet, no slidehsow is running
	// 1 = Stopped on the fly
	// 0 = Not stopped, slideshow still goes on
	app.slideshow_stopped_on_the_fly = 2;

	// Check cli params
	int i;
	int start_slideshow = 0;

	for( i=0; i<argc; i++ )
	{
		if( strcmp( argv[i], "-f" ) == 0 )
			toggle_window_state( STATE_NORMAL, &app );
		else if( strcmp( argv[i], "-h" ) == 0 )
			show_help();
		else if( strcmp( argv[i], "--help" ) == 0 )
			show_help();
		else if( strcmp( argv[i], "--fullscreen" ) == 0 )
			toggle_window_state( STATE_NORMAL, &app );

		// We do not want start slideshow just now, because
		// use might have given --timeout parameter, so we
		// first loop all params and set it later
		else if( strcmp( argv[i], "--slideshow" ) == 0 )
			start_slideshow = 1;

		// Set timeout for slideshow
		else if( strcmp( argv[i], "--timeout" ) == 0 )
		{
			if( argc >= i+1 )
			{
				// Get next param value and convert
				// it to integer.
				guint tmp = (guint)atoi( argv[i+1] );

				// If atoi failed, keep default value
				if( tmp == 0 )
					tmp = app.slideshow_timeout;

				// If user has given value under 1000, 
				// then user might have meant to set how
				// many seconds to wait instead of milliseconds.
				// Just fix it.
				if( tmp < 1000 )
					tmp = tmp * 1000;

				// Set timeout
				app.slideshow_timeout = tmp;
				g_print( "Timeout is %d\n", app.slideshow_timeout );
			}
		}
	}

	// If --slideshow was given, then start it
	if( start_slideshow )
	{
		app.slideshow = RUNNING;
		g_timeout_add( app.slideshow_timeout,
				(GSourceFunc)slideshow_next, &app );
	}

	/* load first image, if available */
	start_up(&app);

	/* run */
	gtk_main();

	return 0;
}

void show_help()
{
	g_print( "gtkPview\n\n" );
	g_print( "USAGE: gtkPview [PARAM]\n" );
	g_print( "Shows tits from tissit.teurasporsaat.org\n\n" );
	g_print( "PARAMS\n" );
	g_print( "--help or -h\t\tThis help\n" );
	g_print( "--slideshow\t\tStart slideshow\n" );
	g_print( "--timeout\t\tTimeout for slideshow. Under 1000 is for\n" );
	g_print( "\t\t\tseconds, over 1000 is for milliseconds\n" );
	g_print( "--fullscreen or -f\tGo fullscreen mode\n\n" );
	g_print( "KEYS\n" );
	g_print( "d\t\t\tStart/stop slideshow\n" );
	g_print( "f\t\t\tGo fullscreen/back\n" );
	g_print( "s\t\t\tSave all images\n" );
	g_print( "q\t\t\tQuit application\n" );
	g_print( "h or left\t\tPrevious image\n" );
	g_print( "l or right\t\tNext image\n" );
	g_print( "g\t\t\tDownload new image\n" );
	g_print( "b\t\t\tBoss key\n" );
	g_print( "?\t\t\tShow keybindings\n\n" );
	exit(0);
}

void toggle_window_state( int state, APP *app )
{
	if( state == STATE_NORMAL )
	{
		// Eventbox background color is nicer when it is black
		GdkColor color;
		gdk_color_parse( "black", &color );
		gtk_widget_modify_bg( app->eventbox, GTK_STATE_NORMAL,
				&color );

		gtk_widget_hide( app->hbox );
		gtk_widget_hide( app->hbox2 );
		gtk_widget_hide( app->hbox_mid );
		gtk_window_fullscreen( GTK_WINDOW( app->window ) );
		app->state = STATE_FULLSCREEN;
	}
	else
	{
		gtk_window_unfullscreen( GTK_WINDOW( app->window ) );
		app->state = STATE_NORMAL;
		gtk_widget_show( app->hbox );
		gtk_widget_show( app->hbox2 );
		gtk_widget_show( app->hbox_mid );

		// Back to gray
		GdkColor color;
		gdk_color_parse( "#e7e5e4", &color );
		gtk_widget_modify_bg( app->eventbox, GTK_STATE_NORMAL, 
				&color );
	}
}


static gboolean slideshow_next( APP *app )
{
	callback_btn_dl( NULL, app );

	if( app->slideshow == RUNNING )
	{
		g_print( "Slideshow is running...\n" );
		return TRUE;
	}

	g_print( "Stopped slideshow\n" );
	return FALSE;
}

// *********************************************
//	update_title
//
//	@brief Update window title, eg. shows the
//		number of current image and number
// 		of images total in linked list.
//
//	@param APP *app Application structure
//
// *********************************************
static void update_title( APP *app )
{
	gchar *title = malloc( 1024 );
	sprintf( title, "gtkPview (%u/%u)", 
		( g_list_index( app->list, app->pixbuf ) +1 ),
		g_list_length( app->list ) );

	gtk_window_set_title( GTK_WINDOW( app->window ), title );
	free( title );
}


static size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written;

	written = fwrite(ptr, size, nmemb, stream);
	return written;
}

static void quit_prog(GtkWidget *widget, GtkWidget *image) {
	gtk_main_quit();
}

static int get_new_image(APP *app) {
	CURL *curl_handle;
	FILE *imagefile;
	char imagefilename[] = FILENAME;

	// Download imagefile to file
	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, URL );
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, &write_data);
	imagefile = fopen(imagefilename, "w");

	if (imagefile == NULL) {
		curl_easy_cleanup(curl_handle);
		return -1;
	}
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, imagefile);
	curl_easy_perform(curl_handle);
	fclose(imagefile);
	curl_easy_cleanup(curl_handle);

	addImageToList( app);

	return TRUE;
}

static void create(APP *app) {
	app->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(GTK_WIDGET(app->window), 300, 300);
	gtk_window_set_title(GTK_WINDOW(app->window), "gtkPview");
	app->vbox = gtk_vbox_new(FALSE, 5);
	app->hbox = gtk_hbox_new(FALSE, 0);
	app->btn_dl = gtk_button_new_with_label("Download");
	app->btn_save = gtk_button_new_with_label("Save pic");
	app->btn_save_all = gtk_button_new_with_label( "Save all" );
	app->entry = gtk_entry_new();
	app->image = gtk_image_new();
	app->hbox2 = gtk_hbox_new(FALSE, 0);
	app->btn_prev = gtk_button_new_with_label("Prev pic");
	app->btn_next = gtk_button_new_with_label("Next pic");
	app->eventbox = gtk_event_box_new();
	app->label_text = gtk_label_new("Save file to:");
	app->hbox_mid = gtk_hbox_new(FALSE, 0);
}

static void put(APP *app) {
	gtk_container_add(GTK_CONTAINER(app->window), app->vbox);
	gtk_container_add( GTK_CONTAINER( app->eventbox ), app->image );

	gtk_box_pack_start(GTK_BOX(app->vbox), app->eventbox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(app->vbox), app->hbox, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(app->hbox), app->btn_dl, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(app->hbox), app->btn_save, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(app->vbox), app->hbox_mid, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(app->hbox_mid), app->label_text, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(app->hbox_mid), app->entry, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(app->vbox), app->hbox2, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(app->hbox2), app->btn_prev, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(app->hbox2), app->btn_next, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(app->hbox2), app->btn_save_all, TRUE, TRUE, 0);
}

static gboolean callback_btn_save_all( GtkWidget *widget, APP *app ) 
{
	GError *error = NULL;

	GList *temp;
	temp = g_list_first( app->current );

	// How many items to loop?
	guint num_items = g_list_length( temp );
	g_print( "Num items: %d\n", num_items );

	guint i;

	// Loop all items and save them to 0.jpg, 1.jpg and so on...
	for( i=0; i<num_items; i++ )
	{
		// Create filename
	 	char *filename = malloc( 20 );
		sprintf( filename, "%03d.jpg", i );

		// Save file
		gdk_pixbuf_save( temp->data, filename, "jpeg", &error, "quality",
				"100", NULL );

		free( filename );

		// Remember to jump to next item!
		temp = g_list_next( temp );
	}

	// Show message dialog
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new( GTK_WINDOW( app->window ), 
		GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
		"Saved %d images!", num_items );

	gtk_window_set_title( GTK_WINDOW( dialog ), "Information" );
	gtk_dialog_run( GTK_DIALOG( dialog ) );
	gtk_widget_destroy( dialog );

	return FALSE;
}

static gboolean callback_key_pressed( GtkWidget *w, GdkEventKey *e, APP *app )
{
	if ( GTK_WIDGET_HAS_FOCUS( app->entry) ) {
		// The value returned from this function indicates whether
		// the event should be propagated further by the GTK event
		// handling mechanism. Returning TRUE indicates that the event
		// has been handled, and that it should not propagate further.
		// Returning FALSE continues the normal event handling. See
		// the section on Advanced Event and Signal Handling for more
		// details on this propagation process.
		// http://www.gtk.org/tutorial1.2/gtk_tut-18.html#sec_Adv_Events_and_Signals
		return FALSE;
	} else {
	switch( e->keyval )
	{
		// To fullscreen and back
		case 'f':

			// To fullscreen
			if( app->state == STATE_NORMAL )
				toggle_window_state( STATE_NORMAL, app );
			else
				toggle_window_state( STATE_FULLSCREEN, app );
			break;

		// Previous image
		case 'h':
		case GDK_Left:
		case GDK_BackSpace:
				callback_btn_prev( NULL, app );
				break;

		// Next image
		case 'l':
		case GDK_Right:
				callback_btn_next( NULL, app );
				break;

		// Quit app
		case 'q':
			gtk_main_quit();
			break;

		case 's':
			callback_btn_save_all( NULL, app );
			break;

		case 'd':
			// If slideshow is currently running, then we should
			// save "slideshow_stopped_on_the_fly" variable to 1.
			// Then we know that we should not show downloaded image.
			if( app->slideshow == RUNNING )
			{
				app->slideshow_stopped_on_the_fly = 1;
				app->slideshow = STOPPED;
			}
			else
			{
				app->slideshow = RUNNING;
				app->slideshow_stopped_on_the_fly = 2;

				g_timeout_add( app->slideshow_timeout,
						(GSourceFunc)slideshow_next, app );
			}
			break;

		// 'g' should switch image
		case 'g':
			callback_btn_dl( NULL, app );
			break;

		case '?':
			g_print( "d\t\t\tStart/stop slideshow\n" );
			g_print( "f\t\t\tGo fullscreen/back\n" );
			g_print( "s\t\t\tSave all images\n" );
			g_print( "q\t\t\tQuit application\n" );
			g_print( "h or left\t\tPrevious image\n" );
			g_print( "l or right\t\tNext image\n" );
			g_print( "g\t\t\tDownload new image\n" );
			g_print( "b\t\t\tBoss key\n" );
			g_print( "?\t\t\tShow keybindings\n\n" );
			break;

		case 'b':
			if ( GTK_WIDGET_VISIBLE( app->image) )
				gtk_widget_hide( app->image);
			else
				gtk_widget_show( app->image);

	}
}
return FALSE;
}

gboolean changed_state( GtkWidget *w, GdkEventConfigure *e, APP *app )
{
	// If window size hasn't changed, then we don't want
	// to scale pixbuf again and set it back, because image
	// should be same sized as before. Just return.
	if (app->width == e->width && app->height == e->height) {
		return FALSE;
	}

	// Read application REAL width and height, not
	// the cached one (what we will get with gtk_window_get_size).
	// see: http://library.gnome.org/devel/gtk/unstable/GtkWindow.html#gtk-window-get-size
	// for more information about that
	app->width = e->width;
	app->height = e->height;

	// Create scaled pixbuf and set it to image.
	if( app->pixbuf != NULL )
	{
		get_scaled( app, DIR_CURRENT );
		gtk_image_set_from_pixbuf(GTK_IMAGE(app->image), app->scaled);
	}

	return FALSE;
}

static void connect_signals(APP *app) {
	g_signal_connect(G_OBJECT(app->window), "destroy", G_CALLBACK(quit_prog),
			app->image);
	g_signal_connect(G_OBJECT(app->btn_dl), "clicked", G_CALLBACK(
			callback_btn_dl), app);
	g_signal_connect(G_OBJECT(app->btn_save), "clicked", G_CALLBACK(
			callback_btn_save), app);
	g_signal_connect( G_OBJECT( app->btn_save_all ), "clicked",
			G_CALLBACK( callback_btn_save_all ), app );

	// This is required when we want to switch fullscreen and
	// directly scale loaded image to fill the whole window area.
	g_signal_connect( G_OBJECT( app->window ), "configure_event", G_CALLBACK(
			changed_state), app);

	/* Keept for recollection
	g_signal_connect(G_OBJECT(app->window), "expose-event", G_CALLBACK(
			resize_window), app);
	*/
	g_signal_connect(G_OBJECT(app->btn_prev), "clicked", G_CALLBACK(
			callback_btn_prev), app);
	g_signal_connect(G_OBJECT(app->btn_next), "clicked", G_CALLBACK(
			callback_btn_next), app);

	// Listen keypresses
	g_signal_connect( G_OBJECT( app->window ), "key-press-event",
			G_CALLBACK( callback_key_pressed ), app );
}

static void makevisable(APP *app) {
	gtk_widget_show_all( app->window );
}

static gboolean start_up(APP *app) {
	gint width, height;
	GError *error = NULL;

	if (app->current == NULL && !app->list) {

		// Does image file exists?
		FILE *fp = fopen(FILENAME, "r");
		if (fp == NULL) {
			g_print("Can't open %s\n", FILENAME);
			return FALSE;
		}
		fclose(fp);

		// Read window size, so we can scale image
		gtk_window_get_size(GTK_WINDOW(app->window), &width, &height);

		// Full sized pixbuf
		app->pixbuf = gdk_pixbuf_new_from_file(FILENAME, &error);

		// Scaled pixbuf
		app->scaled = gdk_pixbuf_new_from_file_at_scale(FILENAME, width,
				height, TRUE, &error);

		g_print("Startup loaded picture 1\n");

		// Create first list element
		// We save full sized pixbuf
		app->list = g_list_append(NULL, app->pixbuf);
		app->current = app->list;

		if (error != NULL) {
			g_print("Error: %s\n", error->message);
			error = NULL;
			return FALSE;
		}
	}

	update_title( app );

	return FALSE;
}

static gboolean addImageToList( APP *app) {
	gint width, height;
	GError *error = NULL;

	// Read window size.
	gtk_window_get_size(GTK_WINDOW(app->window), &width, &height);

	// Create fullsized pixbuf
	app->pixbuf = gdk_pixbuf_new_from_file(FILENAME, &error);

	// Create scaled pixbuf
	app->scaled = gdk_pixbuf_new_from_file_at_scale(FILENAME, width, height,
			TRUE, &error);

	// We must add full sized pixbuf to list
	app->list = g_list_append(app->list, app->pixbuf);
	app->current = g_list_nth(app->list, g_list_index(app->list, app->pixbuf));

	if (error != NULL) {
		g_print("Error: %s\n", error->message);
		error = NULL;
		return FALSE;
	}

	return TRUE;
}

static gboolean set_image(GtkWidget *widget, GdkEventButton *event, APP *app) {

	// Put scaled image here
	gtk_image_set_from_pixbuf(GTK_IMAGE(app->image), app->scaled);

	update_title( app );
	g_print("Showing pic number: %u\n", g_list_index(app->list, app->pixbuf) +1);

	return FALSE;
}

static gboolean callback_btn_dl(GtkWidget *widget, APP *app) {

	if (get_new_image(app))
	{
		if( app->slideshow_stopped_on_the_fly != 1 )
		{
			set_image(NULL, NULL, app);
			app->slideshow_stopped_on_the_fly = 2;
		}
	}
	return FALSE;
}

static gboolean callback_btn_save(GtkWidget *widget, APP *app) {
	GError *error = NULL;
	const gchar *filename;

	filename = gtk_entry_get_text(GTK_ENTRY(app->entry));

	// Add JPG-extension
	char *final_name = malloc( strlen( filename ) + 5 );
	sprintf( final_name, "%s.jpg", filename );

	gdk_pixbuf_save(app->pixbuf, (const gchar *) final_name, "jpeg", 
		&error, "quality", "100", NULL);

	if (error != NULL) {
		g_print("Error: %s\n", error->message);
		error = NULL;
		return FALSE;
	}

	// Show message dialog
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new( GTK_WINDOW( app->window ), 
		GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
		"This image is now saved in file %s.", final_name );

	gtk_window_set_title( GTK_WINDOW( dialog ), "Information" );
	gtk_dialog_run( GTK_DIALOG( dialog ) );
	gtk_widget_destroy( dialog );

	return FALSE;
}

// *********************************************
//	get_scaled
//
//	@brief Load fullsized pixbuf and create scaled
//		pixbuf to app->scaled, so we can set
//		it to image component.
//
//	@param APP *app
//
//	@param char direction
//
// *********************************************
void get_scaled(APP *app, char direction) {
	GError *error = NULL;
	int width, height;

	// Read window size, so we know what sized our
	// scaled pixbuf should be.
	if( app->width == 0 )
	{
		gtk_window_get_size(GTK_WINDOW(app->window), &width, &height);
	}
	else
	{
		width = app->width;
		height = app->height;
	}

	// Should we load next or previous item from list?
	if (direction == DIR_PREVIOUS)
		app->current = g_list_previous(app->current);
	else if( direction == DIR_NEXT )
		app->current = g_list_next(app->current);
	else
		app->current = app->current;

	// Save our full sized pixbuf to file
	gdk_pixbuf_save(app->current->data, FILENAME, "jpeg", &error, "quality",
			"100", NULL);

	// We must free old scaled pixbuf, or else this will take
	// to much memory in long run... :)
	if (app->scaled != NULL)
		g_object_unref(app->scaled);

	// Now we can load it scaled from file.
	// I think that there can be easier way somehow to load
	// it and also keep aspect ratio, but until I find it I use this...
	app->scaled = gdk_pixbuf_new_from_file_at_scale(FILENAME, width, height,
			TRUE, &error);
	app->pixbuf = app->current->data;
}

static gboolean callback_btn_prev(GtkWidget *widget, APP *app) {
	/*app->pixbuf = g_list_nth_data(app->list, (g_list_position(app->list, app->current)-1));*/
	/*app->current = g_list_nth(app->list, (currentst_position(app->list, app->current)-1));*/
	if (g_list_previous(app->current) == NULL) {
		return FALSE;
	}

	// Read scaled image to app->scaled pixbuf
	get_scaled(app, DIR_PREVIOUS);
	set_image( NULL, NULL, app);

	return TRUE;
}

static gboolean callback_btn_next(GtkWidget *widget, APP *app) {
	if (g_list_next(app->current) == NULL) {
		// On last image, but try to get the next -> dl a new one
		get_new_image(app);
	} else {
		// Read to app->scaled scaled pixbuf.
		get_scaled(app, DIR_NEXT);
	}

	set_image( NULL, NULL, app);

	return FALSE;
}
