SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
SRC_FILES_COMPRESSOR := $(filter-out $(SRC_DIR)/Decompressor_main.cpp, $(SRC_FILES))
SRC_FILES_DECOMPRESSOR := $(filter-out $(SRC_DIR)/Compressor_main.cpp, $(SRC_FILES))
CXX := g++
PARAMETERS := -Wall
#OPT_LEVEL := -O3
LIB := 
APP_NAME := compressor
APP_NAME_2 := decompressor

all: $(BIN_DIR)/$(APP_NAME) $(BIN_DIR)/$(APP_NAME_2)

compressor: $(BIN_DIR)/$(APP_NAME)

decompressor: $(BIN_DIR)/$(APP_NAME_2)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@$(CXX) $< -o $@ -c $(LIB) $(PARAMETERS) $(OPT_LEVEL)

$(BIN_DIR)/$(APP_NAME): $(SRC_FILES_COMPRESSOR) | $(BIN_DIR)
	@$(CXX) -o $@ $^ $(LIB) $(PARAMETERS) $(OPT_LEVEL)

$(BIN_DIR)/$(APP_NAME_2): $(SRC_FILES_DECOMPRESSOR) | $(BIN_DIR)
	@$(CXX) -o $@ $^ $(LIB) $(PARAMETERS) $(OPT_LEVEL)

clean:
	@rm -rf $(BIN_DIR) $(OBJ_DIR)
