# tool macros
CC = g++
CU = nvcc
# CXX ?= # FILL: the compiler
CFLAGS := -std=c++20 -O3
CUFLAGS := -std=c++17 -I/usr/local/cuda/samples/common/inc -O3
# CXXFLAGS := # FILL: compile flags
DBGFLAGS := -g
COBJFLAGS := $(CFLAGS) -c -Ofast
CUOBJFLAGS := $(CUFLAGS) -std=c++17 -c -arch=sm_60 -O3

# path macros
BIN_PATH := bin
OBJ_PATH := obj
SRC_PATH := src
DBG_PATH := debug
SAVE_PATH:= saved

# compile macros
TARGET_NAME := out # change for binary name
ifeq ($(OS),Windows_NT)
	TARGET_NAME := $(addsuffix .exe,$(TARGET_NAME))
endif
UTARGET_NAME := cuda # change for binary name
ifeq ($(OS),Windows_NT)
	UTARGET_NAME := $(addsuffix .exe,$(UTARGET_NAME))
endif
TARGET := $(BIN_PATH)/$(TARGET_NAME)
UTARGET := $(BIN_PATH)/$(UTARGET_NAME)
TARGET_DEBUG := $(DBG_PATH)/$(TARGET_NAME)

COUNT_SAVE := $(shell ls $(SAVE_PATH) -1 2>/dev/null | wc -l)
TARGET_SAVE := $(SAVE_PATH)/$(TARGET_NAME)$(COUNT_SAVE)

# src files & obj files
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.cpp*)))
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))
USRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.cu*)))
OBJ_DEBUG := $(addprefix $(DBG_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

# clean files list
DISTCLEAN_LIST := $(OBJ) \
                  $(OBJ_DEBUG)
CLEAN_LIST := $(TARGET) \
			  $(UTARGET) \
			  $(TARGET_DEBUG) \
			  $(DISTCLEAN_LIST)

# default rule
default: makedir all

# non-phony targets
$(TARGET): $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

$(UTARGET): $(USRC)
	$(CU) $(CUFLAGS) -o $@ $(USRC)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp*
	$(CC) $(COBJFLAGS) -o $@ $<

$(DBG_PATH)/%.o: $(SRC_PATH)/%.cpp*
	$(CC) $(COBJFLAGS) $(DBGFLAGS) -o $@ $<

$(TARGET_DEBUG): $(OBJ_DEBUG)
	$(CC) $(CFLAGS) $(DBGFLAGS) $(OBJ_DEBUG) -o $@

# phony rules
.PHONY: makedir
makedir:
	@mkdir -p $(BIN_PATH) $(OBJ_PATH) $(DBG_PATH) $(SAVE_PATH)

.PHONY: all
all: $(TARGET) $(UTARGET)

.PHONY: run
run: $(TARGET)
	./$(TARGET)

.PHONY: debug
debug: $(TARGET_DEBUG)

.PHONY: save
save: $(TARGET)
	@echo $(TARGET_SAVE)
	@mkdir -p $(SAVE_PATH)
	@cp -n $(TARGET) $(TARGET_SAVE)

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	rm -f $(CLEAN_LIST)

.PHONY: distclean
distclean:
	@echo CLEAN $(DISTCLEAN_LIST)
	rm -f $(DISTCLEAN_LIST)