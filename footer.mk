#
# Generic PHONY target
#
# > must include globals.mk first
#

.PHONY: clean doc test help

clean:
	@rm -f *~ \#*\#
	@rm -rf $(BIN_DIR) $(DOC_DIR)

doc:
	cd $(BASEDIR) && doxygen Doxyfile
	open $(DOC_DIR)index.html

test:
	$(MAKE) -C $(TST_DIR) run-test

help:
	@echo "make        : compile only sources"
	@echo "make clean  : clean all"
	@echo "make doc    : generate Doxygen files (html)"
	@echo "make help   : print help"
	@echo "make test   : compile and run tests"
