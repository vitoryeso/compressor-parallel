SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
CXX := g++
PARAMETERS := -Wall
OPT_LEVEL := -O3
LIB := 
APP_NAME := compressor

all: build

$(OBJ_FILES): | $(OBJ_DIR)

$(OBJ_DIR):
	@echo Creating $(OBJ_DIR) directory
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	@echo Creating $(BIN_DIR) directory
	@mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo Compiling $<
	@$(CXX) $< -o $@ -c $(LIB) $(PARAMETERS) $(OPT_LEVEL)

build: $(OBJ_FILES) | $(BIN_DIR)
	@echo Linking $(BIN_DIR)/$(APP_NAME)
	@$(CXX) -o $(BIN_DIR)/$(APP_NAME) $(OBJ_FILES) $(LIB) $(PARAMETERS) $(OPT_LEVEL)

clean:
	@echo Cleaning
	@rm -rf $(BIN_DIR) $(OBJ_DIR)

.PHONY: all build clean
