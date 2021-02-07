TEST_OUTPUT=test-suite
TEST_SUITE_URL=http://www.schaik.com/pngsuite/PngSuite-2017jul19.tgz


run-ut:
	echo nothing.

prepare:
	mkdir $(TEST_OUTPUT)
	curl $(TEST_SUITE_URL) --output test-suite.tar --progress-bar
	tar zxf test-suite.tar --directory $(TEST_OUTPUT)


.PHONY: clean

clean:
	rm -f test-suite.tar
	rm -rf $(TEST_OUTPUT)
