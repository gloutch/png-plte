#
# Generic PHONY target
#
# > must include globals.mk first
#

.PHONY: distclean clean doc test cov help

distclean: clean
	@rm -rf $(BIN_DIR) $(DOC_DIR)
	@rm -rf $(LIB_DIR)zlib.h $(LIB_DIR)libz.a $(LIB_DIR)zlib-*
	@rm -rf $(TST_DIR)suite

clean:
	@rm -f *~ \#*\# *.bmp
	@rm -f $(TARGET_EXEC) $(TARGET_TEST)
	@rm -f $(BIN_DIR)*.gcda

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
	@echo "make           : compile only sources ($(TARGET))"
	@echo "make clean     : clean compilation files"
	@echo "make distclean : reset the folder as fresh new"
	@echo "make doc       : generate Doxygen files (html)"
	@echo "make test      : compile and run tests (maybe use LOG=NONE)"
	@echo "make cov       : recompile all sources and run tests silently, then print coverage"
	@echo
