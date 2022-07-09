SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
CFLAGS  = -Wall -Wextra -pedantic-errors -std=c99 -ggdb

toy: $(SOURCES) $(HEADERS)
	$(CC) -o $@ $(SOURCES) $(CFLAGS)

debug: $(SOURCES) $(HEADERS)
	$(CC) -o $@ $(SOURCES) $(CFLAGS) -D PRINT_DEBUG
