ifndef PLATFORM
$(error PLATFORM is not defined)
endif

CC = clang
CFLAGS = -c -Wall
LDFLAGS = -shared

ifeq ($(PLATFORM), linux)
OPT_FILE = _stock.so
PYTHON_INCLUDE_DIR = /usr/include/python3.5
PYTHON_LDFLAGS = 
CFLAGS += -fpic
endif

ifeq ($(PLATFORM), cygwin)
OPT_FILE = _stock.dll
PYTHON_INCLUDE_DIR = /usr/include/python3.6m
PYTHON_LDFLAGS = -l python3.6m -L /usr/lib
endif


OPT_DIR = .

SRC_DIR = src
OBJ_DIR = objs
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))

SWIG_I = stock.i
SWIG_WRAP_C = swig_wrap.c
SWIG_WRAP_O = swig_wrap.o
SWIG_WRAP_C_DIR = $(SRC_DIR)/swig_gen

.PHONY: setup

all: $(OBJ_FILES)
	@echo '$(SWIG_WRAP_C_DIR)/$(SWIG_WRAP_C) -> $(OBJ_DIR)/$(SWIG_WRAP_O)'
	$(CC) $(CFLAGS) $(SWIG_WRAP_C_DIR)/$(SWIG_WRAP_C) -o $(OBJ_DIR)/$(SWIG_WRAP_O) -I $(PYTHON_INCLUDE_DIR) -I $(SRC_DIR)
	@echo
	@echo '.o -> $(OPT_FILE)'
	$(CC) $(LDFLAGS) $(OBJ_DIR)/*.o -o $(OPT_DIR)/$(OPT_FILE) $(PYTHON_LDFLAGS)
	@echo
	@echo 'done'

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c setup
	@echo '$< -> $@'
	$(CC) $(CFLAGS) $< -o $@ -I $(PYTHON_INCLUDE_DIR)
	@echo

setup: clean
	@mkdir -p $(OBJ_DIR) $(SWIG_WRAP_C_DIR)
	swig -python -o $(SWIG_WRAP_C_DIR)/$(SWIG_WRAP_C) -outdir $(OPT_DIR) $(SRC_DIR)/$(SWIG_I)
	@echo

clean:
	@rm -rf $(OBJ_DIR) $(SWIG_WRAP_C_DIR)

