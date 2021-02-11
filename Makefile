BASEDIR = ./
include globals.mk



.PHONY: main test

all:
	@$(MAKE) -C $(SRC_DIR)

test:
	@$(MAKE) -C $(TST_DIR)



include generic.mk
