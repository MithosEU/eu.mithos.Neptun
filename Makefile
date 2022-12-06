CC = gcc
PKGFLAGS = $(shell pkg-config --libs --cflags gtk4 webkit2gtk-5.0)
CFLAGS = -g -Wall -Wextra -rdynamic
SOURCES = $(wildcard src/*.c)
DATA = data
PREFIX = /usr/local

all:
	mkdir -p build/share/neptun/icons/hicolor/scalable/actions
	mkdir -p build/bin/
	
	gtk4-builder-tool validate $(DATA)/*.ui
	
	$(CC) $(SOURCES) $(CFLAGS) $(PKGFLAGS) -o build/bin/eu.mithos.Neptun
	
	cp $(DATA)/*.ui build/share/neptun/
	cp $(DATA)/icons/*.svg build/share/neptun/icons/hicolor/scalable/actions
	cp $(DATA)/twoModernExtra.css build/share/neptun/twoModernExtra.css

run:
	build/bin/eu.mithos.Neptun
	@echo ""

clean:
	rm -rf build
