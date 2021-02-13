BASEDIR = ../
include ../header.mk



.PHONY: run-test prepare

run-test: $(TARGET_TEST)
	$(TARGET_TEST)

$(TARGET_TEST): $(TARGET) $(SUITE_FOLDER)
	$(CC) $(CFLAGS) $(CUNIT) main.c -o $(TARGET_TEST)


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
