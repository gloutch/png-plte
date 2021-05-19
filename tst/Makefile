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
TEST_SUITE_FOLDER = suite
TEST_SUITE_URL    = http://www.schaik.com/pngsuite/PngSuite-2017jul19.tgz

prepare: $(TEST_SUITE_FOLDER)

$(TEST_SUITE_FOLDER):
	mkdir $@
	@echo "downloading test suite from $(TEST_SUITE_URL)"
	curl $(TEST_SUITE_URL) --output suite.tar --progress-bar
	tar zxf suite.tar --directory $@
	rm suite.tar

run-suite: run-suite.sh $(TARGET) $(TEST_SUITE_FOLDER)
	bash $< $(TARGET) $(TEST_SUITE_FOLDER)



include ../footer.mk
