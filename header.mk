#
# Define constant for the whole project
#
# > define BASEDIR before include
#


# directories
SRC_DIR = $(BASEDIR)src/
TST_DIR = $(BASEDIR)tst/
BIN_DIR = $(BASEDIR)bin/
LIB_DIR = $(BASEDIR)lib/
DOC_DIR = $(BASEDIR)doc/

# target
TARGET_EXEC = $(BIN_DIR)png-plte
TARGET_TEST = $(BIN_DIR)main-test

# makefile flood
VERBOSE = nope
ifeq ($(VERBOSE), yes)
MAKE += -w
else
MAKE += -s
endif

# compilation
LOG    = FATAL
COV    = -O0 -fprofile-arcs -ftest-coverage
CFLAGS = -std=c99 -Wall -DLOG_LEVEL=$(LOG)

# mandatory libraries
ZLIB  = -I$(LIB_DIR) -L$(LIB_DIR) -lz
# development libraries
CUNIT = -lcunit
SDL   = -lSDL2

# default target
$(TARGET_EXEC): $(BASEDIR)header.mk $(SRC_DIR)*
	@$(MAKE) -C $(LIB_DIR) all
	@$(MAKE) -C $(SRC_DIR) all
	@echo $@
