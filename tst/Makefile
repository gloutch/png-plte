BASEDIR = ../
include ../header.mk


# list of files in this directory
TST_HEADERS = $(wildcard *.h)
TST_SOURCES = $(wildcard *.c)
TST_OBJECTS = $(addprefix $(BIN_DIR), $(TST_SOURCES:.c=.o))

# list of files in src directory
SRC_HEADERS = $(wildcard $(SRC_DIR)*.h)
SRC_SOURCES = $(wildcard $(SRC_DIR)*.c)
SRC_OBJECTS = $(patsubst $(SRC_DIR)%.c, $(BIN_DIR)%.o, $(SRC_SOURCES))

# everythin except the main from src
ALL_OBJECTS = $(filter-out $(BIN_DIR)main.o, $(SRC_OBJECTS)) $(TST_OBJECTS)


.PHONY: run-test prepare

run-test: prepare $(TARGET_TEST)
	$(TARGET_TEST)

#
# assuming $(TARGET) compiles all src objects needed $(SRC_OBJECTS)
#
$(TARGET_TEST): $(TARGET) $(TST_OBJECTS)
	$(CC) $(CFLAGS) $(CUNIT) $(ZLIB) $(SDL) -I$(SRC_DIR) $(ALL_OBJECTS) -o $(TARGET_TEST)

$(TST_OBJECTS): $(BIN_DIR)%.o : $(TST_DIR)%.c
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@


# Download png test suite
SUITE_FOLDER=suite
TEST_SUITE_URL=http://www.schaik.com/pngsuite/PngSuite-2017jul19.tgz

prepare: $(SUITE_FOLDER)

$(SUITE_FOLDER):
	mkdir $(SUITE_FOLDER)
	curl $(TEST_SUITE_URL) --output suite.tar --progress-bar
	tar zxf suite.tar --directory $(SUITE_FOLDER)
	rm suite.tar



include ../footer.mk
