BASEDIR = ../
include ../header.mk


# list of files in this directory
TST_HEADERS = $(wildcard *.h)
TST_SOURCES = $(filter-out main.c, $(wildcard *.c))
TST_OBJECTS = $(addprefix $(BIN_DIR), $(TST_SOURCES:.c=.o))

# list of files in src directory
SRC_HEADERS = $(wildcard $(SRC_DIR)*.h)
SRC_SOURCES = $(filter-out $(SRC_DIR)main.c, $(wildcard $(SRC_DIR)*.c))
SRC_OBJECTS = $(patsubst $(SRC_DIR)%.c, $(BIN_DIR)%.o, $(SRC_SOURCES))



.PHONY: run-test prepare

run-test: prepare $(TARGET_TEST)
	$(TARGET_TEST)

#
# assuming $(TARGET) compiles all src objects needed in $(BIN_DIR)
#
$(TARGET_TEST): $(TARGET) main.c $(TST_OBJECTS)
	$(CC) $(CFLAGS) $(CUNIT) -I$(SRC_DIR) $(BIN_DIR)*.o main.c -o $(TARGET_TEST)

$(TST_OBJECTS): $(BIN_DIR)%.o : $(TST_DIR)%.c $(TST_DIR)%.h
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@


# Download png test suite
SUITE_FOLDER=test-suite
TEST_SUITE_URL=http://www.schaik.com/pngsuite/PngSuite-2017jul19.tgz

prepare: $(SUITE_FOLDER)

$(SUITE_FOLDER):
	mkdir $(SUITE_FOLDER)
	curl $(TEST_SUITE_URL) --output test-suite.tar --progress-bar
	tar zxf test-suite.tar --directory $(SUITE_FOLDER)
	rm test-suite.tar



include ../footer.mk
