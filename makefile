# Makefile for C++ OOP Project (Optimized & Recursive)

# Compiler settings
CXX      := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -I include

# Directories
SRC_DIR     := src
OBJ_DIR     := build
BIN_DIR     := bin
INCLUDE_DIR := include
DATA_DIR    := data
CONFIG_DIR  := config

# Target executable
TARGET := $(BIN_DIR)/game

# ── CLI build (default) ───────────────────────────────────────────────────────
# Exclude RaylibUI.cpp from the normal build (no Raylib dependency)
SRCS := $(shell find $(SRC_DIR) -name '*.cpp' -not -name 'RaylibUI.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# ── GUI build (bonus) ─────────────────────────────────────────────────────────
# Supports two raylib locations:
#   1. System-wide  : sudo apt install libraylib-dev
#   2. Local bundle : place raylib files in lib/raylib/include & lib/raylib/lib
RAYLIB_LOCAL := lib/raylib
RAYLIB_CFLAGS := -DUSE_RAYLIB
ifneq ($(wildcard $(RAYLIB_LOCAL)/include/raylib.h),)
  # local copy found → link static .a directly so no LD_LIBRARY_PATH needed
  RAYLIB_CFLAGS  += -I$(RAYLIB_LOCAL)/include
  RAYLIB_LDFLAGS := $(RAYLIB_LOCAL)/lib/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
else
  # fall back to system installation (static preferred)
  RAYLIB_LDFLAGS := -Wl,-Bstatic -lraylib -Wl,-Bdynamic -lGL -lm -lpthread -ldl -lrt -lX11
endif
GUI_TARGET     := $(BIN_DIR)/game-gui
GUI_SRCS       := $(SRCS) $(SRC_DIR)/views/RaylibUI.cpp
GUI_OBJS       := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/gui/%.o, $(GUI_SRCS))

# ── Main targets ──────────────────────────────────────────────────────────────
all: directories $(TARGET)

gui: directories $(GUI_TARGET)
	@echo "GUI build successful! Run with: ./$(GUI_TARGET) --gui"

run: all
	./$(TARGET)

run-gui: gui
	./$(GUI_TARGET) --gui

# ── Directory creation ────────────────────────────────────────────────────────
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(DATA_DIR) $(CONFIG_DIR)
	@mkdir -p $(OBJ_DIR)/gui/core $(OBJ_DIR)/gui/models \
	          $(OBJ_DIR)/gui/utils $(OBJ_DIR)/gui/views

# ── CLI link ──────────────────────────────────────────────────────────────────
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Build successful! Executable is at $(TARGET)"

# ── GUI link ──────────────────────────────────────────────────────────────────
$(GUI_TARGET): $(GUI_OBJS)
	$(CXX) $(CXXFLAGS) $(RAYLIB_CFLAGS) $^ -o $@ $(RAYLIB_LDFLAGS)
	@echo "GUI build successful! Executable is at $(GUI_TARGET)"

# ── CLI compile ───────────────────────────────────────────────────────────────
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ── GUI compile (all files, including RaylibUI.cpp) ───────────────────────────
$(OBJ_DIR)/gui/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(RAYLIB_CFLAGS) -c $< -o $@

# ── Utility ───────────────────────────────────────────────────────────────────
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Cleaned up $(OBJ_DIR) and $(BIN_DIR)"

rebuild: clean all

.PHONY: all gui run run-gui clean rebuild directories