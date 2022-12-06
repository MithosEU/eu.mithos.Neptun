#include <stdio.h>
#include <gtk/gtk.h>
#include "common.h"

char* execPath;
GtkBuilder* builder;

static GActionEntry actionEntries[] = {
	{ "enter_adress", enter_adress_dialog, NULL, NULL, NULL, {0,0,0} },
	{ "load_adress", load_adress, NULL, NULL, NULL, {0,0,0} },
	{ "go_back", go_back, NULL, NULL, NULL, {0,0,0} },
	{ "go_forward", go_forward, NULL, NULL, NULL, {0,0,0} },
	{ "load_overview", load_overview, NULL, NULL, NULL, {0,0,0} },
	{ "add_tab", add_tab, NULL, NULL, NULL, {0,0,0} },
	{ "close_tab", close_tab, NULL, NULL, NULL, {0,0,0} },
	{ "quit", quit, NULL, NULL, NULL, {0,0,0} },
};

static void on_activate(GtkApplication *app){
	//Set var with path to application data
	char buffer[1024];
	readlink("/proc/self/exe", buffer, sizeof(buffer));
	execPath = g_path_get_dirname(buffer);
	execPath[strlen(execPath)-4] = '\0';
	
	//Add custom icons
	GtkIconTheme* iconTheme = gtk_icon_theme_get_for_display(gdk_display_get_default());
	gtk_icon_theme_add_search_path(iconTheme, g_strdup_printf("%s/share/neptun/icons/", execPath));
	g_print("%s/share/jupiter/icons", execPath);
	
	//Set custom theme
	GtkCssProvider* cssProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(cssProvider, g_strdup_printf("%s/share/neptun/twoModernExtra.css", execPath));
	gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	
	//Make an common builder
	builder = gtk_builder_new();

	//load menu
	gtk_builder_add_from_file(builder, g_strdup_printf("%s/share/neptun/menu.ui", execPath), NULL);
	GMenuModel* windowMenuModel = G_MENU_MODEL(gtk_builder_get_object(builder, "windowMenuModel"));
	gtk_application_set_menubar(app, windowMenuModel);
	
	//setup actions
	g_action_map_add_action_entries(G_ACTION_MAP(app), actionEntries, G_N_ELEMENTS(actionEntries), app);

	//Load the two windows
	init_window(app);
}

//Just the starup itself, everything else is managed in on_activate
int main(int argc, char *argv[]){
	GtkApplication *app = gtk_application_new ("eu.mithos.Neptun", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);

	return(g_application_run(G_APPLICATION(app), argc, argv));
}
