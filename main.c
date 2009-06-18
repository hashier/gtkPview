#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <curl/curl.h>

typedef struct _APP {
	GtkWidget *vbox, *hbox;
	GtkWidget *btn_dl, *btn_save;
	GtkWidget *entry;
	GtkWidget *image;
	GtkWidget *window;
} APP;

gboolean callback_btn_dl(GtkWidget *widget, APP *app);
gboolean callback_btn_save();
gboolean set_image(GtkWidget *widget, GdkEventButton *event, APP *app);
static void connect_signals(APP *app);

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written;

	written = fwrite(ptr,size,nmemb,stream);
	return written;
}

int get_new_image() {
	CURL *curl_handle;
	FILE *imagefile;
	char imagefilename[] = "logo.jpg";

	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, "http://www.tux-planet.fr/public/images/photos/linux-mastercards.jpg");
	curl_easy_setopt(curl_handle, CURLOPT_URL, "http://tissit.teurasporsaat.org/random.php");
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
}

static void makevisable(APP *app) {
	gtk_widget_show(app->vbox);
	gtk_widget_show(app->hbox);
	gtk_widget_show(app->btn_dl);
	gtk_widget_show(app->btn_save);
	gtk_widget_show(app->entry);
	gtk_widget_show(app->image);
	gtk_widget_show(app->window);
}

static void put(APP *app) {
	gtk_container_add (GTK_CONTAINER (app->window), app->vbox);
	gtk_box_pack_start (GTK_BOX (app->vbox), app->image, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (app->vbox), app->hbox, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (app->hbox), app->btn_dl, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (app->hbox), app->btn_save, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (app->hbox), app->entry, TRUE, TRUE, 0);
}

static void connect_signals(APP *app) {
	g_signal_connect(G_OBJECT (app->window), "destroy",
	                  G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT (app->btn_dl), "clicked",
	                  G_CALLBACK(callback_btn_dl), app);
	g_signal_connect(G_OBJECT (app->btn_save), "clicked",
	                  G_CALLBACK(callback_btn_save), app);
	g_signal_connect(G_OBJECT (app->window), "expose-event",
	                  G_CALLBACK(set_image), app);
}


gboolean set_image(GtkWidget *widget, GdkEventButton *event, APP *app) {
	gint width, height;
	GError *error = NULL;

	//g_print("set_image\n");
	gtk_window_get_size(GTK_WINDOW(app->window), &width, &height);
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale("logo.jpg", width, height, TRUE, &error);
	if (error != NULL) {
		g_print("Error: %s\n", error->message);
		error = NULL;
		return FALSE;
	}
	gtk_image_set_from_pixbuf(GTK_IMAGE(app->image), pixbuf);
	g_object_unref(pixbuf);
	return FALSE;
}

gboolean callback_btn_dl(GtkWidget *widget, APP *app) {
	if(get_new_image())
		set_image(NULL, NULL, app);
	return FALSE;
}

gboolean callback_btn_save(GtkWidget *widget, APP *app) {
	return FALSE;
}

int main(int argc, char **argv) {

	struct _APP app;

	gtk_init (&argc, &argv);

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
