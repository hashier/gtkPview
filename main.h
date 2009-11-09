#ifndef __main_h__
#define __main_h__

#define STATE_FULLSCREEN 0
#define STATE_NORMAL 1

#define RUNNING 0
#define STOPPED 1

typedef struct _APP {
	GtkWidget *vbox, *hbox, *hbox2, *vbox_mid, *hbox_mid;
	GtkWidget *btn_dl, *btn_save, *btn_save_all;
	GtkWidget *eventbox;
	GtkWidget *entry;
	GtkWidget *image;
	GtkWidget *window;
	GList *list;
	GtkWidget *btn_prev;
	GtkWidget *btn_next;
	GdkPixbuf *pixbuf;
	GdkPixbuf *scaled;
	GList *current;

	// Fullscreen or not?
	int state;
	int slideshow;
	int slideshow_stopped_on_the_fly;

	// Time in milliseconds in slideshow
	guint slideshow_timeout;

	guint width, height;

	GtkWidget *label_text;
} APP;

// These tells if we should load next or previous image from list
enum _Direction {
	DIR_PREVIOUS = 0,
	DIR_NEXT,
	DIR_CURRENT
} Direction;

static void update_title( APP *app );
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
static gboolean callback_btn_next(GtkWidget *widget, APP *app);
static gboolean start_up(APP *app);
static gboolean slideshow_next( APP *app );
void toggle_window_state( int state, APP *app );
void show_help();
gboolean resize_window( GtkWidget *w, GdkEvent *e, APP *app );
void get_scaled(APP *app, char direction);

#endif /* __main_h__ */
