#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <curl/curl.h>

GtkWidget *image;
GtkWidget *window;
GError *error = NULL;

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

int set_image(GtkWidget *widget, GtkWidget *entry) {
	gint width, height;

	//g_print("set_image\n");
	gtk_window_get_size(GTK_WINDOW(window), &width, &height);
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale("logo.jpg", width, height, TRUE, &error);
	if (error != NULL) {
		g_print("Error: %s\n", error->message);
		error = NULL;
		return FALSE;
	}
	gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
	g_object_unref(pixbuf);
	return FALSE;
}

int callback_btn_dl(GtkWidget *widget, GtkWidget *entry) {
	if(get_new_image())
		set_image(NULL, NULL);
	return FALSE;
}

int main(int argc, char **argv) {
	GtkWidget *vbox, *hbox;
	GtkWidget *btn_dl, *btn_save;
	GtkWidget *entry;

	gtk_init (&argc, &argv);

	/* creatin stuff */
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(GTK_WIDGET (window), 300, 300);
	gtk_window_set_title(GTK_WINDOW (window), "gtkPview");
	vbox = gtk_vbox_new(FALSE, 5);
	hbox = gtk_hbox_new(FALSE, 0);
	btn_dl = gtk_button_new_with_label("Download");
	btn_save = gtk_button_new_with_label("Save pic");
	entry = gtk_entry_new ();
	image = gtk_image_new();

	/* putting stuff together */
	gtk_container_add (GTK_CONTAINER (window), vbox);
	gtk_box_pack_start (GTK_BOX (vbox), image, TRUE, TRUE, 0);
	//gtk_box_pack_start (GTK_BOX (vbox), btn_dl, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), btn_dl, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), btn_save, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), entry, TRUE, TRUE, 0);

	/* Connect signals */
	g_signal_connect(G_OBJECT (window), "destroy",
	                  G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT (btn_dl), "clicked",
	                  G_CALLBACK(callback_btn_dl), NULL);
	g_signal_connect(G_OBJECT (window), "expose-event",
	                  G_CALLBACK(set_image), NULL);

	/* make visable */
	gtk_widget_show_all(window);
	//gtk_widget_show(vbox);
	//gtk_widget_show(hbox);
	//gtk_widget_show(btn_dl);
	//gtk_widget_show(btn_save);
	//gtk_widget_show(entry);
	//gtk_widget_show(image);
	//gtk_widget_show(window);

	/* run */
	gtk_main();

	return 0;
}
