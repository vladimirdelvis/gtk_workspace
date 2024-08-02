#include <gtk/gtk.h>
#include <stdlib.h>

struct abc{
    GtkEntry *text;
    GtkWindow *window;
    GtkCalendar *calendar;
};

static const char *buttons[] = {"Hayır","Evet",NULL}; // !! ONEMLI NULL TERMINATED OLMASI GEREKIYOR. YOKSA UZUNLUGUNU BILINEMEZ.


static void selection_callback(GObject *source_object,GAsyncResult *res,gpointer data){
    gboolean result = gtk_alert_dialog_choose_finish(GTK_ALERT_DIALOG(source_object),res,NULL);
    if (result == 1)
        gtk_window_destroy(GTK_WINDOW(data));
}


static gboolean closing(GtkWindow *self, gpointer *data){
    GtkAlertDialog* selection = gtk_alert_dialog_new("Çıkmak istediğinizden emin misiniz?");
    gtk_alert_dialog_set_buttons(selection,buttons);
    gtk_alert_dialog_choose(selection,self,NULL,selection_callback,self);
    return TRUE;
}


static void clicked(struct abc *b){
    //gtk_text_set_buffer(b->text,gtk_text_get_buffer(b->text));
    GtkAlertDialog* alert = gtk_alert_dialog_new("%s %s",gtk_entry_buffer_get_text(gtk_entry_get_buffer(b->text)),g_date_time_format_iso8601(gtk_calendar_get_date(b->calendar)));
    gtk_alert_dialog_show(alert,b->window);
    g_print("%s\n",gtk_alert_dialog_get_message(alert));
}

static void showed(GtkApplication *app, gpointer data){
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window),"Bu bir penceredir");
    gtk_window_set_default_size(GTK_WINDOW(window),500,500);
    gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
    
    //gtk_window_set_icon_name(GTK_WINDOW(window1),);

    GtkEntryBuffer *buff1;
    GtkWidget *grid1,*text1,*button1,*calendar;
    grid1 = gtk_grid_new();
    button1 = gtk_button_new_with_label("Basınız");
    buff1 = gtk_entry_buffer_new(NULL,-1);
    text1 = gtk_entry_new_with_buffer(buff1);
    calendar = gtk_calendar_new();

    gtk_grid_set_column_homogeneous(GTK_GRID(grid1),TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid1),TRUE);

    gtk_grid_attach(GTK_GRID(grid1),text1,0,0,2,1);
    gtk_grid_attach(GTK_GRID(grid1),button1,2,0,1,1);
    gtk_grid_attach(GTK_GRID(grid1),calendar,0,1,3,1);
    /*
    GRID MANTIGI
    EGER BIR CELLE WIDGET ATTACHLANACAKSA
    KESINLIKLE O CELLIN HER HANGI BIR KOMSUSUNDA (CAPRAZ DAHIL) BIR WIDGET BULUNMALIDIR!
    VE EGER BIR ROW VEYA COLUMN VAR OLMAYA DEVAM EDECEKSE ICERISINDE EN AZ 1 ADET WIDGET BULUNMALIDIR. 
    */

    struct abc *temp = malloc(sizeof(struct abc));
    *temp = (struct abc){GTK_ENTRY(text1),GTK_WINDOW(window),GTK_CALENDAR(calendar)};

    g_signal_connect_swapped(GTK_BUTTON(button1),"clicked",G_CALLBACK(clicked),temp);

    gtk_entry_set_placeholder_text(GTK_ENTRY(text1),"Buraya bir metin giriniz...");
    gtk_button_set_has_frame(GTK_BUTTON(button1),1);

    gtk_window_set_child(GTK_WINDOW(window),grid1);
    g_signal_connect(GTK_WINDOW(window),"close-request",G_CALLBACK(closing),NULL);
    gtk_window_present(GTK_WINDOW(window));
    gtk_window_set_interactive_debugging(TRUE);
}

int main(int argc,char **argv){
    GtkApplication *app = gtk_application_new("org.gtk.deneme",G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(G_APPLICATION(app),"activate",G_CALLBACK(showed), NULL);
    g_application_run(G_APPLICATION(app),argc,argv);
    g_object_unref(G_APPLICATION(app));
}