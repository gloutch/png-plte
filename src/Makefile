BASEDIR = ../
include ../globals.mk



TARGET = $(addprefix $(BIN_DIR), $(EXEC_NAME))

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(addprefix $(BIN_DIR), $(HEADERS:.h=.o))


$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) main.c -o $(TARGET)

$(OBJECTS): $(BIN_DIR)%.o : $(SRC_DIR)%.c $(SRC_DIR)%.h
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@



include ../generic.mk
