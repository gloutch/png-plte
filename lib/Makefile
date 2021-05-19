
ZLIB_VERSION = 1.2.11
ZLIB_FOLDER  = zlib-$(ZLIB_VERSION)/
ZLIB_URL     = http://zlib.net/zlib-$(ZLIB_VERSION).tar.gz

$(ZLIB_FOLDER):
	@echo "downloading $(ZLIB_FOLDER) from $(ZLIB_URL)"
	curl $(ZLIB_URL) --output zlib.tar.gz --progress-bar
	tar zxf zlib.tar.gz
	rm zlib.tar.gz

$(ZLIB_FOLDER)libz.a: $(ZLIB_FOLDER)
	cd $< && ./configure
	$(MAKE) -C $< static
	$(MAKE) -C $< teststatic

libz.a: $(ZLIB_FOLDER)libz.a
	cp $< $@

zlib.h: $(ZLIB_FOLDER)
	cp $</$@ $@



all: zlib.h libz.a
.PHONY: all
