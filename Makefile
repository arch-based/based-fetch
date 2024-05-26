# sfetch - suckless fetching utility

.POSIX:

include config.mk

all: sfetch

config.h:
	cp config.def.h config.h

sfetch: $(SRC)
	$(CC) $(SRC) -o sfetch
	
install: sfetch
	cp $(TARGETBIN) $(DESTDIR)

clean:
	rm -rf $(TARGETBIN)

uninstall:
	rm $(DESTDIR)/sfetch
