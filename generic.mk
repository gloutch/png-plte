#
# Generic PHONY target
#
# > must include globals.mk first
#

.PHONY: clean doc

clean:
	@rm -f *~ \#*\#
	@rm -rf $(BIN_DIR) $(DOC_DIR)

doc:
	cd $(BASEDIR) && doxygen Doxyfile
	open $(DOC_DIR)index.html
