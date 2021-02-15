BASEDIR = ../
include ../header.mk



HEADERS = $(wildcard *.h)
SOURCES = $(filter-out main.c, $(wildcard *.c))
OBJECTS = $(addprefix $(BIN_DIR), $(SOURCES:.c=.o))

.PHONY: all

all: main.c $(HEADERS) $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -DLOG_LEVEL=$(LOG) main.c -o $(TARGET)

$(OBJECTS): $(BIN_DIR)%.o : $(SRC_DIR)%.c $(SRC_DIR)%.h
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -DLOG_LEVEL=$(LOG) -c $< -o $@



include ../footer.mk
