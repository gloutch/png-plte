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
TARGET_NAME = PngPlte
TARGET      = $(BIN_DIR)$(TARGET_NAME)

# makefile flood
VERBOSE = yes
ifeq ($(VERBOSE), yes)
MAKE += -w
else
MAKE += -s
endif

# compilation
CC      = cc
CFLAGS  = -std=c99 -Wall

# libraries
CUNIT = -lcunit
SDL   = -lSDL2
ZLIB  = -lz

# default target
$(TARGET):
	@$(MAKE) -C $(SRC_DIR) all
	@echo "> $(TARGET)"
