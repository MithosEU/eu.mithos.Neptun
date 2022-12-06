#ifndef COMMON_H
#define COMMON_H

extern GtkBuilder* builder;
extern char* execPath;

void init_window(GtkApplication*);

//Actions
void enter_adress_dialog();
void load_adress();
void go_back();
void go_forward();
void load_overview();
void add_tab();
void close_tab();
void quit();

#endif
