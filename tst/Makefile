BASEDIR = ../
include ../header.mk



EXEC=test


$(EXEC): main.c
	$(CC) $(CFLAGS) $(CUNIT) main.c -o $(EXEC)



TEST_OUTPUT=test-suite
TEST_SUITE_URL=http://www.schaik.com/pngsuite/PngSuite-2017jul19.tgz

prepare: $(TEST_OUTPUT)

$(TEST_OUTPUT):
	mkdir $(TEST_OUTPUT)
	curl $(TEST_SUITE_URL) --output test-suite.tar --progress-bar
	tar zxf test-suite.tar --directory $(TEST_OUTPUT)
	rm test-suite.tar



include ../footer.mk
