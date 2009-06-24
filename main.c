#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <glib.h>

#include <curl/curl.h>
#include "main.h"

#define URL "http://tissit.teurasporsaat.org/random.php"
#define FILENAME "logo.jpg"

// These tells if we should load next or previous image from list
#define PREVIOUS 0
#define NEXT 1

int main(int argc, char **argv) {
	struct _APP app;

	gtk_init (&argc, &argv);

	app.list=NULL;
	app.current=NULL;

	// This must be NULL if there is no loaded pixbuf at all.
	app.scaled = NULL;

	/* creatin stuff */
	create(&app);

	/* putting stuff together */
	put(&app);

	/* Connect signals */
	connect_signals(&app);

	/* make visable */
	makevisable(&app);

	/* run */
	gtk_main();

	return 0;
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written;

	written = fwrite(ptr,size,nmemb,stream);
	return written;
}

static void quit_prog(GtkWidget *widget, GtkWidget *image) {
	/*g_object_unref(image);*/
	gtk_main_quit();
}

static int get_new_image(APP *app) {
	CURL *curl_handle;
	FILE *imagefile;
	char imagefilename[] = FILENAME;
	gint width, height;
	GError *error = NULL;

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

	// Read window size. This is needed when we create scaled pixbuf.
	gtk_window_get_size(GTK_WINDOW(app->window), &width, &height);

	// Create fullsized pixbuf
	app->pixbuf = gdk_pixbuf_new_from_file( FILENAME, &error );

	// Create scaled pixbuf 
	app->scaled = gdk_pixbuf_new_from_file_at_scale( FILENAME, 
		width, height, TRUE, &error);

	if (error != NULL) {
		g_print("Error: %s\n", error->message);
		error = NULL;
		return FALSE;
	}

	// We must add full sized pixbuf
	app->list=g_list_append(app->list, app->pixbuf);
	app->current=g_list_nth(app->list, g_list_index(app->list, app->pixbuf));

	g_print("Showing pic number: %u\n", g_list_index(app->list, app->pixbuf));

	return TRUE;
}

static void create(APP *app) {
	app->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(GTK_WIDGET (app->window), 300, 300);
	gtk_window_set_title(GTK_WINDOW (app->window), "gtkPview");
	app->vbox = gtk_vbox_new(FALSE, 5);
	app->hbox = gtk_hbox_new(FALSE, 0);
	app->btn_dl = gtk_button_new_with_label("Download");
	app->btn_save = gtk_button_new_with_label("Save pic");
	app->entry = gtk_entry_new ();
	app->image = gtk_image_new();
	app->hbox2 = gtk_hbox_new(FALSE, 0);
	app->btn_prev = gtk_button_new_with_label("Prev pic");
	app->btn_next = gtk_button_new_with_label("Next pic");
}

static void put(APP *app) {
	gtk_container_add (GTK_CONTAINER (app->window), app->vbox);
	gtk_box_pack_start (GTK_BOX (app->vbox), app->image, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (app->vbox), app->hbox, FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (app->hbox), app->btn_dl, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (app->hbox), app->btn_save, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (app->hbox), app->entry, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (app->vbox), app->hbox2, FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (app->hbox2), app->btn_prev, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (app->hbox2), app->btn_next, TRUE, TRUE, 0);
}

static void connect_signals(APP *app) {
	g_signal_connect(G_OBJECT (app->window), "destroy",
	                  G_CALLBACK(quit_prog), app->image);
	g_signal_connect(G_OBJECT (app->btn_dl), "clicked",
	                  G_CALLBACK(callback_btn_dl), app);
	g_signal_connect(G_OBJECT (app->btn_save), "clicked",
	                  G_CALLBACK(callback_btn_save), app);
	/*
	g_signal_connect(G_OBJECT (app->window), "expose-event",
	                  G_CALLBACK(set_image), app);
	*/
	g_signal_connect(G_OBJECT (app->btn_prev), "clicked",
	                  G_CALLBACK(callback_btn_prev), app);
	g_signal_connect(G_OBJECT (app->btn_next), "clicked",
	                  G_CALLBACK(callback_btn_next), app);
}

static void makevisable(APP *app) {
	gtk_widget_show(app->vbox);
	gtk_widget_show(app->hbox);
	gtk_widget_show(app->btn_dl);
	gtk_widget_show(app->btn_save);
	gtk_widget_show(app->entry);
	gtk_widget_show(app->image);
	gtk_widget_show(app->window);
	gtk_widget_show(app->hbox2);
	gtk_widget_show(app->btn_next);
	gtk_widget_show(app->btn_prev);
}

static gboolean set_image(GtkWidget *widget, GdkEventButton *event, APP *app) {
	gint width, height;
	GError *error = NULL;

	if (app->current == NULL) {

		// Does image file exists?
		FILE *fp = fopen( FILENAME, "r" );
		if( fp == NULL )
			return FALSE;
		fclose( fp );

		// Read window size, so we can scale image
		gtk_window_get_size(GTK_WINDOW(app->window), &width, &height);

		// Full sized pixbuf
		app->pixbuf = gdk_pixbuf_new_from_file( FILENAME, &error );

		// Scaled pixbuf
		app->scaled = gdk_pixbuf_new_from_file_at_scale( FILENAME, 
			width, height, TRUE, &error);

		if (error != NULL) {
			g_print("Error: %s\n", error->message);
			error = NULL;
			return FALSE;
		}
	}

	/*g_object_unref(pixbuf);*/
	if (! app->list) {
		g_print("First time, saving start pic\n");

		// We save full sized pixbuf
		app->list=g_list_append(NULL, app->pixbuf);
		app->current=app->list;
	}

	//gtk_image_set_from_pixbuf(GTK_IMAGE(app->image), app->pixbuf);

	// Set scaled pixbuf to image element.
	gtk_image_set_from_pixbuf(GTK_IMAGE(app->image), app->current->data);

	return FALSE;
}

static gboolean callback_btn_dl(GtkWidget *widget, APP *app) {
	if(get_new_image(app)) {
		set_image(NULL, NULL, app);
	}
	return FALSE;
}

static gboolean callback_btn_save(GtkWidget *widget, APP *app) {
	GError *error = NULL;
	const gchar *filename;

	filename=gtk_entry_get_text(GTK_ENTRY(app->entry));
	gdk_pixbuf_save(app->pixbuf, filename, "jpeg", &error, "quality", "100", NULL);
	if (error != NULL) {
		g_print("Error: %s\n", error->message);
		error = NULL;
		return FALSE;
	}
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
void get_scaled( APP *app, char direction )
{
	GError *error = NULL;
	int width, height;

	// Read window size, so we know what sized our 
	// scaled pixbuf should be.
	gtk_window_get_size( GTK_WINDOW( app->window ), &width, &height );

	// Should we load next or previous item from list?
	if( direction == PREVIOUS )
		app->current = g_list_previous(app->current);
	else
		app->current = g_list_next(app->current);

	// Save our full sized pixbuf to file
	gdk_pixbuf_save( app->current->data, FILENAME, "jpeg", &error, 
		"quality", "100", NULL);

	// We must free old scaled pixbuf, or else this will take
	// to much memory in long run... :)
	if( app->scaled != NULL )
		g_object_unref( app->scaled );

	// Now we can load it scaled from file.
	// I think that there can be easier way somehow to load
	// it and also keep aspect ratio, but until I find it I use this...
	app->scaled = gdk_pixbuf_new_from_file_at_scale( FILENAME,
		width, height, TRUE, &error );
}


static gboolean callback_btn_prev(GtkWidget *widget, APP *app) {
	/*app->pixbuf = g_list_nth_data(app->list, (g_list_position(app->list, app->current)-1));*/
	/*app->current = g_list_nth(app->list, (currentst_position(app->list, app->current)-1));*/
	if (g_list_previous(app->current) == NULL) {
		return FALSE;
	}

	// Read scaled image to app->scaled pixbuf
	get_scaled( app, PREVIOUS );
	gtk_image_set_from_pixbuf(GTK_IMAGE(app->image), app->scaled);

	g_print("Showing pic number: %u\n", 
		g_list_index(app->list, app->pixbuf));

	return TRUE;
}

static gboolean callback_btn_next(GtkWidget *widget, APP *app) {
	if (g_list_next(app->current) == NULL) {
			return FALSE;
		}

	// Read to app->scaled scaled pixbuf.
	get_scaled( app, NEXT );

	gtk_image_set_from_pixbuf(GTK_IMAGE(app->image), app->scaled);
	g_print("Showing pic number: %u\n", g_list_index(app->list, app->pixbuf));
	return FALSE;
}
