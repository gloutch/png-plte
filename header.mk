#
# Define constant for the whole project
#
# > define BASEDIR before include
#


# directories
SRC_DIR = $(BASEDIR)src/
TST_DIR = $(BASEDIR)tst/
BIN_DIR = $(BASEDIR)bin/
DOC_DIR = $(BASEDIR)doc/

# target
TARGET      = $(BIN_DIR)png-plte
TARGET_TEST = $(BIN_DIR)test

# makefile flood
VERBOSE = yes
ifeq ($(VERBOSE), yes)
MAKE += -w
else
MAKE += -s
endif

# compilation
CC     = cc
LOG    = INFO
CFLAGS = -std=c99 -Wall -DLOG_LEVEL=$(LOG)

# libraries
CUNIT = -lcunit
SDL   = -lSDL2
ZLIB  = -lz

# default target
$(TARGET): $(BASEDIR)header.mk $(SRC_DIR)*
	$(MAKE) -C $(SRC_DIR) all
	@echo "-> $(TARGET)"
