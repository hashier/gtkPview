#ifndef __main_h__
#define __main_h__

typedef struct _APP {
	GtkWidget *vbox, *hbox, *hbox2;
	GtkWidget *btn_dl, *btn_save;
	GtkWidget *entry;
	GtkWidget *image;
	GtkWidget *window;
	GList *list;
	GtkWidget *btn_prev;
	GtkWidget *btn_next;
	GdkPixbuf *pixbuf;
	GList *current;
} APP;

static gboolean callback_btn_dl(GtkWidget *widget, APP *app);
static gboolean callback_btn_save(GtkWidget *widget, APP *app);
static gboolean set_image(GtkWidget *widget, GdkEventButton *event, APP *app);
static void connect_signals(APP *app);
static size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);
static void quit_prog(GtkWidget *widget, GtkWidget *image);
static void create(APP *app);
static void makevisable(APP *app);
static void put(APP *app);
static int get_new_image(APP *app);
static gboolean callback_btn_prev(GtkWidget *widget, APP *app);
static gboolean callback_btn_next(GtkWidget *widget, APP *app);
static gboolean set_image(GtkWidget *widget, GdkEventButton *event, APP *app);

#endif /* __main_h__ */