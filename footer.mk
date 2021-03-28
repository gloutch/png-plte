#
# Generic PHONY target
#
# > must include globals.mk first
#

.PHONY: clean doc test cov help

clean:
	@rm -f *~ \#*\# *.bmp
	@rm -rf $(BIN_DIR) $(DOC_DIR) $(BIN_DIR)*.gcda

doc:
	cd $(BASEDIR) && doxygen Doxyfile | grep warning | echo "Warnings:"
	open $(DOC_DIR)index.html

test:
	@$(MAKE) -C $(TST_DIR) run-test

cov:
	@rm -rf $(BIN_DIR)
	@echo "compiling + run test"
	@$(MAKE) test CFLAGS="$(CFLAGS) $(COV)" > /dev/null
	@echo
	@mv $(TST_DIR)*.gcno $(TST_DIR)*.gcda $(BIN_DIR)
	@cd $(TST_DIR) && gcov ../bin/*.gcno --no-output

help:
	@echo "make        : compile only sources ($(TARGET))"
	@echo "make clean  : clean compilation"
	@echo "make doc    : generate Doxygen files (html)"
	@echo "make test   : compile and run tests (maybe use LOG=NONE)"
	@echo "make cov    : recompile all sources and run tests silently, then print coverage"
	@echo
