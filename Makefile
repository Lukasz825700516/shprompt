PROGNAME = shprompt
LDFLAGS = $(shell pkg-config --libs libgit2)
CFLAGS = $(shell pkg-config --cflags libgit2) -O3
OBJECTS = \
	main.o

all: $(PROGNAME)

$(PROGNAME): $(OBJECTS)
	gcc $(CFLAGS) $(LDFLAGS) -o $(PROGNAME) $(OBJECTS)

$(OBJECTS): Makefile

.c.o:
	gcc -c $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@ $<

clean:
	rm *.o $(PROGNAME)
