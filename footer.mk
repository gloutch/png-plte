#
# Generic PHONY target
#
# > must include globals.mk first
#

.PHONY: clean doc test cov help

clean:
	@rm -f *~ \#*\#
	@rm -rf $(BIN_DIR) $(DOC_DIR)

doc:
	cd $(BASEDIR) && doxygen Doxyfile | grep warning | echo "Warnings:"
	open $(DOC_DIR)index.html

test:
	$(MAKE) -C $(TST_DIR) run-test

cov:
	@echo "Re compiling..."
	@$(MAKE) --always-make $(TARGET) CFLAGS="$(CFLAGS) -O0 -fprofile-arcs -ftest-coverage" > /dev/null
	@echo "make test"
	@$(MAKE) test CFLAGS="$(CFLAGS) -O0 -fprofile-arcs -ftest-coverage" > /dev/null
	@echo
	@cd $(TST_DIR) && gcov ../$(BIN_DIR)*.gcno --no-output

help:
	@echo "make        : compile only sources"
	@echo "make clean  : clean compilation"
	@echo "make doc    : generate Doxygen files (html)"
	@echo "make help   : print help"
	@echo "make test   : compile and run tests (maybe use LOG=NONE)"
	@echo "make cov    : recompile sources and run test silently, then print coverage"
