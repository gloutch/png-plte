BASEDIR = ../
include ../header.mk



SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(addprefix $(BIN_DIR), $(HEADERS:.h=.o))

.PHONY: all

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) main.c -o $(TARGET)

$(OBJECTS): $(BIN_DIR)%.o : $(SRC_DIR)%.c $(SRC_DIR)%.h
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@



include ../footer.mk
