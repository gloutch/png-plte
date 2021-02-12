#
# Generic PHONY target
#
# > must include globals.mk first
#

.PHONY: clean doc help

clean:
	@rm -f *~ \#*\#
	@rm -rf $(BIN_DIR) $(DOC_DIR)

doc:
	cd $(BASEDIR) && doxygen Doxyfile
	open $(DOC_DIR)index.html

help:
	@echo "make        : compile only sources"
	@echo "make clean  : clean all"
	@echo "make doc    : generate Doxygen files (html)"
	@echo "make help   : print help"
	@echo "make test   : compile and run tests"
