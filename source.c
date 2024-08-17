#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

struct abc{
    // ANA PENCERE
    GtkEntry *text;
    GtkWindow *window;
    GtkCalendar *calendar;
    GtkApplication *app;

    // MODE 3 PENCERE
    GtkButton *btn;
    GtkProgressBar *bar;
    GtkSpinButton *spin;

};
static GtkWidget *rb1, *rb2, *rb3, *emp_label;
static const char *buttons[] = {"Hayır","Evet",NULL}; // !! ONEMLI NULL TERMINATED OLMASI GEREKIYOR. YOKSA UZUNLUGUNU BILINEMEZ.

/*static void set_pulse(GtkSpinButton *spn, struct abc *data){

}*/

static void md3_button_clicked(GtkButton *source, struct abc *data){
    gtk_progress_bar_set_fraction(data->bar,gtk_progress_bar_get_fraction(data->bar) + // ILERLEME CUBUGUNUN ILERLETILMESI
    (1/gtk_spin_button_get_value(data->spin)));
}

static void return_home(GtkWindow *home, GtkWindow *school){
    gtk_window_destroy(school);
    gtk_widget_set_visible(GTK_WIDGET(home),TRUE);
}


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

static void select_day(GtkCalendar *cl, struct abc *d){
    static gboolean kontrol = FALSE;
    static double zaman = 0;
    if(kontrol == FALSE){
        zaman = g_date_time_to_unix(gtk_calendar_get_date(cl));
        kontrol = TRUE;
    }
    else{
        zaman = difftime(zaman,g_date_time_to_unix(gtk_calendar_get_date(cl)));
        if(zaman < 0)
            zaman *= -1;
        GtkAlertDialog *dialog = gtk_alert_dialog_new("%s %u %s","Seçtiğiniz iki gün arasında",(unsigned long)zaman/86400,"gün vardır.");
        gtk_alert_dialog_choose(dialog,d->window,NULL,NULL,NULL);

        g_print("%u\n",(unsigned long)zaman/86400);

        gtk_widget_set_sensitive(rb1,TRUE);
        gtk_widget_set_sensitive(rb2,TRUE);
        g_signal_handlers_destroy(cl);

        zaman = 0;
        kontrol = FALSE;
    }
}

static void clicked(struct abc *d){
    //gtk_text_set_buffer(b->text,gtk_text_get_buffer(b->text));

    if(gtk_check_button_get_active(GTK_CHECK_BUTTON(rb1)) == TRUE){
        // MODE 1

        GtkAlertDialog *alert = gtk_alert_dialog_new("%s %s",gtk_entry_buffer_get_text(gtk_entry_get_buffer(d->text)),g_date_time_format_iso8601(gtk_calendar_get_date(d->calendar)));
        gtk_alert_dialog_choose(alert,d->window,NULL,NULL,NULL);
        g_print("%s\n",gtk_alert_dialog_get_message(alert));
    }
    else if(gtk_check_button_get_active(GTK_CHECK_BUTTON(rb2)) == TRUE){
        // MODE 2

        g_signal_handlers_destroy(d->calendar);
        g_signal_connect(d->calendar,"day-selected",G_CALLBACK(select_day),d);

        gtk_widget_set_sensitive(rb1,FALSE);
        gtk_widget_set_sensitive(rb2,FALSE);

        GtkAlertDialog *alert = gtk_alert_dialog_new("%s","İki adet gün seçiniz.");
        gtk_alert_dialog_choose(alert,d->window,NULL,NULL,NULL);
    }
    else if(gtk_check_button_get_active(GTK_CHECK_BUTTON(rb3)) == TRUE){
        // MODE 3

        GtkWidget *md3_window = gtk_application_window_new(d->app);
        gtk_window_set_title(GTK_WINDOW(md3_window),"Bu eski pencereden açılabilen bir başka penceredir");
        gtk_window_set_default_size(GTK_WINDOW(md3_window),500,100);
        gtk_window_set_resizable(GTK_WINDOW(md3_window),FALSE);

        // MODE 3 İÇİN AÇILAN PENCERENİN NESNELERİNİN AYARLARI

        GtkWidget *grid,*button,*progress_bar,*spin_button,*label,*box;
        label = gtk_label_new("TIKLAMA SAYISI");
        emp_label = gtk_label_new("");
        grid = gtk_grid_new();
        box = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
        button = gtk_button_new_with_label("Tıkla");
        progress_bar = gtk_progress_bar_new();
        spin_button = gtk_spin_button_new_with_range(1,99999999,1);

        gtk_widget_set_valign(progress_bar,GTK_ALIGN_CENTER);
        gtk_widget_set_size_request(gtk_widget_get_first_child(progress_bar),-1,32);
        gtk_widget_set_size_request(gtk_widget_get_first_child(gtk_widget_get_first_child(progress_bar)),-1,25);
        gtk_widget_set_margin_top(progress_bar,27);


        struct abc *temp_md3 = malloc(sizeof(struct abc));
        *temp_md3 = (struct abc){.btn=GTK_BUTTON(button),.bar=GTK_PROGRESS_BAR(progress_bar),.spin=GTK_SPIN_BUTTON(spin_button)};

        gtk_grid_attach(GTK_GRID(grid),progress_bar,0,0,3,1);
        gtk_grid_attach(GTK_GRID(grid),box,3,0,1,1);
        gtk_grid_attach(GTK_GRID(grid),emp_label,0,1,4,1); 
        gtk_grid_attach(GTK_GRID(grid),button,1,2,2,1);

        gtk_grid_set_column_homogeneous(GTK_GRID(grid),TRUE);
        gtk_grid_set_row_homogeneous(GTK_GRID(grid),TRUE);

        gtk_box_append(GTK_BOX(box),label);
        gtk_box_append(GTK_BOX(box),spin_button);

        /*
        0 1 2 3
        0 1 2 3
          1 2
        */

        gtk_window_set_child(GTK_WINDOW(md3_window),grid);

        g_signal_connect_swapped(GTK_WINDOW(md3_window),"close-request",G_CALLBACK(return_home),d->window);
        g_signal_connect(GTK_BUTTON(button),"clicked",G_CALLBACK(md3_button_clicked),temp_md3);
        //g_signal_connect(GTK_SPIN_BUTTON(spin_button),"value-changed",G_CALLBACK(set_pulse),temp_md3);

        gtk_window_present(GTK_WINDOW(md3_window));
        gtk_widget_set_visible(GTK_WIDGET(d->window),FALSE);
    }
}

static void showed(GtkApplication *app, gpointer data){

    // ANA PENCERENİN AYARLARI

    GtkEntryBuffer *buff1;
    GtkWidget *grid1,*text1,*button1,*calendar,*rb_box,*label_box;
    emp_label = gtk_label_new("");
    rb1 = gtk_check_button_new_with_label("MODE 1");
    rb2 = gtk_check_button_new_with_label("MODE 2");
    rb3 = gtk_check_button_new_with_label("MODE 3");
    rb_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    label_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,50);
    grid1 = gtk_grid_new();
    button1 = gtk_button_new_with_label("Seçilen modu çalıştır");
    buff1 = gtk_entry_buffer_new(NULL,-1);
    text1 = gtk_entry_new_with_buffer(buff1);
    calendar = gtk_calendar_new();

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window),"Bu bir penceredir");
    gtk_window_set_default_size(GTK_WINDOW(window),500,500);
    gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
    gtk_window_set_child(GTK_WINDOW(window),grid1);

    struct abc *temp = malloc(sizeof(struct abc));
    *temp = (struct abc){GTK_ENTRY(text1),GTK_WINDOW(window),GTK_CALENDAR(calendar),GTK_APPLICATION(app)};

    gtk_entry_set_placeholder_text(GTK_ENTRY(text1),"Buraya bir metin giriniz...");

    gtk_button_set_has_frame(GTK_BUTTON(button1),1);

    gtk_grid_set_column_homogeneous(GTK_GRID(grid1),TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid1),TRUE);

    gtk_grid_attach(GTK_GRID(grid1),text1,0,0,2,1);
    gtk_grid_attach(GTK_GRID(grid1),button1,2,0,1,1);
    gtk_grid_attach(GTK_GRID(grid1),calendar,0,1,3,1);
    gtk_grid_attach(GTK_GRID(grid1),label_box,3,0,1,1);
    /*
    GRID MANTIGI
    EGER BIR CELLE WIDGET ATTACHLANACAKSA
    KESINLIKLE O CELLIN HER HANGI BIR KOMSUSUNDA (CAPRAZ DAHIL) BIR WIDGET BULUNMALIDIR!
    VE EGER BIR ROW VEYA COLUMN VAR OLMAYA DEVAM EDECEKSE ICERISINDE EN AZ 1 ADET WIDGET BULUNMALIDIR. 
    */
    gtk_box_append(GTK_BOX(rb_box),rb1);
    gtk_box_append(GTK_BOX(rb_box),rb2);
    gtk_box_append(GTK_BOX(rb_box),rb3);

    gtk_box_append(GTK_BOX(label_box),emp_label);
    gtk_box_append(GTK_BOX(label_box),rb_box);

    gtk_check_button_set_group(GTK_CHECK_BUTTON(rb1),GTK_CHECK_BUTTON(rb2));
    gtk_check_button_set_group(GTK_CHECK_BUTTON(rb3),GTK_CHECK_BUTTON(rb2));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(rb1),TRUE);

    g_signal_connect(GTK_WINDOW(window),"close-request",G_CALLBACK(closing),NULL);
    g_signal_connect_swapped(GTK_BUTTON(button1),"clicked",G_CALLBACK(clicked),temp);

    gtk_window_present(GTK_WINDOW(window));
    gtk_window_set_interactive_debugging(TRUE);
}

int main(int argc,char **argv){
    GtkApplication *app = gtk_application_new("org.gtk.deneme",G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(G_APPLICATION(app),"activate",G_CALLBACK(showed), NULL);
    g_application_run(G_APPLICATION(app),argc,argv);
    g_object_unref(G_APPLICATION(app));
}