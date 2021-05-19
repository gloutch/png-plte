BASEDIR = ../
include ../header.mk


HEADERS = $(wildcard *.h)
SOURCES = $(wildcard *.c)

all: main.c $(HEADERS) $(SOURCES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(ZLIB) $(SDL) $(SOURCES) -o $(TARGET_EXEC)

.PHONY: all


include ../footer.mk
