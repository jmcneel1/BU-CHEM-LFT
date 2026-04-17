# Compiler settings
CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Wpedantic

# Directories
SRC_DIR  := src
INC_DIR  := include
OBJ_DIR  := obj

# Includes and Source Files
CPPFLAGS := -I$(INC_DIR)
SRCS     := $(wildcard $(SRC_DIR)/*.cpp)
OBJS     := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS     := $(OBJS:.o=.d)

# Target
TARGET   := bu-lft

# ── Rules ─────────────────────────────────────────────────────────────
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Create object directory if it doesn't exist
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Include dependencies
-include $(DEPS)

# ── Clean ─────────────────────────────────────────────────────────────
clean:
	$(RM) -r $(OBJ_DIR)

distclean: clean
	$(RM) $(TARGET) output.tex

.PHONY: all clean distclean