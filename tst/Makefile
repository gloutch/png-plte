BASEDIR = ../
include ../header.mk


# list of files in this directory
TST_HEADERS = $(wildcard *.h)
TST_SOURCES = $(wildcard *.c)

# list of files in src directory
SRC_HEADERS = $(wildcard $(SRC_DIR)*.h)
SRC_SOURCES = $(wildcard $(SRC_DIR)*.c)

# all .c files without main.c
ALL_SRC = $(filter-out $(SRC_DIR)main.c, $(SRC_SOURCES)) $(TST_SOURCES)



.PHONY: run-test run-suite prepare

run-test: prepare $(TARGET_TEST) 
	$(TARGET_TEST)

$(TARGET_TEST): $(TST_HEADERS) $(SRC_SOURCES) $(ALL_SRC)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(CUNIT) $(ZLIB) $(SDL) -I$(SRC_DIR) $(ALL_SRC) -o $(TARGET_TEST)



# Download png test suite
SUITE_FOLDER=suite
TEST_SUITE_URL=http://www.schaik.com/pngsuite/PngSuite-2017jul19.tgz

prepare: $(SUITE_FOLDER)

$(SUITE_FOLDER):
	mkdir $(SUITE_FOLDER)
	curl $(TEST_SUITE_URL) --output suite.tar --progress-bar
	tar zxf suite.tar --directory $(SUITE_FOLDER)
	rm suite.tar

run-suite: run-suite.sh $(TARGET) $(SUITE_FOLDER)
	bash $< $(TARGET) $(SUITE_FOLDER)



include ../footer.mk
