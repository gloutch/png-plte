BASEDIR = ../
include ../header.mk



HEADERS = $(wildcard *.h)
SOURCES = $(wildcard *.c)

.PHONY: all

all: main.c $(HEADERS) $(SOURCES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(ZLIB) $(SDL) $(SOURCES) -o $(TARGET)


include ../footer.mk
