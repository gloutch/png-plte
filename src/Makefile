BASEDIR = ../
include ../header.mk



HEADERS = $(wildcard *.h)
SOURCES = $(wildcard *.c)
OBJECTS = $(addprefix $(BIN_DIR), $(SOURCES:.c=.o))

.PHONY: all

all: main.c $(HEADERS) $(OBJECTS)
	$(CC) $(CFLAGS) $(ZLIB) $(SDL) $(OBJECTS) -o $(TARGET)

$(OBJECTS): $(BIN_DIR)%.o : $(SRC_DIR)%.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@



include ../footer.mk
