#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include "common.h"

GtkWidget* mainWindow;
GtkWidget* mainStack;
GtkWidget* currentWebView;
int tabCount = 0;

void fix_helper_transient(GtkWidget* dialog){
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(mainWindow));
	gtk_window_set_application(GTK_WINDOW(dialog), gtk_window_get_application(GTK_WINDOW(mainWindow)));
}

void close_dialog(GtkWidget*, GtkWidget* dialog){
	gtk_widget_hide(dialog);
}

void go_back(){
	webkit_web_view_go_back(WEBKIT_WEB_VIEW(currentWebView));
}

void go_forward(){
	webkit_web_view_go_forward(WEBKIT_WEB_VIEW(currentWebView));
}

void change_tab(gpointer name){
	gtk_stack_set_visible_child_name(GTK_STACK(mainStack), name);
	
	GtkWidget* currentTab = gtk_stack_get_visible_child(GTK_STACK(mainStack));
	currentWebView = currentTab;
	
	gtk_widget_show(GTK_WIDGET(gtk_builder_get_object(builder, "navToolbar")));
}

void close_tab(){
	GtkWidget* currentTab = gtk_stack_get_visible_child(GTK_STACK(mainStack));
	
	if( g_strcmp0(gtk_stack_get_visible_child_name(GTK_STACK(mainStack)), "overview") != 0){
		gtk_stack_remove(GTK_STACK(mainStack), currentTab);
	}
		
	load_overview();
}

void quit(){
	gtk_window_destroy(GTK_WINDOW(mainWindow));
}

void load_overview_set_image(GObject* view, GAsyncResult* res, gpointer data){
	cairo_surface_t* surface = webkit_web_view_get_snapshot_finish(WEBKIT_WEB_VIEW(view), res, NULL);
	
	int height = gtk_widget_get_allocated_height(GTK_WIDGET(view));
	int width = gtk_widget_get_allocated_width(GTK_WIDGET(view));
	int offset= (width-height)/2;
	
	GdkPixbuf* pixbuf = gdk_pixbuf_scale_simple(gdk_pixbuf_get_from_surface(surface, offset, 0, height, height), 150, 150, GDK_INTERP_BILINEAR);
	gtk_image_set_from_pixbuf(GTK_IMAGE(data), pixbuf);
}

void do_web_search(GtkEntry* entry){
	add_tab();
	char* searchURL = g_strdup_printf("https://www.ecosia.org/search?q=%s", gtk_editable_get_text(GTK_EDITABLE(entry)));
	webkit_web_view_load_uri(WEBKIT_WEB_VIEW(currentWebView), searchURL);
	free(searchURL);
	gtk_editable_set_text(GTK_EDITABLE(entry), "");
}

void load_overview(){
	GtkWidget* overview = GTK_WIDGET(gtk_builder_get_object(builder, "overview"));
	GListModel* list = G_LIST_MODEL(gtk_stack_get_pages(GTK_STACK(mainStack)));
	int n = g_list_model_get_n_items(list);
	
	gtk_flow_box_bind_model(GTK_FLOW_BOX(overview), NULL, NULL, NULL, NULL);
	
	//for now use model in future
	for(int i=0; i<n; i++){
		GtkWidget* item = g_list_model_get_item(list, i);
		
		if( g_strcmp0(gtk_stack_page_get_name(GTK_STACK_PAGE(item)), "overview") != 0 ){
			GtkWidget* button = gtk_button_new();
			GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
			gtk_button_set_child(GTK_BUTTON(button), box);
			
			const char* full_title = gtk_stack_page_get_title(GTK_STACK_PAGE(item));
			if (full_title == NULL) full_title = "Unbekannt";
			char title[50];
			g_utf8_strncpy(title, full_title, 25);
			GtkWidget* label = gtk_label_new(title);
			gtk_box_append(GTK_BOX(box), label);
			
			GtkWidget* image =  gtk_image_new();
			webkit_web_view_get_snapshot(WEBKIT_WEB_VIEW(gtk_stack_page_get_child(GTK_STACK_PAGE(item))), WEBKIT_SNAPSHOT_REGION_VISIBLE, WEBKIT_SNAPSHOT_OPTIONS_NONE, NULL, (GAsyncReadyCallback) load_overview_set_image, (gpointer) image);
			gtk_image_set_pixel_size(GTK_IMAGE(image), 150);
			gtk_box_prepend(GTK_BOX(box), image);
			
			gtk_flow_box_append(GTK_FLOW_BOX(overview), button);
			g_signal_connect_swapped(button, "clicked", G_CALLBACK(change_tab), (gpointer) gtk_stack_page_get_name(GTK_STACK_PAGE(item)));
		}
	}
	
	//new tab button
	GtkWidget* button = gtk_button_new();
	gtk_actionable_set_action_name(GTK_ACTIONABLE(button), "app.add_tab");
	GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_button_set_child(GTK_BUTTON(button), box);
	
	GtkWidget* label = gtk_label_new("Neuer Reiter");
	gtk_box_append(GTK_BOX(box), label);
	
	GtkWidget* image =  gtk_image_new_from_icon_name("list-add-symbolic");
	gtk_image_set_pixel_size(GTK_IMAGE(image), 50);
	gtk_widget_set_margin_top(image, 50);
	gtk_widget_set_margin_bottom(image, 50);
	gtk_widget_set_margin_start(image, 50);
	gtk_widget_set_margin_end(image, 50);
	gtk_box_prepend(GTK_BOX(box), image);
	
	gtk_flow_box_append(GTK_FLOW_BOX(overview), button);
	
	gtk_stack_set_visible_child_name(GTK_STACK(mainStack), "overview");
	
	//hide toolbar
	gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(builder, "navToolbar")));
}

void enter_adress_dialog(){
	//test dialog
	GtkWidget* dialog1 = GTK_WIDGET(gtk_builder_get_object(builder, "dialog1"));
	gtk_window_present(GTK_WINDOW(dialog1));
	g_timeout_add_once(1000, (GSourceOnceFunc) fix_helper_transient, dialog1);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "te")), "clicked", G_CALLBACK(close_dialog), dialog1);
}

void load_adress(){
	GtkWidget* adressEntry = GTK_WIDGET(gtk_builder_get_object(builder, "adressEntry"));
	webkit_web_view_load_uri(WEBKIT_WEB_VIEW(currentWebView), gtk_editable_get_text(GTK_EDITABLE(adressEntry)));
	gtk_editable_set_text(GTK_EDITABLE(adressEntry), "");
	close_dialog(NULL, GTK_WIDGET(gtk_builder_get_object(builder, "dialog1")));
}

void set_title_co(WebKitWebView* view, WebKitLoadEvent, gpointer data){
	gtk_stack_page_set_title(GTK_STACK_PAGE(data), webkit_web_view_get_title(view));
}

void add_tab_fc(WebKitWebView*, WebKitNavigationAction* navAction){
	add_tab();
	const char* tabURL = webkit_uri_request_get_uri(webkit_navigation_action_get_request(navAction));
	webkit_web_view_load_uri(WEBKIT_WEB_VIEW(currentWebView), tabURL);
}

void add_tab(){
	GtkWidget* webView = webkit_web_view_new();
	GtkStackPage* page = gtk_stack_add_named(GTK_STACK(mainStack), webView, g_strdup_printf("tab%d", tabCount));
	g_signal_connect(WEBKIT_WEB_VIEW(webView), "load-changed", G_CALLBACK(set_title_co), (gpointer) page);
	webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webView), "https://mithoseu.github.io/main/");
	
	g_signal_connect(WEBKIT_WEB_VIEW(webView), "create", G_CALLBACK(add_tab_fc), NULL);
	
	currentWebView = webView;
	change_tab(g_strdup_printf("tab%d", tabCount));
	tabCount++;
}

void hide_toolbar(){
	gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(builder, "navToolbar")));
}

void apo_toolbar(GtkEventControllerMotion*, gdouble, gdouble y){
	if(y>100 && g_strcmp0(gtk_stack_get_visible_child_name(GTK_STACK(mainStack)), "overview") != 0){
		gtk_widget_show(GTK_WIDGET(gtk_builder_get_object(builder, "navToolbar")));
	}
}

void init_window(GtkApplication* app){
	gtk_builder_add_from_file(builder, g_strdup_printf("%s/share/neptun/window.ui", execPath), NULL);
	gtk_builder_add_from_file(builder, g_strdup_printf("%s/share/neptun/dialogs.ui", execPath), NULL);
	mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));
	mainStack = GTK_WIDGET(gtk_builder_get_object(builder, "mainStack"));
	gtk_window_set_application(GTK_WINDOW(mainWindow), app);
	gtk_window_present(GTK_WINDOW(mainWindow));
	load_overview();
	
	//toolbar
	GtkGesture* gesture = gtk_gesture_click_new();
	gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), 0);
	g_signal_connect(gesture, "released", G_CALLBACK(hide_toolbar), NULL);
	gtk_widget_add_controller(GTK_WIDGET(gtk_builder_get_object(builder, "navToolbar")), GTK_EVENT_CONTROLLER(gesture));
	GtkEventController* ecMove = gtk_event_controller_motion_new();
	g_signal_connect(ecMove, "motion", G_CALLBACK(apo_toolbar), NULL);
	gtk_widget_add_controller(GTK_WIDGET(gtk_builder_get_object(builder, "mainStack")), GTK_EVENT_CONTROLLER(ecMove));
	
	//search bar
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "searchBar")), "activate", G_CALLBACK(do_web_search), NULL);
}
